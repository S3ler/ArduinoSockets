// nrf95_ping_pong.ino
// -*- mode: C++ -*-

#ifdef ESP8266
#include <Arduino.h>
#include <ESP8266WiFi.h>
#endif

#include <SPI.h>
#include <LinuxLogger.h>
#include <LoggerInterface.h>
#include <MqttSnMessageHandler.h>
#include <Ethernet.h>


#include <RF95Socket.h>
#include <SimpleMqttSnClientTester.h>

#define PING
#define SIMPLEMQTTSNCLIENTTESTER

RF95Socket socket;
LoggerInterface logger;

#ifdef SIMPLEMQTTSNCLIENTTESTER
SimpleMqttSnClientTester mqttSnMessageHandler;
#else
MqttSnMessageHandler mqttSnMessageHandler;
#endif

#ifdef ESP8266
RH_RF95 rf95(2, 15);
#else
RH_RF95 rf95;
#endif
RHReliableDatagram manager(rf95);

#ifdef PING
#define OWN_ADDRESS 0x02
#define PONG_ADDRESS 0x03
device_address target_address(PONG_ADDRESS, 0, 0, 0, 0, 0);
uint8_t msg[] = {5, 'P', 'i', 'n', 'g'};
#elif PONG
#define OWN_ADDRESS 0x03
#endif


void setup() {
    Serial.begin(9600);
    Serial.println("Starting");

    manager.setThisAddress(OWN_ADDRESS);
    socket.setRf95(&rf95);
    socket.setManager(&manager);
    socket.setLogger(&logger);
    socket.setMqttSnMessageHandler(&mqttSnMessageHandler);
    mqttSnMessageHandler.setLogger(&logger);
    mqttSnMessageHandler.setSocket(&socket);

    if (!mqttSnMessageHandler.begin()) {
        Serial.println("Failure init MqttSnMessageHandler");
    } else {
        Serial.println("Started");
#ifdef PING
#ifndef SIMPLEMQTTSNCLIENTTESTER
        mqttSnMessageHandler.send(&target_address, msg, (uint16_t) msg[0]);
#else
        mqttSnMessageHandler.send_pingreq(&target_address);
#endif
#endif
    }
}

void loop() {
    mqttSnMessageHandler.loop();
}