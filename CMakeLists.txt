cmake_minimum_required(VERSION 3.2)

project(wasmamba)

set (CMAKE_CXX_STANDARD 17)
# find_package(LibSolv)

find_path(SOLV_INCLUDE_DIR solv/solver.h
	PATHS $ENV{CONDA_PREFIX}
	NO_CMAKE_FIND_ROOT_PATH
)
find_library(SOLV_LIBRARY solv
	PATHS $ENV{CONDA_PREFIX}
	NO_CMAKE_FIND_ROOT_PATH
)
find_library(SOLV_EXT_LIBRARY solvext
	PATHS $ENV{CONDA_PREFIX}
	NO_CMAKE_FIND_ROOT_PATH
)

add_executable(main src/main.cpp src/mpool.cpp)

target_include_directories(main PUBLIC ${SOLV_INCLUDE_DIR})

target_link_libraries(main ${SOLV_LIBRARY} ${SOLV_EXT_LIBRARY})

if (EMSCRIPTEN)
	add_executable(wamba src/em.cpp src/mpool.cpp)

	set_target_properties(wamba PROPERTIES LINK_FLAGS "--bind -s ERROR_ON_UNDEFINED_SYMBOLS=0 --js-library  ${CMAKE_SOURCE_DIR}/js/library.js -O3")

	target_link_libraries(wamba ${SOLV_LIBRARY} ${SOLV_EXT_LIBRARY})
endif()