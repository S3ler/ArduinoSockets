#include "DummySocket.h"

bool DummySocket::begin() {
    std::cout << "begin() called" << std::endl;
    std::cout << "initializing Broadcast Address with 192.168.0.255.7.134" << std::endl;
    broadcastAddress = device_address(192, 168, 0, 255, 7, 134);
    std::cout << "initializing Own Address with 192.168.0.1.7.134" << std::endl;
    ownAddress = device_address(192, 168, 0, 1, 7, 134);
    std::cout << "returning true" << std::endl;
    return true;
}

void DummySocket::setLogger(LoggerInterface *logger) {
    std::cout << "setLogger(LoggerInterface *logger) called" << std::endl;
}

void DummySocket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {
    std::cout << "setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) called" << std::endl;
}

device_address *DummySocket::getBroadcastAddress() {
    std::cout << "getBroadcastAddress() called" << std::endl;
    std::cout << "returning device_address{ 192, 168, 0, 255, 7, 134 }" << std::endl;
    return &this->broadcastAddress;
}

device_address *DummySocket::getAddress() {
    std::cout << "getAddress() called" << std::endl;
    std::cout << "returning device_address{ 192, 168, 0, 1, 7, 134 }" << std::endl;
    return &this->ownAddress;
}

uint8_t DummySocket::getMaximumMessageLength() {
    std::cout << "getMaximumMessageLength() called" << std::endl;
    std::cout << "returning 255" << std::endl;
    return 255;
}

bool DummySocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    return send(destination, bytes, bytes_len, UINT8_MAX);
}

bool DummySocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {

    std::cout << "send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength)"
              << std::endl;
    std::cout << "destination { ";
    for (uint8_t i = 0; i < sizeof(destination); i++) {
        std::cout << (int) destination->bytes[i];
        if (i != sizeof(destination) - 1) {
            std::cout << ", ";
        } else {
            std::cout << " } ";
        }
    }
    std::cout << " bytes { ";
    for (uint16_t i = 0; i < bytes_len; i++) {
        std::cout << (int) bytes[i];
        if (i == bytes_len - 1) {
            std::cout << " } ";
        } else {
            std::cout << ", ";
        }
    }

    std::cout << " bytes_len=";
    std::cout << (int) bytes_len;


    std::cout << " signal_strength=";
    std::cout << (int) signal_strength;
    std::cout << std::endl;
    std::cout << "returning true" << std::endl;
    return true;
}

bool DummySocket::loop() {
    srand(time(NULL));
    std::cout << "loop() called" << std::endl;
    std::cout << "showing random data" << std::endl;

    uint8_t signal_strength = static_cast<uint8_t>(rand() % 255 + 1);
    uint16_t bytes_len = static_cast<uint16_t>(rand() % 255 + 1);

    uint8_t bytes[bytes_len];
    for (uint8_t i = 0; i < bytes_len; i++) {
        bytes[i] = static_cast<uint8_t>(rand() % 255 + 1);
    }

    device_address sender;
    for (uint8_t i = 0; i < sizeof(sender); i++) {
        sender.bytes[i] = static_cast<uint8_t>(rand() % 255 + 1);
    }

    std::cout << "sender { ";
    for (uint8_t i = 0; i < sizeof(sender); i++) {
        std::cout << (int) sender.bytes[i];
        if (i != sizeof(sender) - 1) {
            std::cout << ", ";
        } else {
            std::cout << " } ";
        }
    }
    std::cout << " bytes { ";
    for (uint16_t i = 0; i < bytes_len; i++) {
        std::cout << (int) bytes[i];
        if (i == bytes_len - 1) {
            std::cout << " } ";
        } else {
            std::cout << ", ";
        }
    }

    std::cout << " bytes_len=";
    std::cout << (int) bytes_len;


    std::cout << " signal_strength=";
    std::cout << (int) signal_strength;
    std::cout << std::endl;

    std::cout << "returning true" << std::endl;
    return true;
}
