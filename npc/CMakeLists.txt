cmake_minimum_required(VERSION 3.31)
project(npc)

set(CMAKE_CXX_STANDARD 17)


include_directories(
        /usr/local/share/verilator/include
        /usr/local/share/verilator/include/vltstd
        build/obj_dir
        /home/admin123/ysyx-workbench/nvboard/usr/include
        ./include
)

add_executable(npc csrc/sim_main.cpp)