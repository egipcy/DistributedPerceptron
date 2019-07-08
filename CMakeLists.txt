cmake_minimum_required(VERSION 3.10)

project(DistributedPerceptron)


find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

set(CMAKE_CXX_STANDARD 17)
set(MPI_CXX_COMPILER "mpic++")
set(MPI_CXX_COMPILE_OPTIONS "-g -std =c++17")
set(MPI_CXX_LINK_FLAGS "-g -std=c++17")

set (SRC
    src/main.cc
    src/process.cc
    src/matrix/matrix.cc
    src/nn/nn.cc
)
# Main binary

add_executable(DistributedPerceptron ${SRC})

target_link_libraries(DistributedPerceptron ${MPI_LIBRARIES})
