#include "Task07-C.h"

//const char* ssid = "KAU-GUEST";
//const char* password = "";
const char* ssid = "SK_WiFi52B9";
const char* password = "1402007417";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600*9; // 3600
const int daylightOffset_sec = 0; // 3600
WiFiServer server(80);

String header;

unsigned long currentTime = millis();
unsigned long previousTime = millis();
const long timeoutTime = 2000;
int alarmHour, alarmMinute;

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

    if (ltime.tm_hour == alarmHour && ltime.tm_minute == alarmMinute) {
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
        // score input mode
        else {
            Serial.println("No data found. Create new one or play temporarily.");

            // stay until input
            while (Serial.available() < 1) {
                delay(10);
                //Serial.println("Waiting Input...");
                //delay(1000);
            }

            // transfer serial input to string
            int len = 0;
            while (Serial.available() > 0) {
                line[len++] = Serial.read();
            }
            line[len] = '\0';


            // save the value if cmdline starts with '<'
            if (line[0] == '<') {
                Serial.println("Saving to EEPROM...");
                
                int romIdx = 3;
                for (int lineIdx = 1; lineIdx + 2 < len - 1; lineIdx+=2) {
                    int noteVal ,dutyVal = 128, durVal = ctoi(line[lineIdx+1]);
                    if (line[lineIdx] == ',') { noteVal = 0; dutyVal = 0; }
                    else { noteVal = ctoi(line[lineIdx]);}

                    EEPROM.write(romIdx++, noteVal);
                    EEPROM.write(romIdx++, durVal);
                    EEPROM.write(romIdx++, dutyVal);
                }
            
                EEPROM.write(romIdx, -1);
                
                EEPROM.write(0, 0xAA);
                EEPROM.write(1, 0x55);
                EEPROM.write(2, romIdx);  // EEPROM(2) means the length of valid values include (0~2)
                EEPROM.commit();
                
                Serial.println("Saving complete");
            
            }
            // play withdout saving
            else {
            //Serial.printf("input line : %s\n", line);
                Serial.println("Playing...");
                for (int i = 0; i + 1 < len; i+=2) {
                    delay(10);
                    int noteVal, dutyVal = 128, durVal = ctoi(line[i+1]);
                    
                    if (line[i] == ',') { noteVal = 0; dutyVal = 0; }
                    else { noteVal = ctoi(line[i]);}

                    playNote(noteVal, durVal, dutyVal);
                }
            }

            
        } // end of input mode check
    }

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