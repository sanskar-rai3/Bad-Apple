cmake_minimum_required(VERSION 3.15)
project(Bad-Apple)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(SDL2 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

add_executable(Bad-Apple
    src/main.cpp
)

target_include_directories(Bad-Apple PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
)

target_link_libraries(Bad-Apple
    ${OpenCV_LIBS}
    ${SDL2_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

target_compile_options(Bad-Apple PRIVATE
    ${SDL2_MIXER_CFLAGS_OTHER}
)