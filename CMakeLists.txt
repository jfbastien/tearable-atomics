cmake_minimum_required(VERSION 2.8.7)
project(tearable CXX)

# Boilerplate #################################################################

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)
if(CMAKE_BUILD_TYPE AND
    NOT uppercase_CMAKE_BUILD_TYPE MATCHES "^(DEBUG|RELEASE)$")
  message(FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

function(ADD_COMPILE_FLAG value)
  message(STATUS "Building with ${value}")
  foreach(variable CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
    set(${variable} "${${variable}} ${value}" PARENT_SCOPE)
  endforeach(variable)
endfunction()

function(ADD_LINK_FLAG value)
  message(STATUS "Linking with ${value}")
  foreach(variable CMAKE_EXE_LINKER_FLAGS)
    set(${variable} "${${variable}} ${value}" PARENT_SCOPE)
  endforeach(variable)
endfunction()

# clang doesn't print colored diagnostics when invoked from Ninja.
if(UNIX AND
   CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
   CMAKE_GENERATOR STREQUAL "Ninja")
  add_compile_flag("-fcolor-diagnostics")
endif()

include(CheckCXXCompilerFlag)

# Configuration ###############################################################

# On x86 generate code with CMPXCHG16B.
check_cxx_compiler_flag("-mcx16" HAS_MCX16)
if(HAS_MCX16)
  add_compile_flag("-mcx16")
else()
  message(WARNING "Compiler doesn't support -mcx16 flag, non-x86 targets may need special flag")
endif()

add_compile_flag("-Wall")
add_compile_flag("-Werror")
add_compile_flag("-Wextra")
add_compile_flag("-Wno-unused-parameter")
add_compile_flag("-fno-omit-frame-pointer")
if(uppercase_CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  add_compile_flag("-O0")
  add_compile_flag("-g3")
else()
  add_compile_flag("-O2")
endif()

find_package(Threads REQUIRED)
add_compile_flag("-std=c++11")

# Submodule ###################################################################

add_subdirectory(benchmark)

# Build / test ################################################################

add_executable(bench_tearing "bench_tearing.cc")
target_link_libraries(bench_tearing benchmark ${CMAKE_THREAD_LIBS_INIT})
add_dependencies(bench_tearing benchmark)
add_test(bench_tearing bench_tearing)
