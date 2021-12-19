# License: Apache 2.0. See LICENSE file in root directory.
# Copyright(c) 2019 Intel Corporation. All Rights Reserved.
#  minimum required cmake version: 3.1.0
cmake_minimum_required(VERSION 3.1.0)

add_executable(my_capture my_capture.cpp ../example.hpp)
set_property(TARGET my_capture PROPERTY CXX_STANDARD 11)
target_link_libraries(my_capture ${DEPENDENCIES})
include_directories(../ ../../third-party/tclap/include ../../examples)
set_target_properties (my_capture PROPERTIES FOLDER "Examples")

install(TARGETS my_capture RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
