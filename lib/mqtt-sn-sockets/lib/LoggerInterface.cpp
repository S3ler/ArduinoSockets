#include "LoggerInterface.h"

bool LoggerInterface::begin() {
    return true;
}

void LoggerInterface::set_log_lvl(uint8_t log_lvl) {
    this->current_log_lvl = log_lvl;
}

void LoggerInterface::log(char *msg, uint8_t log_lvl) {
    log((const char *) msg, log_lvl);
}

void LoggerInterface::log(const char *msg, uint8_t log_lvl) {
    if (log_lvl > current_log_lvl) {
        return;
    }
    Serial.println();
    char millis_buffer[26];
    sprintf(millis_buffer, "%ld", millis());
    Serial.print(millis_buffer);
    Serial.print(": ");
    Serial.print(msg);
}

void LoggerInterface::start_log(char *msg, uint8_t log_lvl) {
    start_log((const char *) msg, log_lvl);
}

void LoggerInterface::start_log(const char *msg, uint8_t log_lvl) {
    last_started_log_lvl = log_lvl;
    if (last_started_log_lvl > current_log_lvl) {
        return;
    }
    Serial.println();
    char millis_buffer[26];
    sprintf(millis_buffer, "%ld", millis());
    Serial.print(millis_buffer);
    Serial.print(": ");
    Serial.print(msg);
}

void LoggerInterface::set_current_log_lvl(uint8_t log_lvl) {
    last_started_log_lvl = log_lvl;
}

void LoggerInterface::append_log(char *msg) {
    append_log((const char *) msg);
}

void LoggerInterface::append_log(const char *msg) {
    if (last_started_log_lvl > current_log_lvl) {
        return;
    }
    Serial.print(msg);
}
