#include "equation.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

namespace equation {

namespace detail {

struct RootsPrinter {
    std::string operator()(std::nullptr_t) {
        using namespace std::literals::string_literals;
        return "no roots"s;
    }

    std::string operator()(const std::string& s) {
        return s;
    }

    std::string operator()(const std::vector<int>& roots) {
        return GetRootsString(roots);
    }

    std::string operator()(const std::vector<double>& roots) {
        return GetRootsString(roots);
    }

   private:
    template <typename Number>
    std::string GetRootsString(const std::vector<Number>& roots) {
        using namespace std::literals::string_literals;

        std::ostringstream oss;
        oss << '(';

        for (size_t i = 0; i < roots.size(); ++i) {
            oss << roots[i];

            if (i + 1u != roots.size()) {
                oss << ", "s;
            }
        }

        oss << ')';

        return oss.str();
    }
};

}  // namespace detail

// OneVariableEquation

OneVariableEquation::~OneVariableEquation() = default;

OneVariableEquation::Roots OneVariableEquation::GetRoots() {
    if (!roots_.has_value()) {
        Solve();
    }

    return *roots_;
}

std::vector<int> OneVariableEquation::GetCoefficients() const {
    return coefficients_;
}

std::ostream& operator<<(std::ostream& out, const OneVariableEquation::Roots& roots) {
    out << std::visit(detail::RootsPrinter{}, roots);

    return out;
}

// Linear

Linear::Linear(int a, int b) : OneVariableEquation(a, b) {}

void Linear::Solve() {
    using namespace std::literals::string_literals;

    std::vector<int> coeffs = GetCoefficients();
    int a = coeffs[0];
    int b = coeffs[1];

    if (a == 0) {
        if (b == 0) {
            roots_ = "every number"s;
        } else {
            roots_ = nullptr;
        }

        return;
    }

    roots_ = std::vector{-b / static_cast<double>(a)};
}

// Quadratic

Quadratic::Quadratic(int a, int b, int c) : OneVariableEquation(a, b, c) {}

void Quadratic::Solve() {
    std::vector<int> coeffs = GetCoefficients();
    int a = coeffs[0];
    int b = coeffs[1];
    int c = coeffs[2];

    assert(a != 0);

    int d_pow_2 = b * b - 4 * a * c;
    double two_a = 2 * a;

    if (d_pow_2 < 0) {
        roots_ = nullptr;

        return;
    } else if (d_pow_2 == 0) {
        roots_ = std::vector{-b / two_a};

        return;
    }

    double d = std::sqrt(d_pow_2);

    double x_1 = (-b + d) / two_a;
    double x_2 = (-b - d) / two_a;
    std::vector<double> temp{x_1, x_2};
    sort(temp.begin(), temp.end());

    roots_ = std::move(temp);
}

}  // namespace equation
