//
// Created by bele on 30.12.17.
//

#include "MqttSnMessageHandler.h"

bool MqttSnMessageHandler::begin() {
    if (socket == nullptr) {
        return false;
    }
    if (logger == nullptr) {
        return false;
    }
    if (!logger->begin()) {
        return false;
    }
    reset_received_buffer();
    socket->setLogger(logger);
    socket->setMqttSnMessageHandler(this);
    return this->socket->begin();
}


void MqttSnMessageHandler::setSocket(SocketInterface *socket) {
    this->socket = socket;
}


void MqttSnMessageHandler::setLogger(LoggerInterface *logger) {
    this->logger = logger;
}

bool MqttSnMessageHandler::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
#if defined(DRIVER_RH_NRF24) || defined(DRIVER_RH_RF95)
    memset(&tmp_address, 0x0, sizeof(device_address));
    memcpy(&tmp_address, destination, sizeof(device_address));
#endif
    return socket->send(destination, bytes, bytes_len);
}

void MqttSnMessageHandler::receiveData(device_address *address, uint8_t *bytes) {
    if (receive_buffer_length == 0 && sizeof(receive_buffer) > bytes[0]) {
        // buffer empty
        // received bytes less or equal to receive_buffer size
        reset_received_buffer();

        memcpy(&receive_address, address, sizeof(device_address));
#if defined(DRIVER_RH_NRF24) || defined(DRIVER_RH_RF95)
        if (receive_address.bytes[0] == tmp_address.bytes[0]) {
            memcpy(&receive_address, &tmp_address, sizeof(device_address));
        }
#endif
        receive_buffer_length = bytes[0];
        memcpy(receive_buffer, bytes, receive_buffer_length);
    }
}

bool MqttSnMessageHandler::loop() {
    return socket->loop();
}

bool MqttSnMessageHandler::print_received_address() {
    Serial.print("ADDRESS");
    for (uint16_t i = 0; i < sizeof(device_address); i++) {
        Serial.print(" ");
        Serial.print(receive_address.bytes[i], DEC);
    }
    Serial.print("\n");
    return true;
}

bool MqttSnMessageHandler::print_received_data() {
    Serial.print("DATA");
    for (uint16_t i = 0; i < receive_buffer_length; i++) {
        Serial.print(" ");
        Serial.print(receive_buffer[i], DEC);
    }
    Serial.print("\n");
    return true;
}

void MqttSnMessageHandler::reset_received_buffer() {
    memset(&receive_address, 0x0, sizeof(device_address));
    memset(receive_buffer, 0x0, sizeof(receive_buffer));
    receive_buffer_length = 0;
}
