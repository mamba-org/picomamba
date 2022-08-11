cmake_minimum_required(VERSION 3.2)

project(mamba_wasm)

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

set(MAMBA_WASM_SRCS
	src/main.cpp
	src/mpool.cpp
	src/mrepo.cpp
	src/prefix_data.cpp
	src/package_info.cpp
)

add_executable(main ${MAMBA_WASM_SRCS})
target_include_directories(main PUBLIC ${SOLV_INCLUDE_DIR})
target_link_libraries(main ${SOLV_LIBRARY} ${SOLV_EXT_LIBRARY})

if (EMSCRIPTEN)
	add_executable(mamba_wasm src/em.cpp ${MAMBA_WASM_SRCS})

	set_target_properties(mamba_wasm
	  PROPERTIES LINK_FLAGS
		--bind
		-sERROR_ON_UNDEFINED_SYMBOLS=0
		--js-library=${CMAKE_SOURCE_DIR}/js/library.js
	)

	target_link_libraries(mamba_wasm ${SOLV_LIBRARY} ${SOLV_EXT_LIBRARY})
endif()
