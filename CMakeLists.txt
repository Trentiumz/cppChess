cmake_minimum_required(VERSION 3.19)
project(chessEngine)

set(CMAKE_CXX_STANDARD 17)

add_executable(chessEngine main.cpp ratings.cpp program.h boardEdit.cpp validMoveGenerator.cpp preprocessing.cpp evaluator.cpp)