
cmake_minimum_required(VERSION 3.1)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
project (picomamba)


set(PROJECT_INCLUDE_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CPP_ROOT_FOLDER_NAME   picomamba)
include_directories(${PROJECT_INCLUDE_DIR})

# C++ 17
# ===========

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ version selection")  # or 11, 14, 17, 20
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # optional, ensure standard is supported
set(CMAKE_CXX_EXTENSIONS OFF)  # optional, keep compiler extensions off

include(CheckCXXCompilerFlag)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    CHECK_CXX_COMPILER_FLAG("-std=c++17" HAS_CPP17_FLAG)
    if (HAS_CPP17_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    else()
        message(FATAL_ERROR "Unsupported compiler -- C++17 support required!")
    endif()
endif()   


# Emscripten
if(EMSCRIPTEN)
    set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
    set(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "-s SIDE_MODULE=1")
    set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "-s SIDE_MODULE=1")
    set(CMAKE_STRIP FALSE)  # used by default in pybind11 on .so modules
endif()

# Versionning
# ===========
set(${PROJECT_NAME}_VERSION 0.2.0)
message(STATUS "Building project v${${PROJECT_NAME}_VERSION}")

# Dependencies
# ============
find_package(nlohmann_json REQUIRED)
find_package(LibSolv REQUIRED)
# ind_package(xtl      REQUIRED)

# # Build
# # =====


SET(INTERFACE_LIB_NAME picomamba)

file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS  ${PROJECT_INCLUDE_DIR}/*.hpp)


add_library(${INTERFACE_LIB_NAME} INTERFACE)


target_compile_features(${INTERFACE_LIB_NAME}  INTERFACE cxx_std_17)


message(STATUS "INSTALL_INTERFACE: ${CMAKE_INSTALL_INCLUDEDIR}")
target_include_directories(${INTERFACE_LIB_NAME}  INTERFACE 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)



target_link_libraries(${INTERFACE_LIB_NAME} 
  INTERFACE ${LibSolv_LIBRARIES} nlohmann_json::nlohmann_json)


# Python
# ============
OPTION(BUILD_PYTHON "${PROJECT_NAME} python binding" ON)
if(BUILD_PYTHON)
    add_subdirectory(python)
endif()



# Installation
# ============

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)


set(${PROJECT_NAME}_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE
    STRING "install path for ${PROJECT_NAME}Config.cmake")


message(STATUS "DEST ${${PROJECT_NAME}_CMAKECONFIG_INSTALL_DIR}" )


install(TARGETS ${INTERFACE_LIB_NAME}
        EXPORT ${INTERFACE_LIB_NAME}-targets)

install(EXPORT ${INTERFACE_LIB_NAME}-targets
        FILE ${INTERFACE_LIB_NAME}Targets.cmake
        #NAMESPACE ${PROJECT_NAME}:: 
        DESTINATION lib/cmake/${PROJECT_NAME}
         )


install(DIRECTORY  ${PROJECT_INCLUDE_DIR}/${CPP_ROOT_FOLDER_NAME}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})




configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${${PROJECT_NAME}_CMAKECONFIG_INSTALL_DIR})


# ${PROJECT_NAME} is header-only and does not depend on the architecture.
# Remove CMAKE_SIZEOF_VOID_P from ${PROJECT_NAME}ConfigVersion.cmake so that an ${PROJECT_NAME}Config.cmake
# generated for a 64 bit target can be used for 32 bit targets and vice versa.
set(_${PROJECT_NAME}_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
unset(CMAKE_SIZEOF_VOID_P)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${${PROJECT_NAME}_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
set(CMAKE_SIZEOF_VOID_P ${_${PROJECT_NAME}_CMAKE_SIZEOF_VOID_P})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${${PROJECT_NAME}_CMAKECONFIG_INSTALL_DIR})

install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION ${${PROJECT_NAME}_CMAKECONFIG_INSTALL_DIR})

configure_file(${PROJECT_NAME}.pc.in
               "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
                @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig/")



