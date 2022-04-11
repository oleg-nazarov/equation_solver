#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "equation.h"

namespace equation {

namespace detail {

enum class ExtremeType {
    X_MAX,
    X_MIN,
};

}  // namespace detail

struct ExtremePoint {
    detail::ExtremeType type;
    double x_point;
};

class ExtremumSolver {
   public:
    using Extremum = std::variant<std::nullptr_t, std::vector<ExtremePoint>>;
    virtual ~ExtremumSolver();

    const Extremum& GetExtremum();

   protected:
    ExtremumSolver(std::shared_ptr<OneVariableEquation> equation);

    std::shared_ptr<OneVariableEquation> equation_ptr_;
    std::optional<Extremum> extremum_;

   private:
    virtual void FindExtremum() = 0;
};

std::ostream& operator<<(std::ostream& out, const ExtremumSolver::Extremum& extremum);

class ExtremumOfLinear : public ExtremumSolver {
   public:
    ExtremumOfLinear(std::shared_ptr<OneVariableEquation> equation);

   private:
    void FindExtremum() override;
};

class ExtremumOfQuadratic : public ExtremumSolver {
   public:
    ExtremumOfQuadratic(std::shared_ptr<OneVariableEquation> equation);

   private:
    void FindExtremum() override;
};

}  // namespace equation
