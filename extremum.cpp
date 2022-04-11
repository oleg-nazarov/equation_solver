#include "extremum.h"

#include <memory>
#include <sstream>
#include <utility>
#include <vector>

namespace equation {

namespace detail {

struct ExtremumPrinter {
    std::string operator()(std::nullptr_t) {
        using namespace std::literals::string_literals;
        return "no extremum"s;
    }

    std::string operator()(const std::vector<ExtremePoint>& extremum) {
        using namespace std::literals::string_literals;

        std::ostringstream oss;

        for (size_t i = 0; i < extremum.size(); ++i) {
            ExtremePoint extreme = extremum[i];

            if (extreme.type == detail::ExtremeType::X_MIN) {
                oss << "Xmin"s;
            } else {
                oss << "Xmax"s;
            }

            oss << '=';
            oss << extreme.x_point;

            if (i + 1 != extremum.size()) {
                oss << ' ';
            }
        }

        return oss.str();
    }
};

}  // namespace detail

std::ostream& operator<<(std::ostream& out, const ExtremumSolver::Extremum& extremum) {
    out << std::visit(detail::ExtremumPrinter{}, extremum);

    return out;
}

// ExtremumSolver

ExtremumSolver::ExtremumSolver(std::shared_ptr<OneVariableEquation> equation)
    : equation_ptr_(std::move(equation)) {}

ExtremumSolver::~ExtremumSolver() = default;

const ExtremumSolver::Extremum& ExtremumSolver::GetExtremum() {
    if (!extremum_.has_value()) {
        FindExtremum();
    }

    return *extremum_;
}

// ExtremumOfLinear

ExtremumOfLinear::ExtremumOfLinear(std::shared_ptr<OneVariableEquation> equation)
    : ExtremumSolver(std::move(equation)) {}

void ExtremumOfLinear::FindExtremum() {
    extremum_ = nullptr;
}

// ExtremumOfQuadratic

ExtremumOfQuadratic::ExtremumOfQuadratic(std::shared_ptr<OneVariableEquation> equation)
    : ExtremumSolver(std::move(equation)) {}

void ExtremumOfQuadratic::FindExtremum() {
    const std::vector<int>& coeffs = equation_ptr_->GetCoefficients();
    int a = coeffs[0];
    int b = coeffs[1];

    ExtremePoint extreme;
    extreme.type = a > 0 ? detail::ExtremeType::X_MIN : detail::ExtremeType::X_MAX;
    extreme.x_point = -b / (2. * a);

    extremum_ = std::vector{extreme};
}

}  // namespace equation
