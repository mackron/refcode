cmake_minimum_required(VERSION 3.14)
project(refcode)

# Options
option(REFCODE_FORCE_CXX "Force compilation as C++" OFF)
option(REFCODE_FORCE_C89 "Force compilation as C89" OFF)

# Construct compiler options.
set(COMPILE_OPTIONS)

if(REFCODE_FORCE_CXX AND REFCODE_FORCE_C89)
    message(FATAL_ERROR "REFCODE_FORCE_CXX and REFCODE_FORCE_C89 cannot be enabled at the same time.")
endif()

if(REFCODE_FORCE_CXX)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(STATUS "Compiling as C++ (GNU/Clang)")
        list(APPEND COMPILE_OPTIONS -x c++)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(STATUS "Compiling as C++ (MSVC)")
        list(APPEND COMPILE_OPTIONS /TP)
    else()
        message(WARNING "REFCODE_FORCE_CXX is enabled but the compiler does not support it. Ignoring.")
    endif()
endif()

if(REFCODE_FORCE_C89)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(STATUS "Compiling as C89")
        list(APPEND COMPILE_OPTIONS -std=c89)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(WARNING "MSVC does not support forcing C89. REFCODE_FORCE_C89 ignored.")
    else()
        message(WARNING "REFCODE_FORCE_C89 is enabled but the compiler does not support it. Ignoring.")
    endif()
endif()

# Warnings
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND COMPILE_OPTIONS -Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    #list(APPEND COMPILE_OPTIONS /W4)
endif()


# spinlock
add_library(spinlock STATIC spinlock.c)
target_compile_options    (spinlock PRIVATE ${COMPILE_OPTIONS})
target_include_directories(spinlock PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR})

# allocation_callbacks
add_library(allocation_callbacks STATIC allocation_callbacks.c)
target_compile_options    (allocation_callbacks PRIVATE ${COMPILE_OPTIONS})
target_include_directories(allocation_callbacks PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR})

# sort
add_library(sort STATIC sort.c)
target_compile_options    (sort PRIVATE ${COMPILE_OPTIONS})
target_include_directories(sort PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR})

# results
add_library(results STATIC results.c)
target_compile_options    (results PRIVATE ${COMPILE_OPTIONS})
target_include_directories(results PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR})