cmake_minimum_required(VERSION 3.7)
project(mqtt-sn-sockets)

set(CMAKE_CXX_STANDARD 11)
add_definitions(-DRH_PLATFORM=RH_PLATFORM_RASPI -D__RASPBERRY_PI_)
set(RADIOHEAD_SRC_FILES
        RadioHead/RH_RF95.h
        RadioHead/RH_RF95.cpp
        RadioHead/RHReliableDatagram.h
        RadioHead/RHReliableDatagram.cpp
        RadioHead/RHutil/)

set(EXAMPLE_LIB_SRC_FILES
	lib/SocketInterface.h
        lib/global_defines.h
	lib/mqttsn_messages.h
	lib/LoggerInterface.h
	lib/LoggerInterface.cpp
	lib/LinuxLogger.h
	lib/LinuxLogger.cpp
	lib/MqttSnMessageHandler.h
	lib/MqttSnMessageHandler.cpp)

include_directories(RadioHead/)
include_directories(lib/)
include_directories(lib/arduino-linux-abstraction)


add_library(lib-rh96-socket ${RADIOHEAD_SRC_FILES} RF95Socket.h RF95Socket.cpp)
target_link_libraries(lib-rh96-socket arduino-linux-abstraction wiringPi)

add_library(example-lib-rh96-socket ${EXAMPLE_LIB_SRC_FILES})
add_library(example-lib-rh96-socket lib-rh96-socket)
