
#include "mqtt-sn-gateway/SocketInterface.h"
#include "DummySocket.h"

SocketInterface *socketInterface = new DummySocket();


device_address destination(192, 168, 0, 2, 7, 134);
uint8_t bytes[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
uint16_t bytes_len = 10;

int main(int argc, char *argv[]) {
    socketInterface->begin();
    socketInterface->send(&destination, reinterpret_cast<uint8_t *>(&bytes), bytes_len);
    socketInterface->loop();
}