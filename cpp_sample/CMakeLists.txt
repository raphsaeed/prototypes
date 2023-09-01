cmake_minimum_required(VERSION 3.10)
project(main)

# Set C++17 as the standard for the project
set(CMAKE_CXX_STANDARD 17)
# Make sure the compiler uses C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(OpenCV REQUIRED)
add_executable(main main.cpp)
#target_link_libraries(main ${OpenCV_LIBS})