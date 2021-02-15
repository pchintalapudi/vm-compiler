cmake_minimum_required(VERSION 3.17.2)
project(oops-compiler VERSION 0.1.0)

include(CTest)
enable_testing()

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED true)

add_executable(oops-compiler main.cpp)

add_subdirectory(compiler)
add_subdirectory(lexer)
add_subdirectory(logger)
add_subdirectory(parser)
add_subdirectory(files)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O0 -Wextra -Wall -Winit-self -Wold-style-cast -Woverloaded-virtual -Wuninitialized -Winit-self -Wno-unknown-pragmas -fsanitize=undefined -fsanitize-undefined-trap-on-error")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
