#include <AWS_IOT.h>
#include <WiFi.h>
#include <base64.h>

#include "OV7670.h"

AWS_IOT doorBellIOT;
const char* ssid = "12345";
const char* password = "hwang1151";
char HOST_ADDRESS[] = "acj2gilk7nyok-ats.iot.ap-northeast-2.amazonaws.com";
char CLIENT_ID[] = "KAU_DOORBELL";
char pTOPIC_NAME[]= "esp32/button";   // publish topic name
int status = WL_IDLE_STATUS;
char payload[512];
const int buzPin = 0;
const int buttonPin = 23;
const int ledChannel = 0;
const long intMil = 1000;
unsigned long preMil = 0;
const int resolution = 8;
const int duty = 128;

const int SIOD = 21; //SDA
const int SIOC = 22; //SCL

const int VSYNC = 34;
const int HREF = 35;

const int XCLK = 32;
const int PCLK = 33;

const int D0 = 27;
const int D1 = 17;
const int D2 = 16;
const int D3 = 15;
const int D4 = 14;
const int D5 = 13;
const int D6 = 12;
const int D7 = 4;

OV7670 *camera;

String encodedImage;


void wifiConnect(){
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
}

void iotSetting(){
  if(doorBellIOT.connect(HOST_ADDRESS,CLIENT_ID)==0){
    Serial.println("Connected to AWS");
    delay(1000);
  }
  else {
    Serial.println("AWS connection failed, Check the HOST Address");
    while(1);
  }
}

void ringBell(int dur){
  Serial.println("ring");
  ledcSetup(ledChannel,466,resolution);
  ledcWrite(ledChannel,duty);
  delay(dur);
  ledcSetup(ledChannel,370,resolution);
  ledcWrite(ledChannel,duty);
  delay(dur);
  ledcWrite(ledChannel,0);
  
}

void take_picture(){
  camera->oneFrame();
  Serial.println(String(camera->xres * camera->yres * 2));
  Serial.println(base64::encode(camera->frame, camera->xres * camera->yres * 2));
}

void pushToAWS(){
  sprintf(payload,"test");
  if(doorBellIOT.publish(pTOPIC_NAME,payload) == 0){
    Serial.print("Publish Message:");
    Serial.println(payload);
  }
  else
        Serial.println("Publish failed");
}

void buttonPush(){
  Serial.println("Button pushed");
  //ringBell(700);
  take_picture();
  //encodedImage = base64::encode(camera->frame, camera->xres * camera->yres * 2);
  
  //pushToAWS();
  
  
}

//test



void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
   camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
   wifiConnect();
   iotSetting();
   // ledcAttachPin(buzPin, ledChannel);
   pinMode(buttonPin,INPUT);
}

void loop() {
  camera->oneFrame();
  //Serial.println(String(camera->xres * camera->yres * 2));
  Serial.println(base64::encode(camera->frame, camera->xres * camera->yres * 2));
  // put your main code here, to run repeatedly:
  if((millis()-preMil)>intMil){
    if(digitalRead(buttonPin)){
      preMil=millis();
      buttonPush();
    }
  }
}
