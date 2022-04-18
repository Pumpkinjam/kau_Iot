#pragma once
#include <WiFi.h>
#include "time.h"
#include <EEPROM.h>
#define EEPROM_SIZE 150
#define COMMAND_SIZE 300

const int ledChannel = 0;
const int resolution = 8;
const int buzPin = 22;

char sInput;

// Serial Moniter input string
char line[COMMAND_SIZE];

int nFrq[] = {
  //0
  262 /*C*/, 277, 
  //2
  294 /*D*/, 311, 
  //4
  330 /*E*/, 
  //5
  349 /*F*/, 370, 
  //7
  392 /*G*/, 415, 
  //9
  440 /*A*/, 466, 
  //b
  494 /*B*/, 
  //c
  523 /*C5*/, 554,
  //e
  587 /*D5*/, 622,
  //g
  659 /*E5*/,
  //h
  698 /*F5*/, 740,
  //j
  784 /*G5*/, 831,
  //l
  880 /*A5*/, 932,
  //n
  988 /*B5*/,
  //o
  1047 /*C6*/
};

int nDur[] = {2000, 1500, 1000, 750, 500, 375, 250};

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