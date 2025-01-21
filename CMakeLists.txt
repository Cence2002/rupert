cmake_minimum_required(VERSION 3.16)
project(rupert)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

#set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#find_package(Eigen3 REQUIRED)
#find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(rupert main.cpp)

#target_include_directories(rupert PRIVATE ${EIGEN3_INCLUDE_DIR})
#target_link_libraries(rupert PRIVATE ${OpenCV_LIBS})
target_include_directories(rupert PRIVATE ${Boost_INCLUDE_DIRS})
