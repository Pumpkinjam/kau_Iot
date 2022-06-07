#include <AWS_IOT.h>
#include <WiFi.h>
#include <ESP32_Servo.h>
#include <Arduino_JSON.h>

Servo servo1;
AWS_IOT MOTORIOT;
const char* ssid = "iiiii";
const char* password = "1876075554";
char HOST_ADDRESS[]= "acj2gilk7nyok-ats.iot.ap-northeast-2.amazonaws.com";
char CLIENT_ID[]= "KAU_MOTOR";
char sTOPIC_NAME[]= "ESP32/Doorlock"; // subscribe topic name // 틀리면 
char pTOPIC_NAME[]= "esp32/doorsensor"; // publish topic name
int status = WL_IDLE_STATUS;
int msgCount=0,msgReceived = 0;
char payload[512];
char rcvdPayload[512];
unsigned long preMil = 0; 
const long intMil = 5000;
static const int servoPin = 33; 
int open_count = 0 ;

#define motor_sensor 32
#define Door_sensor 25


void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
  strncpy(rcvdPayload,payLoad,payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

void MOTOR(void){ // 문을 모터로 열어줬다가 문이 닫히면 문을 잠구어 줌
  servo1.write(100);
  while(open_count < 100){ // 모터를 센서에 감지될 때 까지 열어줌
    if(digitalRead(motor_sensor)==1)open_count ++;
    if(digitalRead(motor_sensor)==0)open_count = 0;
  }
  open_count = 0;
  servo1.write(90);
  while(open_count < 1000){  // 문 열었을때
    sensor();
    if(digitalRead(Door_sensor)==0)open_count ++;
    if(digitalRead(Door_sensor)==1)open_count = 0;
  }
  open_count = 0;
  while(open_count < 10000){ //문 닫았을때
    sensor();
    if(digitalRead(Door_sensor)==1)open_count ++;
    if(digitalRead(Door_sensor)==0)open_count = 0;
  }
  open_count = 0;
  servo1.write(45);
  delay(200);
  servo1.write(90);
}

void sensor(void){
  if ((millis() - preMil) > intMil) {
    preMil = millis();
    JSONVar state;
    state["door_sensor"] = digitalRead(Door_sensor); //esp32/doorsensor
    JSON.stringify(state).toCharArray(payload, 512);
    
    if (MOTORIOT.publish(pTOPIC_NAME, payload) == 0) {
      Serial.print("Publish Message: ");
      Serial.println(payload);
    }
    else { Serial.println("Oops, Publish Failed."); }
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
  pinMode(motor_sensor, INPUT);
  pinMode(Door_sensor, INPUT);
}

void loop() {
  
  if(msgReceived == 1)
  {
    msgReceived = 0;
// Parse JSON
    JSONVar myObj = JSON.parse(rcvdPayload);
    JSONVar state = myObj["state"];
    String doormotor = (const char*) state["doormotor"]; // esp32/doorset 
    if (doormotor == "OPEN"){
      MOTOR();
    }
  }
  sensor();
}
