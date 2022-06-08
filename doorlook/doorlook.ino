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
char pTOPIC_NAME[]= "esp32/doorsensor";
int status = WL_IDLE_STATUS;
int msgCount=0,msgReceived = 0;
char payload[512];
char rcvdPayload[512];
unsigned long preMil = 0; 
const long intMil = 5000;
static const int servoPin = 33; 
int open_count = 0 ;
unsigned long timeVal;
unsigned long readTime = 0;
int door_error = 0; // 문을 강제로 열려고 했을때

#define motor_sensor 32
#define Door_sensor 25
#define door_butten 26

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
  strncpy(rcvdPayload,payLoad,payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

void MOTOR(void){ // 문을 모터로 열어줬다가 문이 닫히면 문을 잠구어 줌
  servo1.write(100);
  delay(1000);
  servo1.write(90);
  while(open_count < 200 && digitalRead(door_butten) == 1){  // 문 열었을때
    delay(1);
    if(digitalRead(Door_sensor)==0)open_count ++;
    if(digitalRead(Door_sensor)==1)open_count = 0;
  }Serial.println("문이 열렸습니다.");
  open_count = 0;
  while(open_count < 3000 && digitalRead(door_butten) == 1){ //문 닫았을때
    delay(1);
    if(digitalRead(Door_sensor)==1)open_count ++;
    if(digitalRead(Door_sensor)==0)open_count = 0;
  }Serial.println("문이 닫혔습니다.");
  open_count = 0;
  servo1.write(80);
  while(open_count < 500){ // 모터를 센서에 감지될 때 까지 닫아줌
    if(digitalRead(motor_sensor)==1)open_count ++;
    if(digitalRead(motor_sensor)==0)open_count = 0;
  }Serial.println("센서에 감지되었습니다.");
  open_count = 0;
   delay(50);
  servo1.write(90);
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
  pinMode(door_butten, INPUT);
}

void loop() {
  if(digitalRead(door_butten) == 0){
    delay(300);
    MOTOR();
    }
  if(msgReceived == 1)
  {
    msgReceived = 0;
// Parse JSON
    JSONVar myObj = JSON.parse(rcvdPayload);
    JSONVar state = myObj["state"];
    String doormotor = (const char*) state["doormotor"]; // esp32/doorset 
    if (doormotor == "OPEN"){
      MOTOR();
      door_error = 0;
    }
  }
  if ((millis() - preMil) > intMil) {
    preMil = millis();
    if(door_error == 0 && digitalRead(Door_sensor) == 0){
      JSONVar state;
      state["door"] = 1; //esp32/doorsensor
      JSON.stringify(state).toCharArray(payload, 512);
      if (MOTORIOT.publish(pTOPIC_NAME, payload) == 0) {
        Serial.println("문이강제로 열렸습니다");
        door_error = 1;
      }
      else { Serial.println("Oops, Publish Failed."); }
      delay(1000);
    }
  }
  if(digitalRead(Door_sensor) == 0){ // 문이 1분동안 열려있으면 문이 열려있다고 문자를 보냄
    if(millis()-timeVal >= 60000){
    readTime = millis()/60000;
    JSONVar state;
    state["door"] = 3; //esp32/doorsensor
    JSON.stringify(state).toCharArray(payload, 512);
    Serial.println("1분 경과");
    timeVal = millis();
    if (MOTORIOT.publish(pTOPIC_NAME, payload) == 0) {
      Serial.print("Publish Message: ");
      Serial.println(payload);
    }
    else { Serial.println("Oops, Publish Failed."); }
    }
  }
  if(digitalRead(Door_sensor) == 1){
    timeVal = millis();
  }
}
