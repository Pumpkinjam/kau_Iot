/* This code is based on Task09-B */
#include <AWS_IOT.h>    
#include <WiFi.h>
#include <Arduino_JSON.h>
#include "connection_data.h"

AWS_IOT hornbill;

/* Data below are defined in 'connection_data.h'
 * 
 * const char* ssid = WIFI_SSID;
 * const char* password = WIFI_PW;
 * char HOST_ADDRESS[] = AWS_DEVICE_DATA_ENDPOINT;
 * 
 * these are not supposed to be uploaded to github
*/
char CLIENT_ID[] = "PumpkinjamESP32";
char sTOPIC_NAME[] = "esp32/led";

// but this tiny cute pTOPIC_NAME won't be used in this code
char pTOPIC_NAME[] = "esp32/bme280";

int status = WL_IDLE_STATUS;
int msgCount = 0, msgReceived = 0;
char payload[512];
char rcvdPayload[512];

const int ledPin = 16;
unsigned long preMil = 0;
const long intMil = 5000;   // button Cooldown 5s

void mySubCallBackHandler(char* topicName, int payloadLen, char* payLoad) {
  // set rcvdPayload(recieved payload) to payload
  strncpy(rcvdPayload, payLoad, payloadLen);
  // add null character
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

void setup() {
  Serial.begin(115200);

  // Initialize WIFI
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.disconnect(true);
  delay(1000);

  WiFi.mode(WIFI_STA);
  delay(1000);

  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Initialize LED
  if (hornbill.connect(HOST_ADDRESS, CLIENT_ID) == 0) {
    Serial.println("Connected to AWS");
    delay(1000);

    if (0 == hornbill.subscribe(sTOPIC_NAME, mySubCallBackHandler)) {
      Serial.println("Subscribe Succeed");
    }
    else {
      Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
      while(1);
    }
  }
  else {
    Serial.println("AWS connection failed, Check the HOST Address");
    while(1);
  }
  pinMode(ledPin, OUTPUT);
  delay(2000);
}

void loop() {
  if (msgReceived == 1) {
    msgReceived = 0;  // Semaphore needed if it's multiprocessor
    Serial.print("Received Message: ");
    Serial.println(rcvdPayload);

    // Parse JSON
    JSONVar myObj = JSON.parse(rcvdPayload);    // myObj has { "state" : {"led" : "ON" | "OFF"} }
    JSONVar state = myObj["state"];             // state has { "led" : "ON" | "OFF" }
    String led = (const char*)state["led"];     // led has "ON" | "OFF"

    Serial.print("LED will be ");
    Serial.println(led);
  
    if (led == "ON") { digitalWrite(ledPin, HIGH); }
    else { digitalWrite(ledPin, LOW); }
  }

}
