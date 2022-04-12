cmake_minimum_required(VERSION 3.10)

project(EquationSolver CXX)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /JMC")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wno-implicit-fallthrough")
endif()

find_package(Threads REQUIRED)

set(IO_FILES input_output.h input_output.cpp)
set(EQUATION_FILES calculations.h calculations.cpp equation.h equation.cpp extremum.h extremum.cpp)

add_executable(equation_solver main.cpp ${IO_FILES} ${EQUATION_FILES})

target_link_libraries(equation_solver Threads::Threads)
