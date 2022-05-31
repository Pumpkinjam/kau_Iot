#include "secret.h"
#include "connection_data.h"
#include "WiFiData.h"
#include <AWS_IOT.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include <Wire.h>

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600*9; // 3600
const int daylightOffset_sec = 0; // 3600
WiFiServer server(80);
String header;

char sTOPIC_NAME[] = "$aws/things/ESP32_Doorlock/shadow/update/delta";
char pTOPIC_NAME[] = "web/publish";
char sTOPIC_ACTION[] = "web/action"
/* Data below are defined in 'WiFiData.h' and 'connection_data.h' 
 * 
 * const char* ssid = WIFI_SSID;
 * const char* password = WIFI_PW;
 * char HOST_ADDRESS[] = AWS_DEVICE_DATA_ENDPOINT;
 * char CLIENT_ID[] = CLIENT_ID;
 * 
 * these are not supposed to be uploaded to github
*/

AWS_IOT hornbill;

int status = WL_IDLE_STATUS;
int msgCount = 0, msgReceived = 0;
char payload[512];
char rcvdPayload[512];
char cmd[20];

void mySubCallBackHandler(char* topicName, int payloadLen, char* payLoad) {
  // set rcvdPayload(recieved payload) to payload
  strncpy(rcvdPayload, payLoad, payloadLen);
  // add null character
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

String parsePw(String str) {
    int idx_start = str.indexOf("newpw=") + 6;
    int idx_end = str.indexOf("&");
    return str.substring(idx_start, idx_end);
}

String parseTime(String str) {
    int idx_start = str.indexOf("time=") + 5;
    int idx_end = idx_start + 12;
    return str.substring(idx_start, idx_end);
}

String parseUntil(String str) {
    int idx_start = str.indexOf("until=") + 6;
    if (str.charAt(idx_start) == '*') { return String("*"); }
    
    int idx_end = idx_start + 12;
    return str.substring(idx_start, idx_end);
}

void setup() {
    Serial.begin(115200);

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
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    if (hornbill.connect(HOST_ADDRESS, CLIENT_ID) == 0) {
        Serial.println("Connected to AWS");
        delay(1000);

        if (0 == hornbill.subscribe(sTOPIC_ACTION, mySubCallBackHandler)) {
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

    server.begin();
}


void loop(){
    if (msgReceived == 1) {
        msgReceived = 0;  // Semaphore needed if it's multiprocessor
        Serial.print("Received Message: ");
        Serial.println(rcvdPayload);

        // Parse JSON
        JSONVar myObj = JSON.parse(rcvdPayload);    // myObj has { "command" : "asdf" }
        cmd = myObj["command"]
    }

    WiFiClient client = server.available(); // Listen for incoming clients
    if (client) { // If a new client connects,

        Serial.println("New Client."); // print a message out in the serial port
        String currentLine = ""; // make a String to hold incoming data from the client
        while (client.connected()) { // loop while the client's connected

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
                        
                        if (header.indexOf("GET /manage") >= 0) {
                            client.println(manage_html);  
                        }
                        else if (header.indexOf("GET /manage2") >= 0) {
                            client.println(manage2_html);
                        }
                        else if (header.indexOf("GET /lcd") >= 0) {
                            client.println(lcd_html);  
                        }
                        // until=* for changing password permanently
                        // GET /?newpw=********&time=yyyymmddhhmm&until=yyyymmddhhmm HTTP/1.1
                        else if (header.indexOf("GET /?newpw") >= 0) {
                            JSONVar pwSettingValues;
                            pwSettingValues["newpw"] = parsePw(header);
                            pwSettingValues["time"] = parseTime(header);
                            pwSettingValues["until"] = parseUntil(header);

                            JSON.stringify(state).toCharArray(payload, 512);

                            Serial.println(payload);
                            
                            if (hornbill.publish("doorlock/setpw", payload) == 0) {
                                Serial.print("Published : ");
                                Serial.println(payload);
                            }
                            else {Serial.println("Publish failed. My heart really breaks.");}

                            client.println(manage_html);
                        }
                        else {
                            client.println(login_html);
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
