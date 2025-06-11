cmake_minimum_required(VERSION 3.31)
project(npc)

set(CMAKE_CXX_STANDARD 17)


include_directories(
        /usr/local/share/verilator/include
        /usr/local/share/verilator/include/vltstd
        ${PROJECT_SOURCE_DIR}/obj_dir
)

add_executable(npc sim_main.cpp)