cmake_minimum_required(VERSION 3.8)
project(RadioHeadSocket)

set(CMAKE_CXX_STANDARD 11)

set(INTERFACE_FILE mqtt-sn-gateway/SocketInterface.h)

set(SOURCE_FILES DummySocket.cpp DummySocket.h)
add_library(DummySocket ${SOURCE_FILES} ${INTERFACE_FILE})

set(MAIN_FILE TestMain.cpp)
add_executable(runTestMain ${MAIN_FILE})
target_link_libraries(runTestMain DummySocket)
