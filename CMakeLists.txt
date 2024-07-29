cmake_minimum_required(VERSION 2.8)

project(RTKLib)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "-std=c++11")

# # find opencv to read yaml
# find_package(Eigen3 REQUIRED)
# find_package( OpenCV 3.0.0 REQUIRED )

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SRC_FILES "src/*.cpp" "src/*.c" "src/rcv/*.c")
add_library(RTKLib ${SRC_FILES})
# target_link_libraries(RTKLib blas lapack z)

add_executable(RTKLibGNSS src/RTKLibGNSS.cpp)
target_link_libraries(RTKLibGNSS RTKLib pthread)