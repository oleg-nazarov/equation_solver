cmake_minimum_required(VERSION 3.10)

project(EquationSolver CXX)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /JMC")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wno-implicit-fallthrough")
endif()

find_package(Threads REQUIRED)

set(EQUATION_FILES equation.h equation.cpp)
set(EXTREMUM_FILES extremum.h extremum.cpp)
set(IO_FILES input_output.h input_output.cpp)

add_executable(equation_solver main.cpp ${IO_FILES} ${EQUATION_FILES} ${EXTREMUM_FILES})

target_link_libraries(equation_solver Threads::Threads)
