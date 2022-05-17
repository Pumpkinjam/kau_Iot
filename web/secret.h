#pragma once
#include <WiFi.h>
#include "time.h"
#include <EEPROM.h>
#define EEPROM_SIZE 200
#define COMMAND_SIZE 300

void printLoginPage(WiFiClient*);
void printManagePage(WiFiClient*);
void printLcdPage(WiFiClient*);
