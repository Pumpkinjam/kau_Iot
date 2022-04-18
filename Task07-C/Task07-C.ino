#include "printPage.h"

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