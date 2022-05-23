#include "secret.h"
#include "connection_data.h"
#include "WiFiData.h"
#include <AWS_IOT.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

char HOST_ADDRESS[] = "acj2gilk7nyok-ats.iot.ap-northeast-2.amazonaws.com";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600*9; // 3600
const int daylightOffset_sec = 0; // 3600
WiFiServer server(80);
String header;

/* Data below are defined in 'connection_data.h'
 * 
 * const char* ssid = WIFI_SSID;
 * const char* password = WIFI_PW;
 * char HOST_ADDRESS[] = AWS_DEVICE_DATA_ENDPOINT;
 * char CLIENT_ID[] = CLIENT_ID;
 * char sTOPIC_NAME[] = "$aws/things/<Thing_Name>/shadow/update/delta";
 * char pTOPIC_NAME[] = "$aws/things/<Thing_Name>/shadow/update";
 * 
 * these are not supposed to be uploaded to github
*/

AWS_IOT hornbill;

int status = WL_IDLE_STATUS;
int msgCount = 0, msgReceived = 0;
char payload[512];
char rcvdPayload[512];

void mySubCallBackHandler(char* topicName, int payloadLen, char* payLoad) {
  // set rcvdPayload(recieved payload) to payload
  strncpy(rcvdPayload, payLoad, payloadLen);
  // add null character
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

String parseId(String str) {
    int idx_start = str.indexOf("loginid=") + 8;
    int idx_end = str.indexOf("&");
    return str.substring(idx_start, idx_end);
}

String parsePw(String str) {
    int idx_start = str.indexOf("loginpw=") + 8;
    int idx_end = str.indexOf(" HTTP") - 1;
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

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    server.begin();
}


void loop(){
    // when recieved message from subscribed topic
    // Maybe later, if we need this
    /*
    if (msgReceived == 1) {
        msgReceived = 0;  // Semaphore needed if it's multiprocessor
        Serial.print("Received Message: ");
        Serial.println(rcvdPayload);

        // Parse JSON
        JSONVar myObj = JSON.parse(rcvdPayload);    // myObj has { ~~ "state" : { ~~ } }
    }
    */
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
                            printManagePage(&client);   
                        }
                        else if (header.indexOf("GET /lcd") >= 0) {
                            printLcdPage(&client);   
                        }
                        else {
                            printLoginPage(&client);
                        }

                        // GET /?loginid=admin&loginpw=alpine HTTP/1.1
                        if (header.indexOf("GET /?loginid") >= 0) {
                            JSONVar loginValues;
                            loginValues["id"] = parseId(header);
                            loginValues["password"] = parsePw(header);

                            JSONVar reported;
                            reported["reported"] = loginValues;
                            JSONVar state;
                            state["state"] = reported;

                            JSON.stringify(state).toCharArray(payload, 512);

                            if (hornbill.publish(pTOPIC_NAME, payload) == 0) {
                                Serial.print("Login Value Published : ");
                                Serial.println(payload);
                            }
                            else {Serial.println("Login Value publish failed. My heart really breaks.");}
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
