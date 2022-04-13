#include "input_output.h"

#include <future>
#include <iostream>
#include <string>

#include "equation.h"
#include "extremum.h"

namespace equation {

namespace detail {

void ProduceCoefficients(std::string a, std::string b, std::string c, Queue& coefficients,
                         std::condition_variable& cv, std::mutex& m) {
    {
        std::lock_guard lk(m);
        coefficients.push(std::make_tuple(std::move(a), std::move(b), std::move(c)));
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
}

}  // namespace detail

void ReadTreyAndProduceCoefficients(int argc, char** argv, Queue& coefficients,
                                    ThreadHelper& thread_helper) {
    for (int i = 1; i < argc; i += 3) {
        detail::ProduceCoefficients(argv[i], argv[i + 1], argv[i + 2], coefficients,
                                    thread_helper.GetCVQueue(), thread_helper.GetMutexQueue());
    }

    thread_helper.SetInputEmpty();
}

void PrintRootsAndExtremum(std::ostream& out, std::vector<std::future<PrintResult>>& print_results,
                           ThreadHelper& thread_helper) {
    using namespace std::literals::string_literals;

    // wait until reading the whole input to not modify "print_results" anymore
    thread_helper.GetSemPrint().acquire();

    for (size_t i = 0; i < print_results.size(); ++i) {
        const std::variant<InputGarbageError, EquationResult>& result = print_results[i].get();

        if (const InputGarbageError* err = std::get_if<InputGarbageError>(&result)) {
            out << '(' << err->GetCoefficients() << ')';
            out << " => "s;
            out << err->what();
        } else if (const EquationResult* eq_res = std::get_if<EquationResult>(&result)) {
            detail::PrintCoeffs(out, eq_res->coeffs);
            out << " => "s;
            out << eq_res->roots << ' ' << eq_res->extremum;
        }

        out << '\n';
    }
}

}  // namespace equation
