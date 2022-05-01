/* This code is based on Task09-C which is based on Task09-A which is based on Task09-B :D */
#include <AWS_IOT.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include "connection_data.h"


AWS_IOT hornbill;

/* BME280 class
 * 
 * Temperature (*C) can be get by:
 *  bme.readTemperature()
 * Humidity (%) can be get by:
 *  bme.readHumidity()
 * Pressure (hPa) can be get by:
 *  bme.readPressure() / 100.0F
 */
Adafruit_BME280 bme;    // I2C

/* Data below are defined in 'connection_data.h'
 * 
 * const char* ssid = WIFI_SSID;
 * const char* password = WIFI_PW;
 * char HOST_ADDRESS[] = AWS_DEVICE_DATA_ENDPOINT;
 * char CLIENT_ID[] = CLIENT_ID;
 * char sTOPIC_NAME[] = "$aws/things/<Thing_Name>/shadow/update/delta";
 * char pTOPIC_NAME[] = "$aws/things/<Thing_Name>/shadow/update";
 * 
 * these are not supposed to be uploaded to github
*/

int status = WL_IDLE_STATUS;
int msgCount = 0, msgReceived = 0;
char payload[512];
char rcvdPayload[512];

const int ledPin = 16;
bool ledStatus = false;       // this variable shows if led is now ON or OFF
unsigned long preMil = 0;
const long intMil = 10000;   // BME280 Cooldown 10s

double temp, humid, press;

void mySubCallBackHandler(char* topicName, int payloadLen, char* payLoad) {
  // set rcvdPayload(recieved payload) to payload
  strncpy(rcvdPayload, payLoad, payloadLen);
  // add null character
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

void setup() {
  Serial.begin(115200);

  // Initialize BME280
  bool bme_status;
  
  for (int i = 0x00; i <= 0xFF; i++) {
      bme_status = bme.begin(i);
      if (bme_status) {break;}
  }
  // if sensorID was not found
  if (!bme_status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
    Serial.print(" ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print(" ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print(" ID of 0x60 represents a BME 280.\n");
    Serial.print(" ID of 0x61 represents a BME 680.\n");
    Serial.println("Initialize Failed.");
    while (1) delay(10);
  }
  Serial.println("BME280 initialized.");
  
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
      Serial.println("Initialize Failed.");
      while(1);
    }
  }
  else {
    Serial.println("AWS connection failed, Check the HOST Address");
    Serial.println("Initialize Failed.");
    while(1);
  }
  pinMode(ledPin, OUTPUT);

  Serial.println("Initialize completed.");
  delay(2000);
}

void loop() {

    // when recieved message from subscribed topic
  if (msgReceived == 1) {
    msgReceived = 0;  // Semaphore needed if it's multiprocessor
    Serial.print("Received Message: ");
    Serial.println(rcvdPayload);

    // Parse JSON
    JSONVar myObj = JSON.parse(rcvdPayload);    // myObj has { ~~ "state" : {"led" : "ON" | "OFF"} ~~ }
    JSONVar state = myObj["state"];             // state has {"led" : "ON" | "OFF"}
    String led = (const char*)state["led"];     // led has "ON" | "OFF"

    Serial.print("LED will be ");
    Serial.println(led);
  
    if (led == "ON") { 
      digitalWrite(ledPin, HIGH); ledStatus = true;
//      hornbill.publish(pTOPIC_NAME, "{\"state\": {\"reported\": {\"led\": \"ON\"} } }");
    }
    else { 
      digitalWrite(ledPin, LOW); ledStatus = false;
//      hornbill.publish(pTOPIC_NAME, "{\"state\": {\"reported\": {\"led\": \"ON\"} } }");
    }
  }

    // this will be operated every 10 seconds.
  if ((millis() - preMil) > intMil) {
    preMil = millis();

    // read Values from BME280
    temp = bme.readTemperature();
    humid = bme.readHumidity();
    press = bme.readPressure() / 100.0F;

    /* We need to make JSON in this form:
     * { "state" : {
            "reported" : {
                "temp" : 1415, "humid" : 9265, "press" : 3589, "led" : "ON | OFF"
                "yuzu" : "soft", "do a" : "barrel roll", "nerf" : "this"
            } 
        }
     }
     */
    JSONVar bmeValues;
    bmeValues["temp"] = temp;
    bmeValues["humid"] = humid;
    bmeValues["press"] = press;
    bmeValues["led"] = ledStatus ? "ON" : "OFF";
    JSONVar reported;
    reported["reported"] = bmeValues;
    JSONVar state;
    state["state"] = reported;

    // change JSON to String
    // and then change String to char*
    // this string will be saved to payload
    JSON.stringify(state).toCharArray(payload, 512);
    
    if (hornbill.publish(pTOPIC_NAME, payload) == 0) {
      Serial.print("Publish Message: ");
      Serial.println(payload);
    }
    else { Serial.println("Oops, Publish Failed."); }
  }
   
}
