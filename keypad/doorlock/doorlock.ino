#include "connection_data.h"
#include "WiFiData.h"
#include "AWS_Connect.h"
#include "time.h"


const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600*9; // 3600
const int daylightOffset_sec = 0; // 3600

sTOPIC_NAME = "doorlock/setpw";

#include <EEPROM.h>
#define EEPROM_SIZE 200
#define COMMAND_SIZE 300

#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 4;
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

bool pw_compare(String input) {
    int z = EEPROM.read(16);
    
    for (int i = 0; i < z; i++) {
        if (input.charAt(i) != doorPassword.charAt(i)) return false;
    }
    return input.charAt(z) == '\0';
}

void setup() {
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);

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
    
    // if there's some password saved, EEPROM[17] == 0XAA && EEPROM[18] == 0X55
    if (EEPROM.read(17) == 0XAA && EEPROM.read(18) == 0x55) {
        for (int i = 0, z = EEPROM.read(16); i < z; i++) {
            doorPassword += EEPROM.read(i);
        }
    }
    else {
        Serial.println("Doorlock password is not initialized.");
    }
}

/*
EEPROM[0 ~ ] : original password
EEPROM[16] : original password length
EEPROM[17 & 18] : password was set or not
EEPROM[19] : 0 if using original password, 1 if using temporary password by time
EEPROM[20 ~] : temporary password
EEPROM[36] : temporary password length
*/
void loop() {
    if (msgReceived == 1) {
        msgReceived = 0;  // Semaphore needed if it's multiprocessor
        Serial.print("Received Message: ");
        Serial.println(rcvdPayload);

        // Parse JSON
        JSONVar myObj = JSON.parse(rcvdPayload);    // myObj has { "newpw" : "1234" }
        char newpw[20] = myObj["newpw"];

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
        if (isGoodPw) {
            EEPROM.write(16, len);
            for (int i = 0; i < len; i++) {
                EEPROM.write(i, newpw[i]);
            }
            Serial.println("Password reset succeed.");
        }
    }
    
    
    keyPressed = customKeypad.getKey();
    if (keyPressed != NO_KEY) {
        if (keyPressed == '*')
            Serial.println("passwword input start");

        int length = 0;

        while(1){
            keyPressed = customKeypad.getKey();
            if(keyPressed>='0' && keyPressed<='9'){
                Serial.print(keyPressed);
                inputPassword += keyPressed;
                length++;
                delay(100);
            }
            if (keyPressed == '#'){
                inputPassword += '\0';

                Serial.print('\n');
                Serial.println(inputPassword);
                
                if (pw_compare(inputPassword)) {
                    Serial.println("Door opened.");
                }
                else { Serial.println("Wrong password."); }

                inputPassword="";
                break;
            }
        }
        
    }
  
      
}
