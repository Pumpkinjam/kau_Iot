#include <Keypad.h>
#include <LiquidCrystal.h>
#include <AWS_IOT.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include <Wire.h>
#include <EEPROM.h>
#define EEPROM_SIZE 1024
#define SWAP 0 // sw access point
#include <AWS_IOT.h>
//#include "secret.h"

AWS_IOT aws;

char CLIENT_ID[] = "ysESP32";
char sTOPIC_NAME[] = "$aws/things/ESP32_Doorlock/shadow/get/accepted";
char SENT_GET[]= "$aws/things/ESP32_Doorlock/shadow/get";

char pTOPIC_NAME[] = "esp32/doorsensor";

int status = WL_IDLE_STATUS;
int msgCount = 0, msgReceived = 0;
char payload[512];
char reportpayload[512];
char rcvdPayload[512];

//const char* ssid = "-";
//const char* password = "-!";
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
bool isTemp = false;     //임시 비밀번호가 세팅되었을 때
int temp = 3;

const char* ntpServer = "kr.pool.ntp.org";
const long gmtOffset_sec = 3600*9;  //3600
const int daylightOffset_sec = 0; // 3600

int door = 2;
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
String newpw0 = "";
String lcdmsg0 = "";        
String timeset0 = "";
String temp0 = "";

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
  doorPassword = "";
  int z = EEPROM.read(16);
    
    for (int i = 0; i < z; i++) {
      doorPassword += EEPROM.read(i) - '0';
    }
    Serial.print("original password set :");
    Serial.println(doorPassword);
}

void save_password(String p, int len){
  Serial.println("EEPROM save :");
  EEPROM.write(16, len);
  EEPROM.commit();
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
// timeStringBuff => "June 06 2022 13:48"
// time => "222211110000"
bool check_time(String t){
  //날짜가 같은지
  if (timeStringBuff[5] == t[6] &&timeStringBuff[6] == t[7]){
    //시각이 같은지
    if(timeStringBuff[13] == t[8] &&timeStringBuff[14] == t[9]){
      //분이 같은지
       if (timeStringBuff[16] == t[10] &&timeStringBuff[17] == t[11]){
          return true;  
       }
       else{
        return false;
        Serial.println("time3"); 

       } 
    }
    else{
      return false;
      Serial.println("time2"); 
    }
  }
  
  else{
    return false;
    Serial.println("time1"); 
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
  
  if (aws.connect(HOST_ADDRESS, CLIENT_ID) == 0) {
    Serial.println("Connected to AWS");
    delay(1000);

    if (0 == aws.subscribe(sTOPIC_NAME, mySubCallBackHandler)) {
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
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  pinMode(buttonPin, INPUT_PULLUP);
  currentButtonState = digitalRead(buttonPin);

  //초기값 지정
//  EEPROM.write(17, 0XAA);
//  EEPROM.write(18, 0X55);
//  EEPROM.commit();
//  save_password("12345678", 8);

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
            doorPassword += EEPROM.read(i) - '0';
            
            
        }
        Serial.print("door Password : ");
        Serial.println(doorPassword);
    }
   else {
        Serial.println("Doorlock password is not initialized.");
        //임의
        //doorPassword = "12345678";
   }
   // get shadow state
    if(aws.publish(SENT_GET,"{}") == 0)
      {       
        Serial.print("Empty String Published\n");
      }
    else
      {
        Serial.println("Empty String Publish failed\n");
      }  /*Sent Empty string to fetch Shadow desired state*/   
  
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
    JSONVar doorState;
    doorState["door"] = door;
         
    if (msgReceived == 1) {
        msgReceived = 0;  // Semaphore needed if it's multiprocessor
        Serial.print("Received Message: ");
        Serial.println(rcvdPayload);

        // Parse JSON
        JSONVar myObj = JSON.parse(rcvdPayload);    
        JSONVar state = myObj["state"];
        JSONVar reported = state["reported"];    // reported has { "newpw" : "1234", "lcd" : "a", "time" : "202206062349" , "temp" : 0 }
        
        String newpw = (const char*)reported["newpw"];
        String lcdmsg = (const char*)reported["lcd"];        
        String timeset = (const char*)reported["time"];
        String temp = (const char*)reported["temp"];

        bool isGoodPw = true;
        int len = 0;
        // check whether newpw characters are between 0 to 9 or not
        for (int i = 0; newpw[i] != 0; i++) {
            char tmp = newpw[i];
            Serial.print("tmp: ");
            Serial.println(tmp);
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

        // reported{} 값이 변경됐을 때
        if (newpw0 != newpw || lcdmsg0 != lcdmsg || timeset0 != timeset || temp0 != temp){
          newpw0 = newpw;
          lcdmsg0 = lcdmsg;
          timeset0 = timeset;
          temp0 = temp;
          Serial.println("-----shadow value changed-----");
          Serial.println(isGoodPw);
          Serial.println(newpw);
          Serial.println(lcdmsg);
          Serial.println(timeset);
          Serial.println(temp);
          Serial.println("---------------------------");
          // password length is saved at EEPROM[16]
          // if there's some password saved, EEPROM[17] == 0XAA && EEPROM[18] == 0X55       
  
          // lcd 메시지 출력 => temp = 0 일때
          // temp = 1 => 비밀번호 영구 변경
          // temp = 2 임시 비밀번호 변경
          if (temp == "0")
          {                  
            if (lcdmsg == "a"){
              My_LCD.clear();
              Serial.println("start 1");
              My_LCD.begin(16, 2);
              My_LCD.print("Hello 1");
            }
            else if (lcdmsg == "b"){
              My_LCD.clear();
              Serial.println("start 2");
              My_LCD.begin(16, 2);
              My_LCD.print("Hello 2");          
            }
            else if (lcdmsg == "c"){
              My_LCD.clear();
              Serial.println("start 3");
              My_LCD.begin(16, 2);
              My_LCD.print("Hello 3");          
            }
            else{
              My_LCD.clear();
              Serial.println("start 4");
              My_LCD.begin(16, 2);
              My_LCD.print("Hello 4");          
            }
          }
          //비밀번호 영구 변경                
          else if (isGoodPw && temp == "1") {
              EEPROM.write(16, len);
              for (int i = 0; i < len; i++) {
                  EEPROM.write(i, newpw[i]);
              }
              load_password();
              Serial.println("Password reset succeed.");
          }
         // 임시 비밀번호 시간 설정
          else if (temp == "2" && isGoodPw){
            //변수에 저장
            tempTimeSet = timeset;
            Serial.print("tempTimeSet : ");
            Serial.println(tempTimeSet);
            isTemp = true;
            Serial.print("tempPasswordSet : ");
            doorPassword = newpw;
            Serial.println(doorPassword);
                      
           }
           
        }
        
   
      
        
      }
      // 임시 비밀번호 시간이 다 되면
      if (isTemp && check_time(tempTimeSet)==true ){
          Serial.println("------temp time over ------");
          isTemp = false;
          load_password();
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
              //inputPassword += '\0';
              
              Serial.print('\n');
              Serial.println(inputPassword);
              if (doorPassword == inputPassword) {
                playNote(20, 250);
                playNote(18, 250);
                playNote(17, 125);
                playNote(13, 125);
                playNote(13, 250);
                playNote(18, 400);
  
                door = 1;
                Serial.println("password correct : ");
                sprintf(payload, "Password correct!");
  
                JSON.stringify(doorState).toCharArray(payload, 512);
                
                if (aws.publish(pTOPIC_NAME, payload) == 0) {
                  Serial.print("Publish Message: ");
                  Serial.println(payload);
                }
                else { 
                  Serial.println("Oops, Publish Failed."); 
                }
              }
              else{
                playNote(18, 250);
  
                door = 0;
                Serial.println("password error : ");
                Serial.println(doorPassword);
                Serial.println(inputPassword);
                
                JSON.stringify(doorState).toCharArray(payload, 512);
                sprintf(payload, "Password Error!");
  
                if (aws.publish(pTOPIC_NAME, payload) == 0) {
                  Serial.print("Publish Message: ");
                  Serial.println(payload);
                }
                else { 
                  Serial.println("Oops, Publish Failed."); 
                }
              }
    
              inputPassword="";
              door = 2;
              break;
            }
          } 
        }
      }

            
  } 
}

  //lastButtonState = currentButtonState;
