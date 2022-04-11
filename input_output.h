#pragma once

#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <tuple>

#include "equation.h"
#include "extremum.h"

namespace equation {

using Queue = std::queue<std::tuple<std::string, std::string, std::string>>;

void ReadAndPushCoefficients(int argc, char** argv, Queue& q, bool& input_is_empty,
                            std::condition_variable& cv, std::mutex& m);

void CalculateRootsExtremumAndPrint(std::ostream& out, Queue& q, const bool& input_is_empty,
                                    std::condition_variable& cv, std::mutex& m);

}  // namespace equation
