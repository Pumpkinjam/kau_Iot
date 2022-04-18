#pragma once
#include <WiFi.h>
#include "time.h"
#include <EEPROM.h>
#define EEPROM_SIZE 150
#define COMMAND_SIZE 300

struct tm localTime;

struct tm getTime() {
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        return timeinfo;
    }

    return timeinfo;
}

void printPage(WiFiClient*);