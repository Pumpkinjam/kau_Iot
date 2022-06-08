#include <AWS_IOT.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include<Adafruit_Sensor.h>
#include<Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; //I2C
AWS_IOT testButton;
const char* ssid= "12345";
const char* password = "hwang1151";
char HOST_ADDRESS[]= "acj2gilk7nyok-ats.iot.ap-northeast-2.amazonaws.com";
char CLIENT_ID[]= "ChoiESP32";
char sTOPIC_NAME[]= "test/image";  // subscribe topic name
char pTOPIC_NAME[]= "$aws/things/ESP32_BME280/shadow/update";   // publish topic name
int status = WL_IDLE_STATUS;
int msgCount=0,msgReceived = 0;
char payload[512];
char rcvdPayload[512];
const int buttonPin= 15;  // pushbutton pin
unsigned long preMil= 0; 
const long intMil= 5000;
const long pubPeriod = 10000;
unsigned long lastPublishedMil = 0;
int temp;
int pressure;
int humidity;
String rcvdMQTT;


const int ledPin = 16;


void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
  Serial.println(payloadLen);
  for(int i = 0; i<payloadLen;i++){
    rcvdMQTT += payLoad[i];
    
  }
  rcvdMQTT+=payLoad;
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

void setup() {
  Serial.begin(115200);

   
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());//++choi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to wifi");
  if(testButton.connect(HOST_ADDRESS,CLIENT_ID)== 0) {
    Serial.println("Connected to AWS");
    delay(1000);
    if(0==testButton.subscribe(sTOPIC_NAME,mySubCallBackHandler)) {
      Serial.println("Subscribe Successfull");
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
  // initialize the pushbutton pin as an input
  delay(2000);
}



void loop() {
  if(msgReceived== 1) {
    msgReceived= 0;
    Serial.println("Received Message:");
    Serial.println(rcvdMQTT);
    Serial.println(rcvdMQTT.length());
    rcvdMQTT = "";
  }
 
}
