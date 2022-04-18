#pragma once
#include <WiFi.h>
#include "time.h"
#include <EEPROM.h>
#define EEPROM_SIZE 900
#define COMMAND_SIZE 300

void printPage(WiFiClient*);
