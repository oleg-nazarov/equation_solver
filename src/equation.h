#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace equation {

class OneVariableEquation {
   public:
    using Roots = std::variant<std::nullptr_t, std::string, std::vector<int>, std::vector<double>>;

    virtual ~OneVariableEquation();

    Roots GetRoots();
    std::vector<int> GetCoefficients() const;

   protected:
    template <typename... Args>
    OneVariableEquation(Args... args);

    const std::vector<int> coefficients_;
    std::optional<Roots> roots_;

   private:
    virtual void Solve() = 0;
};

std::ostream& operator<<(std::ostream& out, const OneVariableEquation::Roots& roots);

class Linear : public OneVariableEquation {
   public:
    Linear(int a, int b);

   private:
    void Solve() override;
};

class Quadratic : public OneVariableEquation {
   public:
    Quadratic(int a, int b, int c);

   private:
    void Solve() override;
};

template <typename... Args>
OneVariableEquation::OneVariableEquation(Args... args) : coefficients_{args...} {}

}  // namespace equation
