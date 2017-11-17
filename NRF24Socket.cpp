//
// Created by bele on 17.11.17.
//

#include "NRF24Socket.h"

bool NRF24Socket::begin() {
    return false;
}

void NRF24Socket::setLogger(LoggerInterface *logger) {

}

void NRF24Socket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {

}

device_address *NRF24Socket::getBroadcastAddress() {
    return nullptr;
}

device_address *NRF24Socket::getAddress() {
    return nullptr;
}

uint8_t NRF24Socket::getMaximumMessageLength() {
    return nullptr;
}

bool NRF24Socket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    return false;
}

bool NRF24Socket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
    return false;
}

bool NRF24Socket::loop() {
    return false;
}
