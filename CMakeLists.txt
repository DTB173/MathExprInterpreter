cmake_minimum_required(VERSION 3.15)
project(cmdCalc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(mathcore
    src/Lexer.cpp
    src/Evaluator.cpp
    src/Parser.cpp
)

target_include_directories(mathcore PUBLIC include)

add_executable(cmdCalc main.cpp)
target_link_libraries(cmdCalc PRIVATE mathcore)

# --- Tests ---
enable_testing()
include(FetchContent)

FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(catch2)

add_executable(tests
    tests/test_evaluator.cpp
    tests/test_parser.cpp
    tests/test_lexer.cpp
)
target_link_libraries(tests PRIVATE mathcore Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE include)

add_test(NAME AllTests COMMAND tests)
