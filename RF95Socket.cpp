#include "RF95Socket.h"

bool RF95Socket::begin() {
    if (this->logger == nullptr) {
        return false;
    }
    if (this->mqttSnMessageHandler == nullptr) {
        return false;
    }
    if (this->rf95 == nullptr) {
        return false;
    }
    if (this->manager == nullptr) {
        return false;
    }

    this->broadcastAddress = device_address(RH_BROADCAST_ADDRESS, 0, 0, 0, 0, 0);
    this->ownAddress = device_address(OWN_ADDRESS, 0, 0, 0, 0, 0);

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    if (!rf95->init()) {
        return false;
    }

#ifdef FREQUENCY
    rf95->setFrequency(FREQUENCY);
#endif

#ifdef TX_POWER_PIN
    rf95->setTxPower(18, false);
#endif

#ifdef MODEM_CONFIG_CHOICE
    rf95->setModemConfig(RH_RF95::MODEM_CONFIG_CHOICE);
#endif

    return manager->init();
}

void RF95Socket::setRf95(RH_RF95 *rf95) {
    RF95Socket::rf95 = rf95;
}

void RF95Socket::setManager(RHReliableDatagram *manager) {
    RF95Socket::manager = manager;
}


void RF95Socket::setLogger(LoggerInterface *logger) {
    RF95Socket::logger = logger;
}

void RF95Socket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {
    RF95Socket::mqttSnMessageHandler = mqttSnMessageHandler;
}

device_address *RF95Socket::getBroadcastAddress() {
    return &this->broadcastAddress;
}

device_address *RF95Socket::getAddress() {
    return &this->ownAddress;
}

uint8_t RF95Socket::getMaximumMessageLength() {
    return RH_RF95_MAX_MESSAGE_LEN;
}

bool RF95Socket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    return send(destination, bytes, bytes_len, UINT8_MAX);
}

bool RF95Socket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
    manager->sendto(bytes, bytes_len, destination->bytes[0]);
    manager->waitPacketSent();
    manager->available(); // put it back to receive mode
    return true;
}

bool RF95Socket::loop() {
    uint8_t buf_len = RH_RF95_MAX_MESSAGE_LEN;
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    memset(&buf, 0x0, RH_RF95_MAX_MESSAGE_LEN);
    device_address from;
    memset(&from.bytes[0], 0x0, sizeof(device_address));
    if (manager->available()) {
        if (manager->recvfrom(buf, &buf_len, &from.bytes[0])) {
            mqttSnMessageHandler->receiveData(&from, (uint8_t *) &buf);
        }
    }
    return true;
}

