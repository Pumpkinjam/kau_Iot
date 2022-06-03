#include <Keypad.h>
#include <LiquidCrystal.h>
#include <AWS_IOT.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <EEPROM.h>
#define EEPROM_SIZE 1024
#define SWAP 0 // sw access point
#include <AWS_IOT.h>
//#include "secret.h"

AWS_IOT hornbill;

char CLIENT_ID[] = "ysESP32";
char sTOPIC_NAME[] = "esp32/lcd";
char pTOPIC_NAME[] = "esp32/doorlock";

int status = WL_IDLE_STATUS;
int msgCount = 0, msgReceived = 0;
char payload[512];
char rcvdPayload[512];

//const char* ssid = "-";
//const char* password = "-";
//char HOST_ADDRESS[] = "acj2gilk7nyok-ats.iot.ap-northeast-2.amazonaws.com";

const char* ssid = "-";
const char* password = "-";
char HOST_ADDRESS[] = "acj2gilk7nyok-ats.iot.ap-northeast-2.amazonaws.com";
WiFiServer server(80);

// Create An LCD Object. Signals: [ RS, EN, D4, D5, D6, D7 ]
LiquidCrystal My_LCD(32, 33, 25, 26, 27, 14);

const int freq = 5000; 
const int ledChannel = 0; 
const int resolution = 8; 
const int buzPin = 22; 
int duty = 128;
const int buttonPin = 15;

const byte ROWS = 4;
const byte COLS = 4;
int currentButtonState;
int lastButtonState;

bool isGoodPw = true;

char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};
byte rowPins[ROWS] = {21,19,18,5};
byte colPins[COLS] ={17,16,4,15};
//byte rowPins[ROWS] = {19,18,5,17};
//byte colPins[COLS] ={16,4,23,15};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
char keyPressed;
String inputPassword = "";
String doorPassword = "";
String tempPassword = "";
//String memoryPassword = "12345678";

String val;
String p_val;

//시간 관련 변수

unsigned long timeVal;    //이전 시간
unsigned long readTime;   //현재 타이머 시간
char timeStringBuff[50]; //50 chars should be enough
String tempTimeSet;      //임시 비밀번호 시간 저장
bool isTemp = false;            //임시 비밀번호가 세팅되었을 때

const char* ntpServer = "kr.pool.ntp.org";
const long gmtOffset_sec = 3600*9;  //3600
const int daylightOffset_sec = 0; // 3600


int sVal;
int nFrq[] = {131, 139, 147, 156, 165, 
175, 262, 277, 294, 311, 
330, 349, 370, 392, 415, 
440, 466, 494, 523, 554, 
587, 622, 659, 698,740, 
784, 831, 880, 932, 988};
int keyNum;
//18 22
unsigned long preMil = 0;
const long intMil = 5000;

void playNote(int note, int dur) { 
  //duty = 0;
  ledcSetup(ledChannel, nFrq[note], resolution); 
  
  ledcWrite(ledChannel, duty); 
  //Serial.println(note);
  delay(dur);
  ledcWrite(ledChannel, 0);
  delay(20);
}

void load_password(){
  int z = EEPROM.read(16);
    
    for (int i = 0; i < z; i++) {
      doorPassword += EEPROM.read(i);
    }
}

void save_password(String p, int len){
  Serial.println("EEPROM save :");
  EEPROM.write(16, len);
  
  for ( int i = 0; i< len; i++){
    EEPROM.write(i, p[i]);  
    Serial.print(p[i]); 
  }
  Serial.println(" ");
  EEPROM.commit();
}


bool pw_compare(String input) {
    int z = EEPROM.read(16);
    
    for (int i = 0; i < z; i++) {
        if (input.charAt(i) != doorPassword.charAt(i)) return false;
    }
    return input.charAt(z) == '\0';
}

bool check_time(String t){
  if (timeStringBuff[16] == t[10] &&timeStringBuff[17] == t[11]){
    return true;  
  }
  else{
    return false;
  }
}

void mySubCallBackHandler(char* topicName, int payloadLen, char* payLoad) {
  // set rcvdPayload(recieved payload) to payload
  strncpy(rcvdPayload, payLoad, payloadLen);
  // add null character
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}
void printLocalTime()
{
  struct tm timeinfo; 
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time"); 
    return;
  } 
  //timeinfo를 string값으로
  strftime(timeStringBuff, sizeof(timeStringBuff), "%B %d %Y %H:%M", &timeinfo);
  String asString(timeStringBuff);
  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  pinMode(buttonPin, INPUT_PULLUP);
  currentButtonState = digitalRead(buttonPin);
  
  //save_password("12345678");

  wifi_aws_Connect();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); 
  printLocalTime();
  
  ledcAttachPin(buzPin, ledChannel);
  My_LCD.clear();
  Serial.println("start");
  My_LCD.begin(16, 2);
  My_LCD.print("Hello World!");
  Serial.println("Hello World!");

  if (EEPROM.read(17) == 0XAA && EEPROM.read(18) == 0x55) {
        for (int i = 0, z = EEPROM.read(16); i < z; i++) {
            doorPassword += EEPROM.read(i);
        }
    }
   else {
        Serial.println("Doorlock password is not initialized.");
   }
  
}


void loop() {

  lastButtonState = currentButtonState;
  currentButtonState = digitalRead(buttonPin);
  keyPressed = customKeypad.getKey();
  printLocalTime();

  if(millis()-timeVal >= 60000){
    readTime = millis()/60000;
    Serial.println("1분 경과, 현재 시간: ");
    Serial.println(timeStringBuff);
    timeVal = millis();
  }
  if ( (millis()-preMil) > intMil){
    
    if (msgReceived == 1) {
        msgReceived = 0;  // Semaphore needed if it's multiprocessor
        Serial.print("Received Message: ");
        Serial.println(rcvdPayload);

        // Parse JSON
        JSONVar myObj = JSON.parse(rcvdPayload);    // myObj has { "newpw" : "1234", "lcd" : "a", "time" : "202206062349" }
        const char* newpw = (const char*)myObj["newpw"];
        const char* lcdmsg = (const char*)myObj["lcd"];
        const char* timeset = (const char*)myObj["time"];
        

        bool isGoodPw = true;
        int len = 0;
        // check whether newpw characters are between 0 to 9 or not
        for (int i = 0; newpw[i] != 0; i++) {
            char tmp = newpw[i];
            if (tmp < '0' || tmp > '9') {
                Serial.println("Password reset failed : unavailable character");
                isGoodPw = false; break;
            }
            len++;
            if (len > 16) {
                Serial.println("Password reset failed : length over 16");
                isGoodPw = false; break;
            }
        }        
        // password length is saved at EEPROM[16]
        // if there's some password saved, EEPROM[17] == 0XAA && EEPROM[18] == 0X55
        

        // lcd 메시지 출력
        if (lcdmsg == "a"){
          My_LCD.clear();
          Serial.println("start");
          My_LCD.begin(16, 2);
          My_LCD.print("1");
        }
        else if (lcdmsg == "b"){
          My_LCD.clear();
          Serial.println("start");
          My_LCD.begin(16, 2);
          My_LCD.print("2");          
        }
        else if (lcdmsg == "c"){
          My_LCD.clear();
          Serial.println("start");
          My_LCD.begin(16, 2);
          My_LCD.print("3");          
        }
        else{
          My_LCD.clear();
          Serial.println("start");
          My_LCD.begin(16, 2);
          My_LCD.print("4");          
        }
        //비밀번호 영구 변경 - timeset이 0이면
        if (isGoodPw && timeset == "0") {
            EEPROM.write(16, len);
            for (int i = 0; i < len; i++) {
                EEPROM.write(i, newpw[i]);
            }
            Serial.println("Password reset succeed.");
        }
       // 임시 비밀번호 시간 설정
        if (timeset != "0" && isGoodPw && !isTemp){
          //변수에 저장
          tempTimeSet = timeset;
          isTemp = true;
          Serial.println("tempPasswordSet");
          doorPassword = newpw;
          Serial.println(timeStringBuff);
                    
         }
      // 임시 비밀번호 시간이 다 되면
        if (isTemp && check_time(tempTimeSet) ){
          Serial.println("temp time over. original password set");
          isTemp = false;
          load_password();
        }
      }      
    }


    

    if (keyPressed != NO_KEY) {
      if(keyPressed == '*'){
        playNote(16, 200);
        playNote(12, 200);
        Serial.println("password input start");
        while(1){
          keyPressed = customKeypad.getKey();
          if(keyPressed>='0' && keyPressed<='9'){
            Serial.print(keyPressed);
            inputPassword += keyPressed;
            //keyNum = random(13,24);
            playNote(16, 200);
            delay(100);
          }
          if (keyPressed == '#'){
            inputPassword += '\0';
            
            Serial.print('\n');
            Serial.println(inputPassword);
            if (pw_compare(inputPassword) ) {
              playNote(20, 250);
              playNote(18, 250);
              playNote(17, 125);
              playNote(13, 125);
              playNote(13, 250);
              playNote(18, 400);  
              Serial.println("password correct");
              sprintf(payload, "Door Open!");
            }
            else{
              Serial.println("password error");
              sprintf(payload, "Password Error");
              playNote(18, 250);

            }
  
            inputPassword="";
            break;
          }
        } 
      } 
    }
}
  //lastButtonState = currentButtonState;


void wifi_aws_Connect(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");
//  WiFi.softAP(ssid, password); 
//  IPAddress IP = WiFi.softAPIP(); 
//  Serial.print("AP IP address: "); 
//  Serial.println(IP);
  #if SWAP
  WiFi.softAP(ssid, password); 
  IPAddress IP = WiFi.softAPIP(); 
  Serial.print("AP IP address: "); 
  Serial.println(IP);
  #else
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to "); 
  Serial.println(ssid); 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); 
  }
  // Print local IP address and start web server Serial.println("");
  Serial.println("WiFi connected."); 
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
  #endif
  server.begin();
}
