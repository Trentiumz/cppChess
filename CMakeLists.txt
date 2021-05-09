cmake_minimum_required(VERSION 3.19)
project(chessEngine)

set(CMAKE_CXX_STANDARD 17)

add_executable(chessEngine engine_src/main.cpp engine_src/ratings.cpp engine_src/program.h engine_src/boardEdit.cpp engine_src/validMoveGenerator.cpp engine_src/preprocessing.cpp engine_src/evaluator.cpp)