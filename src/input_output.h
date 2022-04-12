#pragma once

#include <iostream>

#include "calculations.h"
#include "equation.h"
#include "extremum.h"

namespace equation {

void ReadTreyAndProduceCoefficients(int argc, char** argv, Queue& coefficients,
                                    ThreadHelper& thread_helper);

void PrintRootsAndExtremum(std::ostream& out, std::vector<std::future<PrintResult>>& print_results,
                           ThreadHelper& thread_helper);

}  // namespace equation
