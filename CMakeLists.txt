cmake_minimum_required(VERSION 3.12)
project(project)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(project main.cpp searchAlgorithm.h
        puzzle.cpp puzzle.h puzzlePDB.cpp puzzlePDB.h
        minHeap.h util.cpp util.h grid.cpp grid.h salesman.cpp salesman.h)
add_executable(parsing output-parsing.cpp util.cpp util.h)

target_link_libraries(project Threads::Threads)