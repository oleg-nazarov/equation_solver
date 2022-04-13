#pragma once

#include <future>
#include <queue>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include "equation.h"
#include "extremum.h"

namespace equation {

struct EquationResult {
    std::vector<int> coeffs;
    OneVariableEquation::Roots roots;
    ExtremumSolver::Extremum extremum;
};

class InputGarbageError : std::domain_error {
   public:
    InputGarbageError(std::string a, std::string b, std::string c);

    const char* what() const noexcept;

    std::string GetCoefficients() const;

   private:
    std::string coefficients_;
};

using Queue = std::queue<std::tuple<std::string, std::string, std::string>>;
using PrintResult = std::variant<InputGarbageError, EquationResult>;

// helper for passing variables for concurrency needs inside one convenient structure
struct ThreadHelper {
    ThreadHelper() = default;
    ThreadHelper(const ThreadHelper&) = delete;

    ThreadHelper& operator=(const ThreadHelper&) = delete;

    bool IsInputEmpty() const;
    void SetInputEmpty();

    std::mutex& GetMutexQueue();
    std::condition_variable& GetCVQueue();

    std::binary_semaphore& GetSemPrint();

   private:
    bool input_emptiness_{false};
    std::mutex m_queue_;
    std::condition_variable cv_queue_;

    std::binary_semaphore sem_print_{0};
};

void FindRootsAndExtremum(std::vector<std::future<PrintResult>>& print_results, Queue& coefficients,
                          ThreadHelper& thread_helper);

}  // namespace equation
