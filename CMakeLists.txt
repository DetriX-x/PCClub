cmake_minimum_required(VERSION 3.16)

project(ClubSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(club_simulator
    main.cc
    Event.cc
    EventReader.cc
)

target_include_directories(club_simulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(club_simulator PRIVATE
    -Wall -Wextra -pedantic
)
