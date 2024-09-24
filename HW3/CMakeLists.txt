cmake_minimum_required(VERSION 3.10)

project(HW3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall")

add_executable(HW3 
    main.cpp 
    complex_number.cpp 
    sort.cpp
)

target_include_directories(HW3 PUBLIC ${CMAKE_SOURCE_DIR})
