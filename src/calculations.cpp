#include "calculations.h"

#include <iterator>

namespace equation {

namespace detail {

const std::regex NUMBER_RE{R"(^\-?\d+$)"};

bool IsNotDigit(const std::string& s) {
    std::smatch sm;
    std::regex_search(s, sm, NUMBER_RE);

    return sm.size() == 0;
}

void CheckCoeffsForGarbage(const std::string& a, const std::string& b, const std::string& c) {
    if (IsNotDigit(a) || IsNotDigit(b) || IsNotDigit(c)) {
        throw InputGarbageError{a, b, c};
    }
}

std::vector<int> GetTreyCoefficients(const std::shared_ptr<OneVariableEquation>& equation_ptr) {
    if (dynamic_cast<Linear*>(equation_ptr.get())) {
        std::vector<int> new_coeffs{0};

        std::vector<int> coeffs = equation_ptr->GetCoefficients();
        new_coeffs.insert(new_coeffs.end(), std::move_iterator(coeffs.begin()), std::move_iterator(coeffs.end()));

        return new_coeffs;
    }

    return equation_ptr->GetCoefficients();
}

void CalculateAndWriteRootsAndExtremum(std::promise<PrintResult> result_promise,
                                       std::shared_ptr<OneVariableEquation> equation_ptr,
                                       std::shared_ptr<ExtremumSolver> extremum_ptr) {
    EquationResult equation_res;

    // 1. add coefficients

    // as we get equation coefficients as treys, we want to print them as treys too,
    // even if it is a linear equation ("0 4 -4")
    equation_res.coeffs = GetTreyCoefficients(equation_ptr);

    // 2. calculating roots
    equation_res.roots = equation_ptr->GetRoots();

    // 3. finding extremum
    equation_res.extremum = extremum_ptr->GetExtremum();

    result_promise.set_value(PrintResult{equation_res});
}

}  // namespace detail

// InputGarbageError

InputGarbageError::InputGarbageError(std::string a, std::string b, std::string c)
    : domain_error(a + ' ' + b + ' ' + c),
      coefficients_(a + ' ' + b + ' ' + c) {}

const char* InputGarbageError::what() const noexcept {
    return "input garbage";
}

std::string InputGarbageError::GetCoefficients() const {
    return coefficients_;
}

// ThreadHelper

bool ThreadHelper::IsInputEmpty() const {
    return input_emptiness_;
}
void ThreadHelper::SetInputEmpty() {
    input_emptiness_ = true;
}

std::mutex& ThreadHelper::GetMutexQueue() {
    return m_queue_;
}
std::condition_variable& ThreadHelper::GetCVQueue() {
    return cv_queue_;
}

std::binary_semaphore& ThreadHelper::GetSemPrint() {
    return sem_print_;
}

void FindRootsAndExtremum(std::vector<std::future<PrintResult>>& print_results, Queue& coefficients,
                          ThreadHelper& thread_helper) {
    auto& m_queue = thread_helper.GetMutexQueue();
    auto& cv_queue = thread_helper.GetCVQueue();

    std::shared_ptr<OneVariableEquation> equation_ptr;
    std::shared_ptr<ExtremumSolver> extremum_ptr;

    while (!(thread_helper.IsInputEmpty() && coefficients.empty())) {
        std::promise<PrintResult> new_result_promise;
        std::tuple<std::string, std::string, std::string> coeffs;

        {
            std::unique_lock lk(m_queue);

            cv_queue.wait(lk, [&coefficients]() {
                return !coefficients.empty();
            });

            // push future-object to read later from it an equation roots and extremum
            print_results.push_back(std::move(new_result_promise.get_future()));

            coeffs = std::move(coefficients.front());
            coefficients.pop();
        }

        std::string a_s = std::get<0>(coeffs);
        std::string b_s = std::get<1>(coeffs);
        std::string c_s = std::get<2>(coeffs);

        // check for input garbage
        try {
            detail::CheckCoeffsForGarbage(a_s, b_s, c_s);
        } catch (const InputGarbageError& err) {
            new_result_promise.set_value(PrintResult{err});

            continue;
        }

        // create equation and extremum_solver
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

        std::thread t(detail::CalculateAndWriteRootsAndExtremum,
                      std::move(new_result_promise), std::move(equation_ptr), std::move(extremum_ptr));

        t.detach();
    }

    thread_helper.GetSemPrint().release();
}

}  // namespace equation
