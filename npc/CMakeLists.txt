cmake_minimum_required(VERSION 3.31)
project(npc)

set(CMAKE_CXX_STANDARD 17)


include_directories(
        /usr/local/share/verilator/include
        /usr/local/share/verilator/include/vltstd
        build/obj_dir
        /home/admin123/ysyx-workbench/nvboard/usr/include
        /home/admin123/ysyx-branches/npc/npc/include
)

file(GLOB_RECURSE CSRCS_MODULE "csrc/*.c")
file(GLOB CSRCS_MAIN "csrc/*.cpp")

add_executable(npc ${CSRCS_MODULE} ${CSRCS_MAIN})