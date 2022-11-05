cmake_minimum_required(VERSION 3.14)
project(malloc_bench)

add_subdirectory(third-party/gflags)

add_executable(malloc_bench malloc_bench.cc)
target_link_libraries(malloc_bench gflags::gflags pthread)