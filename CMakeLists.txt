cmake_minimum_required(VERSION 3.15)
project(Bad-Apple)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(include)

add_executable(Bad-Apple
    src/main.cpp
    src/tool.cpp
)

target_link_libraries(Bad-Apple ${OpenCV_LIBS})

