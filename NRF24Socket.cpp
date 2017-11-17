#include "NRF24Socket.h"

bool NRF24Socket::begin() {
    if (this->logger == nullptr) {
        return false;
    }
    if (this->mqttSnMessageHandler == nullptr) {
        return false;
    }

    this->broadcastAddress = device_address(BROADCAST_ADDRESS, 0, 0, 0, 0, 0);
    this->ownAddress = device_address(OWN_ADDRESS, 0, 0, 0, 0, 0);

    // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
    return manager->init();
}

void NRF24Socket::setLogger(LoggerInterface *logger) {
    this->logger = logger;
}

void NRF24Socket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {
    this->mqttSnMessageHandler = mqttSnMessageHandler;
}

device_address *NRF24Socket::getBroadcastAddress() {
    return &this->broadcastAddress;
}

device_address *NRF24Socket::getAddress() {
    return &this->ownAddress;
}

uint8_t NRF24Socket::getMaximumMessageLength() {
    return nrf24->maxMessageLength();
}

bool NRF24Socket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    return send(destination, bytes, bytes_len, UINT8_MAX);
}

bool NRF24Socket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
    return manager->sendto(bytes, bytes_len, destination->bytes[0]);
}

bool NRF24Socket::loop() {
    uint8_t buf_len = 0;
    uint8_t buf[UINT8_MAX];
    memset(&buf, 0x0, UINT8_MAX);
    device_address from;
    memset(&from.bytes[0], 0x0, UINT8_MAX);
    if (manager->available()) {
        if (manager->recvfrom(buf, &buf_len, &from.bytes[0])) {
#ifdef ARDUINO
            mqttSnMessageHandler->receiveData(&from, &buf);
#else
            #warning "Compilation for Non Arduino Environment."
#endif
        }
    }
    return true;
}
