#include "secret.h"
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


void setup() {
    Serial.begin(115200);

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
