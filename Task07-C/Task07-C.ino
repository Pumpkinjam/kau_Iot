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

                            checkHour();
                            checkMinute();
                            delay(10);
                            Serial.printf("%d : %d\n", alarmHour, alarmMinute);
                        }
                        delay(10);
                        printPage(&client);
                        
                        client.println(R"rawliteral(
                            <script>
                                document.getElementById("alarmDisp").innerHTML= document.getElementById("Hour").value + ' : ' + document.getElementById("Minute").value;
                            </script>
                            <h2 id='alarmDisp'></h2>
                            )rawliteral");
                        client.printf("<h2>Alarm was set to %d : %d</h2>\n", alarmHour, alarmMinute);
                        
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


// hard coding :(
void checkHour() {
    if (header.indexOf("/h0") >= 0) { 
        alarmHour = 0; 
    }
    else if (header.indexOf("/h1") >= 1) { alarmHour = 1; }
    else if (header.indexOf("/h2") >= 1) { alarmHour = 2; }
    else if (header.indexOf("/h3") >= 1) { alarmHour = 3; }
    else if (header.indexOf("/h4") >= 1) { alarmHour = 4; }
    else if (header.indexOf("/h5") >= 1) { alarmHour = 5; }
    else if (header.indexOf("/h6") >= 1) { alarmHour = 6; }
    else if (header.indexOf("/h7") >= 1) { alarmHour = 7; }
    else if (header.indexOf("/h8") >= 1) { alarmHour = 8; }
    else if (header.indexOf("/h9") >= 1) { alarmHour = 9; }
    else if (header.indexOf("/h10") >= 0) { alarmHour = 10; }
    else if (header.indexOf("/h11") >= 1) { alarmHour = 11; }
    else if (header.indexOf("/h12") >= 1) { alarmHour = 12; }
    else if (header.indexOf("/h13") >= 1) { alarmHour = 13; }
    else if (header.indexOf("/h14") >= 1) { alarmHour = 14; }
    else if (header.indexOf("/h15") >= 1) { alarmHour = 15; }
    else if (header.indexOf("/h16") >= 1) { alarmHour = 16; }
    else if (header.indexOf("/h17") >= 1) { alarmHour = 17; }
    else if (header.indexOf("/h18") >= 1) { alarmHour = 18; }
    else if (header.indexOf("/h19") >= 1) { alarmHour = 19; }
    else if (header.indexOf("/h20") >= 0) { alarmHour = 20; }
    else if (header.indexOf("/h21") >= 1) { alarmHour = 21; }
    else if (header.indexOf("/h22") >= 1) { alarmHour = 22; }
    else if (header.indexOf("/h23") >= 1) { alarmHour = 23; }
    else { Serial.println("Where's my hour?"); return; }
    //Serial.printf("checkHour complete - %d\n", alarmHour);
}
// that's worse X(
void checkMinute() {
    if (header.indexOf("/m0") >= 0) {
        alarmMinute = 0;
        Serial.println(0);
    }
    else if (header.indexOf("/m1") >= 0) { alarmMinute = 1; }
    else if (header.indexOf("/m2") >= 0) { alarmMinute = 2; }
    else if (header.indexOf("/m3") >= 0) { alarmMinute = 3; }
    else if (header.indexOf("/m4") >= 0) { alarmMinute = 4; }
    else if (header.indexOf("/m5") >= 0) { alarmMinute = 5; }
    else if (header.indexOf("/m6") >= 0) { alarmMinute = 6; }
    else if (header.indexOf("/m7") >= 0) { alarmMinute = 7; }
    else if (header.indexOf("/m8") >= 0) { alarmMinute = 8; }
    else if (header.indexOf("/m9") >= 0) { alarmMinute = 9; }
    else if (header.indexOf("/m10") >= 0) { alarmMinute = 10; }
    else if (header.indexOf("/m11") >= 0) { alarmMinute = 11; }
    else if (header.indexOf("/m12") >= 0) { alarmMinute = 12; }
    else if (header.indexOf("/m13") >= 0) { alarmMinute = 13; }
    else if (header.indexOf("/m14") >= 0) { alarmMinute = 14; }
    else if (header.indexOf("/m15") >= 0) { alarmMinute = 15; }
    else if (header.indexOf("/m16") >= 0) { alarmMinute = 16; }
    else if (header.indexOf("/m17") >= 0) { alarmMinute = 17; }
    else if (header.indexOf("/m18") >= 0) { alarmMinute = 18; }
    else if (header.indexOf("/m19") >= 0) { alarmMinute = 19; }
    else if (header.indexOf("/m20") >= 0) { alarmMinute = 20; }
    else if (header.indexOf("/m21") >= 0) { alarmMinute = 21; }
    else if (header.indexOf("/m22") >= 0) { alarmMinute = 22; }
    else if (header.indexOf("/m23") >= 0) { alarmMinute = 23; }
    else if (header.indexOf("/m24") >= 0) { alarmMinute = 24; }
    else if (header.indexOf("/m25") >= 0) { alarmMinute = 25; }
    else if (header.indexOf("/m26") >= 0) { alarmMinute = 26; }
    else if (header.indexOf("/m27") >= 0) { alarmMinute = 27; }
    else if (header.indexOf("/m28") >= 0) { alarmMinute = 28; }
    else if (header.indexOf("/m29") >= 0) { alarmMinute = 29; }
    else if (header.indexOf("/m30") >= 0) { alarmMinute = 30; }
    else if (header.indexOf("/m31") >= 0) { alarmMinute = 31; }
    else if (header.indexOf("/m32") >= 0) { alarmMinute = 32; }
    else if (header.indexOf("/m33") >= 0) { alarmMinute = 33; }
    else if (header.indexOf("/m34") >= 0) { alarmMinute = 34; }
    else if (header.indexOf("/m35") >= 0) { alarmMinute = 35; }
    else if (header.indexOf("/m36") >= 0) { alarmMinute = 36; }
    else if (header.indexOf("/m37") >= 0) { alarmMinute = 37; }
    else if (header.indexOf("/m38") >= 0) { alarmMinute = 38; }
    else if (header.indexOf("/m39") >= 0) { alarmMinute = 39; }
    else if (header.indexOf("/m40") >= 0) { alarmMinute = 40; }
    else if (header.indexOf("/m41") >= 0) { alarmMinute = 41; }
    else if (header.indexOf("/m42") >= 0) { alarmMinute = 42; }
    else if (header.indexOf("/m43") >= 0) { alarmMinute = 43; }
    else if (header.indexOf("/m44") >= 0) { alarmMinute = 44; }
    else if (header.indexOf("/m45") >= 0) { alarmMinute = 45; }
    else if (header.indexOf("/m46") >= 0) { alarmMinute = 46; }
    else if (header.indexOf("/m47") >= 0) { alarmMinute = 47; }
    else if (header.indexOf("/m48") >= 0) { alarmMinute = 48; }
    else if (header.indexOf("/m49") >= 0) { alarmMinute = 49; }
    else if (header.indexOf("/m50") >= 0) { alarmMinute = 50; }
    else if (header.indexOf("/m51") >= 0) { alarmMinute = 51; }
    else if (header.indexOf("/m52") >= 0) { alarmMinute = 52; }
    else if (header.indexOf("/m53") >= 0) { alarmMinute = 53; }
    else if (header.indexOf("/m54") >= 0) { alarmMinute = 54; }
    else if (header.indexOf("/m55") >= 0) { alarmMinute = 55; }
    else if (header.indexOf("/m56") >= 0) { alarmMinute = 56; }
    else if (header.indexOf("/m57") >= 0) { alarmMinute = 57; }
    else if (header.indexOf("/m58") >= 0) { alarmMinute = 58; }
    else if (header.indexOf("/m59") >= 0) { alarmMinute = 59; }
    else { Serial.println("Where's my minute????"); return; }
    //Serial.printf("checkMinute complete - %d\n", alarmMinute);
}