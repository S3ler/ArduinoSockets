#include "RF69Socket.h"

bool RF69Socket::begin() {
    if (this->logger == nullptr) {
        return false;
    }
    if (this->mqttSnMessageHandler == nullptr) {
        return false;
    }
    if (this->rf69 == nullptr) {
        return false;
    }
    if (this->manager == nullptr) {
        return false;
    }

    this->broadcastAddress = device_address(RH_BROADCAST_ADDRESS, 0, 0, 0, 0, 0);
    this->ownAddress = device_address(OWN_ADDRESS, 0, 0, 0, 0, 0);

    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
    if (!rf69->init()) {
        return false;
    }
#ifdef RFM69HW
    this->rf69->setTxPower(14, true);
#endif
    return manager->init();
}

void RF69Socket::setRf69(RH_RF69 *rf69) {
    RF69Socket::rf69 = rf69;
}

void RF69Socket::setManager(RHReliableDatagram *manager) {
    RF69Socket::manager = manager;
}


void RF69Socket::setLogger(LoggerInterface *logger) {
    RF69Socket::logger = logger;
}

void RF69Socket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {
    RF69Socket::mqttSnMessageHandler = mqttSnMessageHandler;
}

device_address *RF69Socket::getBroadcastAddress() {
    return &this->broadcastAddress;
}

device_address *RF69Socket::getAddress() {
    return &this->ownAddress;
}

uint8_t RF69Socket::getMaximumMessageLength() {
    return RH_RF69_MAX_MESSAGE_LEN;
}

bool RF69Socket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    return send(destination, bytes, bytes_len, UINT8_MAX);
}

bool RF69Socket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
    // we send the bytes and hope that they are finished transmitting until we reach loop() function the next time
    return manager->sendto(bytes, bytes_len, destination->bytes[0]);
}

bool RF69Socket::loop() {
    // first wait for last packet send - or timeout
    manager->waitPacketSent(WAIT_PACKET_SEND_TIMEOUT);

    uint8_t buf_len = 0;
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    memset(&buf, 0x0, RH_RF69_MAX_MESSAGE_LEN);
    device_address from;
    memset(&from.bytes[0], 0x0, sizeof(device_address));
    if (manager->available()) {
        if (manager->recvfrom(buf, &buf_len, &from.bytes[0])) {
#ifdef GATEWAY_MQTTSNMESSAGEHANDLER_H
            mqttSnMessageHandler->receiveData(&from, &buf);
#else
#warning "Compilation without Core MQTT-SN Gateway."
#endif
        }
    }
    return true;
}

