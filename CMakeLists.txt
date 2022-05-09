cmake_minimum_required(VERSION 3.2)

project(wasmamba)

# find_package(LibSolv)

find_path(SOLV_INCLUDE_DIR solv/solver.h)
find_library(SOLV_LIBRARY solv)
find_library(SOLV_EXT_LIBRARY solvext)

add_executable(main src/main.cpp)

target_include_directories(main PUBLIC ${SOLV_INCLUDE_DIR})

target_link_libraries(main ${SOLV_LIBRARY} ${SOLV_EXT_LIBRARY})

