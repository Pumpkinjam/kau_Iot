#include <AWS_IOT.h>
#include <WiFi.h>
#include <ESP32_Servo.h>
#include <Arduino_JSON.h>

Servo servo1;
AWS_IOT MOTORIOT;
const char* ssid = "SK_WiFi67AD";
const char* password = "1102000710";
char HOST_ADDRESS[]= "acj2gilk7nyok-ats.iot.ap-northeast-2.amazonaws.com";
char CLIENT_ID[]= "KAU_MOTOR";
char sTOPIC_NAME[]= "doormotor"; // subscribe topic name
char pTOPIC_NAME[]= "esp32/doorlock"; // publish topic name
int status = WL_IDLE_STATUS;
int msgCount=0,msgReceived = 0;
char payload[512];
char rcvdPayload[512];
unsigned long preMil = 0; 
const long intMil = 5000;
static const int servoPin = 33;

int a = 0; //////////////////////////////////////////////////////// 임시 변수 1= 도어락을 여세요; 2 = 도어락을 잠구세요 3 = 문 열려있음, 4 = 문 닫혀있음 

#define sensor 33
#define Door_sensor 25


void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
  strncpy(rcvdPayload,payLoad,payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

void MOTOR(int door){ // 1은 열림 0은 닫힘
  if(door == 1){
    servo1.write(0);
    delay(1000);
    servo1.write(90);
  }
}

void setup() {
  Serial.begin(115200);
  servo1.attach(servoPin);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode()); //++choi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to wifi");
  if(MOTORIOT.connect(HOST_ADDRESS,CLIENT_ID)== 0) {
    Serial.println("Connected to AWS");
    delay(1000);
    if(0==MOTORIOT.subscribe(sTOPIC_NAME,mySubCallBackHandler)) {
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
  pinMode(sensor, INPUT);
  pinMode(Door_sensor, INPUT);
}

void loop() {
  if(msgReceived == 1)
  {
    msgReceived = 0;
// Parse JSON
    JSONVar myObj = JSON.parse(rcvdPayload);
    JSONVar state = myObj["state"];
    String doormotor = (const char*) state["doormotor"];
    if (doormotor == "OPEN"){MOTOR(1);}
    else if (doormotor == "ClOSE"){MOTOR(0);}
  }
  if ((millis() - preMil) > intMil) {
    preMil = millis();
    JSONVar bmeValues;
    bmeValues["doorsensor"] = digitalRead(Door_sensor);
    JSONVar reported;
    reported["reported"] = bmeValues;
    JSONVar state;
    state["state"] = reported;
    JSON.stringify(state).toCharArray(payload, 512);
   
  }
}
