cmake_minimum_required(VERSION 3.8)
project(ArduinoSockets)

set(CMAKE_CXX_STANDARD 11)

include_directories(mqtt-sn-gateway/)
include_directories(RadioHead/)
set(INTERFACE_FILE mqtt-sn-gateway/SocketInterface.h)
set(MAIN_FILE TestMain.cpp)
add_executable(runTestMain ${MAIN_FILE})

add_library(DummySocket ${INTERFACE_FILE} DummySocket.cpp DummySocket.h)
add_library(NRF24Socket ${INTERFACE_FILE} NRF24Socket.cpp NRF24Socket.h)
add_library(RF69Socket ${INTERFACE_FILE} RF69Socket.cpp RF69Socket.h)
add_library(EthernetUDPSocket ${INTERFACE_FILE} EthernetUDPSocket.cpp EthernetUDPSocket.h)


target_link_libraries(runTestMain DummySocket)
