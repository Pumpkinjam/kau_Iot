#pragma once
#include <WiFi.h>
#include "time.h"
#include <EEPROM.h>
#define EEPROM_SIZE 200
#define COMMAND_SIZE 300

void printPage(WiFiClient*);
