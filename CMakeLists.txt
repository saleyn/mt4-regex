﻿# CMakeList.txt : CMake project for mt4-regex, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set( MSVC_INCREMENTAL_YES_FLAG "/INCREMENTAL:NO")

project(mqt-regex VERSION 1.0)

if (WIN32)

  #set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#    if (CMAKE_BUILD_TYPE MATCHES "(Debug|debug|DEBUG)")
#      set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
#    else()
#      set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
#    endif()

  add_compile_definitions(
  	_CRT_SECURE_NO_WARNINGS
    UNICODE _UNICODE _CONSOLE
    BUILDING_LIB
  )

  set(CMAKE_MFC_FLAG 0)
endif()

add_compile_options(/wd26812)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO" )

# Add source to this project's executable.
#-------------------------------------------------------------------------------
# Target projects:
#-------------------------------------------------------------------------------
add_library(mqt-regex SHARED mt4-regex.cpp)

target_compile_definitions(mqt-regex PUBLIC _USRDLL _WINDLL)

if (CMAKE_CL_64 OR CMAKE_GENERATOR_PLATFORM EQUAL "x64")
  MESSAGE("==> 64 bit build")
  set_target_properties(mqt-regex PROPERTIES SUFFIX ".x64.dll")
else()
  MESSAGE("==> 32 bit build")
  set_target_properties(mqt-regex PROPERTIES SUFFIX ".x86.dll")
endif()


add_executable(mqt-regex-test mt4-regex-test.cpp)
target_link_libraries(mqt-regex-test mt4-regex)

enable_testing()

add_test(mqt-regex mqt-regex-test)

install(TARGETS mqt-regex mqt-regex-test RUNTIME DESTINATION bin)
