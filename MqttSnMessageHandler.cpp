#include <stdio.h>
#include "MqttSnMessageHandler.h"
#include <Arduino.h>

bool MqttSnMessageHandler::begin() {
    if(socket == nullptr){
        return false;
    }
    if(logger == nullptr){
        return false;
    }
    if(!logger->begin()){
        return false;
    }
    return this->socket->begin();
}

void MqttSnMessageHandler::setSocket(SocketInterface *socket) {
    this->socket = socket;
}


void MqttSnMessageHandler::setLogger(LoggerInterface *logger) {
    this->logger = logger;
}


bool MqttSnMessageHandler::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len){
    return socket->send(destination, bytes, bytes_len);
}

void MqttSnMessageHandler::receiveData(device_address *address, uint8_t *bytes) {
    message_header *header = (message_header *) bytes;
    if (header->length < 2) {
        Serial.print("header->length < 2 | ");
        printReceived(address, bytes);
        socket->send(address, bytes, bytes[0]);
        return;
    }
    switch (header->type) {
        case MQTTSN_PINGREQ:
            Serial.print("MQTTSN_PINGREQ");
            break;
        case MQTTSN_SEARCHGW:
            Serial.print("MQTTSN_SEARCHGW");
            break;
        case MQTTSN_CONNECT:
            Serial.print("MQTTSN_CONNECT");
            break;
        case MQTTSN_WILLTOPIC:
            Serial.print("MQTTSN_WILLTOPIC");
            break;
        case MQTTSN_WILLMSG:
            Serial.print("MQTTSN_WILLMSG");
            break;
        case MQTTSN_REGISTER:
            Serial.print("MQTTSN_REGISTER");
            break;
        case MQTTSN_REGACK:
            Serial.print("MQTTSN_REGACK");
            break;
        case MQTTSN_PUBLISH:
            Serial.print("MQTTSN_PUBLISH");
            break;
        case MQTTSN_PUBACK:
            Serial.print("MQTTSN_PUBACK");
            break;
        case MQTTSN_SUBSCRIBE:
            Serial.print("MQTTSN_SUBSCRIBE");
            break;
        case MQTTSN_UNSUBSCRIBE:
            Serial.print("MQTTSN_UNSUBSCRIBE");
            break;
        case MQTTSN_DISCONNECT:
            Serial.print("MQTTSN_DISCONNECT");
        case MQTTSN_WILLTOPICUPD:
            Serial.print("MQTTSN_WILLTOPICUPD");
            break;
        case MQTTSN_WILLMSGUPD:
            Serial.print("MQTTSN_WILLMSGUPD");
            break;
        case MQTTSN_FORWARD_ENCAPLSULATION:
            Serial.print("MQTTSN_FORWARD_ENCAPLSULATION");
            break;
        case MQTTSN_ADVERTISE:
            Serial.print("MQTTSN_ADVERTISE");
            break;
        default:
            Serial.print("UNKNOWN");
            break;
    }
    Serial.print(" | ");
    printReceived(address, bytes);
    socket->send(address, bytes, bytes[0]);
}

void MqttSnMessageHandler::printReceived(device_address *address, uint8_t *bytes) {
    printDeviceAddress(address);
    Serial.print(" | ");
    Serial.print(" bytes_len ");
    Serial.print(bytes[0]);
    Serial.print(" | ");
    printBuffer(bytes);
    Serial.println("");
}


void MqttSnMessageHandler::printDeviceAddress(device_address *address) {
    Serial.print("from { ");
    for (uint8_t i = 0; i < sizeof(device_address); i++) {
        Serial.print(address->bytes[i]);
        if (i == sizeof(device_address) - 1) {
            Serial.print(" }");
        } else {
            Serial.print(", ");
        }
    }
}

void MqttSnMessageHandler::printBuffer(uint8_t *bytes) {
    Serial.print("bytes { ");
    for (uint8_t i = 0; i < bytes[0]; i++) {
        Serial.print(bytes[i]);
        if (i == bytes[0] - 1) {
            Serial.print(" }");
        } else {
            Serial.print(", ");
        }
    }
}

bool MqttSnMessageHandler::loop() {
    return socket->loop();
}
