# Equation Solver
A program that solves a quadratic equation and finds an extremum of the parabola.

**Example:**
```
$ equation_solver 1 -2 -3 -3 4 5 0 4 -4

Output:
(1 -2 -3) => (-1, 3) Xmin=1
(-3 4 5) => (-0.7863, 2.11963) Xmax=0.666667
(0 4 -4) => (1) no relative extreme
```

## Details

- parameters trey is a set of quadratic coefficients, i.e. *a, b, c*
- coefficients are integers (without a fractional component)
- protection against input garbage, i.e. `4 aaa 11`
- multithreading for calculation of each equation concurrently

## How to use

**Build:**
```
mkdir build && cd build

cmake ../src
cmake --build .
```

**Run:**
- try this:
```
./equation_solver 1 -2 -3 -3 4 5 0 4 -4 3 4 5 0 0 0 4 aaa 11
```
- or run with your equations

## Next plans

- solve cubic equations
- [avoid loss of significance](https://en.wikipedia.org/wiki/Quadratic_equation#Avoiding_loss_of_significance)
- draw graphs
