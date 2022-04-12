#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

#include "calculations.h"
#include "equation.h"
#include "input_output.h"

void PrintUsage(std::ostream& out = std::cerr) {
    using namespace std::literals::string_literals;

    out << "Usage: equation_solver [integers], e.g.:\n"s;
    out << "$ equation_solver 1 -2 -3 -3 4 5 0 4 -4 3 4 5 0 0 0 4 aaa 11\n"s;
}

// implementing Producer-Consumer Pattern, we're going to read equation coefficients in one thread,
// calculate roots and extremum in multiple other threads, and print results in the last thread
// without awaitining for the end of calculations
int main(int argc, char** argv) {
    using namespace equation;

    if (argc == 1) {
        PrintUsage();
        return 1;
    }

    // we can print roots and extremum after we have read all the arguments
    // but before have finished all calculations

    // mutex restrains "push" and "pop" operations for Queue
    Queue coefficients;
    ThreadHelper thread_helper;

    std::thread t_read(ReadTreyAndProduceCoefficients,
                       argc, argv, std::ref(coefficients), std::ref(thread_helper));

    std::vector<std::future<PrintResult>> print_results;
    std::thread t_calculate(FindRootsAndExtremum,
                            std::ref(print_results), std::ref(coefficients), std::ref(thread_helper));

    std::thread t_print(PrintRootsAndExtremum,
                        std::ref(std::cout), std::ref(print_results), std::ref(thread_helper));

    t_read.join();
    t_calculate.join();
    t_print.join();

    return 0;
}
