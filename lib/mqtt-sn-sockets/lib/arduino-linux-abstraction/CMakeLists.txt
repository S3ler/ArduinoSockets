cmake_minimum_required(VERSION 3.6)
project(arduino-linux-abstraction)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)

set(ABSTRACTION_FILES
        src/Arduino.h
        src/Arduino.cpp
        src/SD.h
        src/SD.cpp
        )

add_library(arduino-linux-abstraction ${ABSTRACTION_FILES})
