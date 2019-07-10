cmake_minimum_required(VERSION 3.10)
project(DistributedPerceptron)
project(test-matrix)
project(test-message)
project(test-nn)
project(test-run)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

set(CMAKE_CXX_STANDARD 17)
set(MPI_CXX_COMPILER "mpic++")
set(MPI_CXX_COMPILE_OPTIONS "-g -std =c++17")
set(MPI_CXX_LINK_FLAGS "-g -std=c++17")

set (SRC
    src/process.cc
    src/matrix/matrix.cc
    src/nn/nn.cc
    src/timeout.cc
    )
set (FINALY
     src/main.cc
     )
   
set (RUN
     src/run.cc
    )
set (MESSAGE
     tests/test-nn.cc
     )

set (MATRIX
     tests/test-message.cc
     )
set (NN
     tests/test-nn.cc
    )

# Main binary

add_executable(DistributedPerceptron ${FINALY} ${SRC})
add_executable(test-run ${RUN} ${SRC})
add_executable(test-message ${MESSAGE} ${SRC})
add_executable(test-matrix ${MATRIX} ${SRC})
add_executable(test-nn ${NN} ${SRC})

target_link_libraries(DistributedPerceptron ${MPI_LIBRARIES})
target_link_libraries(test-run ${MPI_LIBRARIES})
target_link_libraries(test-message ${MPI_LIBRARIES})
target_link_libraries(test-matrix ${MPI_LIBRARIES})
target_link_libraries(test-nn ${MPI_LIBRARIES})
