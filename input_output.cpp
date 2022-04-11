#include "input_output.h"

#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <queue>
#include <regex>
#include <stdexcept>
#include <string>
#include <thread>

#include "equation.h"
#include "extremum.h"

namespace equation {

namespace detail {

const std::string INPUT_GARBAGE_S = "input garbage";

class InputGarbageError {
   public:
    InputGarbageError(std::string a, std::string b, std::string c)
        : coefficients_(a + ' ' + b + ' ' + c) {}

    const char* what() const noexcept {
        return INPUT_GARBAGE_S.data();
    }

    std::string GetCoefficients() const {
        return coefficients_;
    }

   private:
    std::string coefficients_;
};

const std::regex NUMBER_RE{R"(^\-?\d+$)"};

void CheckCoeffsForGarbage(const std::string& a, const std::string& b, const std::string& c) {
    {
        std::smatch sm;
        std::regex_search(a, sm, detail::NUMBER_RE);

        if (sm.size() == 0) {
            throw InputGarbageError{a, b, c};
        }
    }

    {
        std::smatch sm;
        std::regex_search(b, sm, detail::NUMBER_RE);

        if (sm.size() == 0) {
            throw InputGarbageError{a, b, c};
        }
    }

    {
        std::smatch sm;
        std::regex_search(c, sm, detail::NUMBER_RE);

        if (sm.size() == 0) {
            throw InputGarbageError{a, b, c};
        }
    }
}

void ProduceCoefficients(std::string a, std::string b, std::string c, Queue& q,
                         std::condition_variable& cv, std::mutex& m) {
    {
        std::lock_guard lk(m);
        // q.push(std::make_pair(std::move(equation), std::move(extremum)));
        q.push(std::make_tuple(std::move(a), std::move(b), std::move(c)));
    }

    cv.notify_one();
}

void PrintCoeffs(std::ostream& out, const std::vector<int>& coeffs) {
    using namespace std::literals::string_literals;
    out << '(';

    for (size_t i = 0; i < coeffs.size(); ++i) {
        out << coeffs[i];

        if (i + 1u != coeffs.size()) {
            out << ' ';
        }
    }

    out << ')';
    out << " => ";
}

void CalculateRootsAndExtremum(std::promise<std::vector<int>> coeffs_promise,
                               std::promise<OneVariableEquation::Roots> roots_promise,
                               std::promise<ExtremumSolver::Extremum> extremum_promise,
                               std::shared_ptr<OneVariableEquation> equation_ptr,
                               std::shared_ptr<ExtremumSolver> extremum_ptr) {
    // 1. calculating roots
    roots_promise.set_value(equation_ptr->GetRoots());

    // 2. calculating extremum
    extremum_promise.set_value(extremum_ptr->GetExtremum());

    // 3. add coeffs
    coeffs_promise.set_value(equation_ptr->GetCoefficients());
}

}  // namespace detail

void ReadAndPushCoefficients(int argc, char** argv, Queue& q, bool& input_is_empty,
                             std::condition_variable& cv, std::mutex& m) {
    for (int i = 1; i < argc; i += 3) {
        detail::ProduceCoefficients(argv[i], argv[i + 1], argv[i + 2], q, cv, m);
    }

    input_is_empty = true;
}

void CalculateRootsExtremumAndPrint(std::ostream& out, Queue& q, const bool& input_is_empty,
                                    std::condition_variable& cv, std::mutex& m) {
    std::vector<std::variant<detail::InputGarbageError, std::future<std::vector<int>>>> coeffs;
    std::vector<std::future<OneVariableEquation::Roots>> roots;
    std::vector<std::future<ExtremumSolver::Extremum>> extremum;

    std::shared_ptr<OneVariableEquation> equation_ptr;
    std::shared_ptr<ExtremumSolver> extremum_ptr;

    while (!(input_is_empty && q.empty())) {
        {
            std::unique_lock lk(m);

            cv.wait(lk, [&q]() {
                return !q.empty();
            });

            std::string a_s = std::get<0>(q.front());
            std::string b_s = std::get<1>(q.front());
            std::string c_s = std::get<2>(q.front());

            try {
                detail::CheckCoeffsForGarbage(a_s, b_s, c_s);
            } catch (const detail::InputGarbageError& err) {
                coeffs.push_back(err);

                q.pop();
                continue;
            }

            std::tuple<std::string, std::string, std::string> coeffs = std::move(q.front());
            int a = std::stoi(a_s);
            int b = std::stoi(b_s);
            int c = std::stoi(c_s);

            if (a == 0) {
                equation_ptr = std::make_shared<Linear>(b, c);
                extremum_ptr = std::make_shared<ExtremumOfLinear>(equation_ptr);
            } else {
                equation_ptr = std::make_shared<Quadratic>(a, b, c);
                extremum_ptr = std::make_shared<ExtremumOfQuadratic>(equation_ptr);
            }

            q.pop();
        }

        std::promise<std::vector<int>> coeffs_promise;
        std::promise<OneVariableEquation::Roots> roots_promise;
        std::promise<ExtremumSolver::Extremum> extremum_promise;

        coeffs.push_back(coeffs_promise.get_future());
        roots.push_back(roots_promise.get_future());
        extremum.push_back(extremum_promise.get_future());

        std::thread t(detail::CalculateRootsAndExtremum, std::move(coeffs_promise), std::move(roots_promise),
                      std::move(extremum_promise), std::move(equation_ptr), std::move(extremum_ptr));

        t.detach();
    }

    for (size_t i = 0; i < coeffs.size(); ++i) {
        if (detail::InputGarbageError* err = std::get_if<detail::InputGarbageError>(&(coeffs[i]))) {
            out << '(' << err->GetCoefficients() << ')';
            out << " => ";
            out << err->what();
        } else if (std::future<std::vector<int>>* ptr = std::get_if<std::future<std::vector<int>>>(&(coeffs[i]))) {
            detail::PrintCoeffs(out, ptr->get());
            out << roots[i].get() << ' ' << extremum[i].get();
        }

        out << '\n';
    }
}

}  // namespace equation
