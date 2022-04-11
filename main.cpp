#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>

#include "equation.h"
#include "input_output.h"

int main(int argc, char** argv) {
    using namespace equation;

    std::mutex m;
    std::condition_variable cv;
    bool input_is_empty = false;
    equation::Queue q;

    std::thread t1(ReadAndPushCoefficients, argc, argv, std::ref(q), std::ref(input_is_empty),
                   std::ref(cv), std::ref(m));
    std::thread t2(CalculateRootsExtremumAndPrint, std::ref(std::cout), std::ref(q),
                   std::cref(input_is_empty), std::ref(cv), std::ref(m));

    t1.join();
    t2.join();

    return 0;
}
