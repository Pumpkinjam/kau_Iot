#include "Task07-C.h"

// Server Connection variables
const char* ssid = "WiFi_ssid";
const char* password = "WiFi_pw";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600*9; // 3600
const int daylightOffset_sec = 0; // 3600
WiFiServer server(80);

String header;

unsigned long currentTime = millis();
unsigned long previousTime = millis();
const long timeoutTime = 2000;

// alarm variables
int alarmHour, alarmMinute;

// music playing variables
const int ledChannel = 0;
const int resolution = 8;
const int buzPin = 22;

int nFrq[] = {
  //0
  262 /*C*/, 277, 
  //2
  294 /*D*/, 311, 
  //4
  330 /*E*/, 
  //5
  349 /*F*/, 370, 
  //7
  392 /*G*/, 415, 
  //9
  440 /*A*/, 466, 
  //b
  494 /*B*/, 
  //c
  523 /*C5*/, 554,
  //e
  587 /*D5*/, 622,
  //g
  659 /*E5*/,
  //h
  698 /*F5*/, 740,
  //j
  784 /*G5*/, 831,
  //l
  880 /*A5*/, 932,
  //n
  988 /*B5*/,
  //o
  1047 /*C6*/
};

int nDur[] = {2000, 1500, 1000, 750, 500, 375, 250};

// functions
void checkHour();
void checkMinute();
int ctoi(char);
void playNote(int, int, int);


void setup() {
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);
    ledcAttachPin(buzPin, ledChannel);

    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    server.begin();
}


void loop(){
    
    struct tm ltime;
    getLocalTime(&ltime);

    if (ltime.tm_hour == alarmHour && ltime.tm_min == alarmMinute) {
        int val1 = EEPROM.read(0);
        int val2 = EEPROM.read(1);
        playNote(0, -1, 0);

        // saved score found
        if (val1 == 0xAA && val2 == 0x55) {
            //Serial.printf("%x %x\n", val1, EEPROM.read(0));
            Serial.println("Reading from EEPROM...");

            /*
            Serial.println("saved data is: ");

            for (int idx = 0, z = EEPROM.read(2); idx < z; idx++) {
            Serial.printf("(%d) - %d\n", idx,  EEPROM.read(idx));
            }
            Serial.println();
            */
            
            // save data of note, duration, duty
            for (int idx = 3, z = EEPROM.read(2); idx+2 < z; ) {
            delay(10);
            int noteVal = EEPROM.read(idx++);
            int durVal = EEPROM.read(idx++);
            int dutyVal = EEPROM.read(idx++);
            
            playNote(noteVal, durVal, dutyVal);
            }
            
        }
        // if there's no score
        else {
            Serial.println("It's time to ring, but there's no music to play");
        }
            
    } // end of input mode check


    WiFiClient client = server.available(); // Listen for incoming clients
    if (client) { // If a new client connects,
        currentTime = millis();
        previousTime = currentTime;
        Serial.println("New Client."); // print a message out in the serial port
        String currentLine = ""; // make a String to hold incoming data from the client
        while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
            currentTime = millis();
            if (client.available()) { // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                Serial.write(c); // print it out the serial monitor
                header += c;
                
                // start reading header
                if (c == '\n') { // if the byte is a newline character
                // if the current line is blank, you got two newline characters in a row.
                // that's the end of the client HTTP request, so send a response:

                    if (currentLine.length() == 0) {
                    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                    // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                    Serial.print("Header is ");
                    Serial.println(header);
                        
                        if (header.indexOf("GET /set") >= 0) {
                            delay(100);
                            Serial.print("alarm set to - ");

                            checkHour(header);
                            checkMinute(header);
                            delay(10);
                            Serial.printf("%d : %d\n", alarmHour, alarmMinute);
                            printPage(&client);
                            client.printf("<h2>Alarm was set to %d : %d</h2>\n", alarmHour, alarmMinute);
                        }
                        else {
                            printPage(&client);
                        }
                        
                        
                        // The HTTP response ends with another blank line
                        client.println();
                        // Break out of the while loop
                        break;
                        } //** if (currentLine.length() == 0) {
                    else { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }

                } //** if (c == '\n') {
                else if (c != '\r') { // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }
            } //* if (client.available()) {
        } //** while
        // Clear the header variable
        header = "";
        delay(50);
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    } //** if (client) {

} //** loop() {

void checkHour(String str) {
    static int firstSlashIndex = str.indexOf("/");
    // xxx.xxx.xxx.xxx/set/h**/m**
    
    int a = str.charAt(firstSlashIndex + 6) - '0';
    int b = str.charAt(firstSlashIndex + 7) - '0';
    
    alarmHour = 10*a + b;
}

void checkMinute(String str) {

    static int firstSlashIndex = str.indexOf("/");
    // xxx.xxx.xxx.xxx/set/h**/m**
    
    int a = str.charAt(firstSlashIndex + 10) - '0';
    int b = str.charAt(firstSlashIndex + 11) - '0';
    
    alarmMinute = 10*a + b;
}

int ctoi(char c) {
  return ('0' <= c && c <= '9' ? (int)(c - '0') : ('a' <= c && c <= 'z' ? (int)(c - 'a' + 10) : -1));
}

void playNote(int note, int dur, int duty) {
  // not a code for user
  if (dur == -1) {
    ledcSetup(ledChannel, 0, resolution);
    ledcWrite(ledChannel, 0);
    delay(1);
    return;
  }
  
  ledcSetup(ledChannel, nFrq[note], resolution);
  ledcWrite(ledChannel, duty);
  //Serial.println(duty == 0 ? -1 : note);
  
  //Serial.printf("%d %d %d\n", note, dur, duty);
  delay(nDur[dur]);
}
