// IoT07-1 ESP32 WebServer
#include <EEPROM.h>
#define EEPROM_SIZE 1024
#define SWAP 0 // sw access point
// Load Wi-Fi library 
#include <WiFi.h>
#include "time.h"

const int ledChannel = 0; 
const int resolution = 8; 
const int buzPin = 23; 
const int duty = 128;

int vNote=0, vDur=0;
//String vvNote;
const int dDur = 250; // default duration

int nFrq[] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523}; 
int nDur[] = { 2000, 1500, 1000, 750, 500, 375, 250 };
bool savePlay = false;

// Replace with your network credentials 
#if SWAP
const char* ssid = "ESP32-AP";
const char* password = "123456789"; 
#else
//const char* ssid = "KAU-Guest"; 
//const char* password = ""; 
const char* ssid = "-"; 
const char* password = "-"; 
#endif
// Set web server port number to 80 
WiFiServer server(80);
// password should be long!!
// Variable to store the HTTP request
String header;
// Auxiliar variables to store the current output state
String output16State = "off";
String output17State = "off";
// Assign output variables to GPIO pins
const int output16 = 16;
const int output17 = 17;
int val_Pairs = 0;
int val_ndPairs = 0;
int count = 3;



const char* ntpServer = "pool.ntp.org"; 

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s) 
const long timeoutTime = 2000;
const long gmtOffset_sec = 3600*9;  //3600
const int daylightOffset_sec = 0; // 3600
String tInfo = "n";

void playNote(int note, int dur) { 
  if (note <= -1 || note >=20 ) {
    ledcSetup(ledChannel, 0, resolution);
    ledcWrite(ledChannel, 0);
    dur -= 2;
  }
  else {
    ledcSetup(ledChannel, nFrq[note], resolution);
    ledcWrite(ledChannel, duty);
  }
  Serial.println(String(note)+","+String(dur));
  delay(nDur[dur]); 
}

void printLocalTime()
{ 
  WiFiClient client = server.available(); // Listen for incoming clients

  struct tm timeinfo; 
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time"); 
    return;
  } 
  client.println("<script>var totalTime="+String(timeinfo.tm_hour*3600+timeinfo.tm_min*60+timeinfo.tm_sec)+
   "; setInterval(function(){totalTime++; document.getElementById('timer').innerHTML='NowTime: '+Math.floor(totalTime/3600) + ':' + Math.floor(totalTime%3600/60) + ':' + totalTime%3600%60;}, 1000);");
  client.println("var myAl = '-'; </script>"); 
  client.println(&timeinfo, "<h2 id='timer'>NowTime: %H:%M:%S</h2>");
  client.println("Year: "+String(timeinfo.tm_year+1900)+", Month: " + String(timeinfo.tm_mon+1));
  //client.println("var x;")
            
  
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  //Serial.println("Year: " + String(timeinfo.tm_year+1900) + ", Month: " + String(timeinfo.tm_mon+1));    
  //tInfo = String(timeinfo.tm_year+1900)+":"+String(timeinfo.tm_mon+1)+":"+String(timeinfo.tm_mday)+":"+String(timeinfo.tm_hour)+":"+String(timeinfo.tm_min)+":"+String(timeinfo.tm_sec);
}


void setup() {
  Serial.begin(115200);

  Serial.printf("Connecting to %s ", ssid); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  } 
  Serial.println(" CONNECTED");
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); 
  printLocalTime();
  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  
  // Initialize the output variables as outputs
  pinMode(output16, OUTPUT);
  pinMode(output17, OUTPUT);
  
  digitalWrite(output16, LOW);
  digitalWrite(output17, LOW);
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

void loop(){
  WiFiClient client = server.available(); // Listen for incoming clients
  if (client) {   // If a new client connects,
    struct tm timeinfo; 
    if(!getLocalTime(&timeinfo)){
      Serial.println("Failed to obtain time"); 
      return;
    }
    
    currentTime = millis();
    previousTime = millis();
    //printLocalTime();
    //struct tm timeinfo; 


    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) { 
      // loop while the client's connected
      currentTime = millis();
      if (client.available()) { // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c); // print it out the serial monitor
        header += c;
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
            // turns the GPIOs on and off
            if (header.indexOf("GET /16/on") >= 0) {
            Serial.println("GPIO 16 on"); 
            output16State = "on"; 
            digitalWrite(output16, HIGH);
            } 
            else if (header.indexOf("GET /16/off") >= 0) { 
              Serial.println("GPIO 16 off");
              output16State = "off"; 
              digitalWrite(output16, LOW);
            }
            else if (header.indexOf("GET /17/on") >= 0) { 
              Serial.println("GPIO 17 on");
              output17State = "on"; 
              digitalWrite(output17, HIGH);
            }
            else if (header.indexOf("GET /17/off") >=0) {
              Serial.println("GPIO 17 off");
              output17State = "off";
              digitalWrite(output17, LOW);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"); 
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences 
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"); 
            client.println(".button { background-color: #4CAF50;border: none; color: white; padding: 16px 40px;"); 
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            

            client.println(&timeinfo, "<h2 id='timer'>NowTime: %H:%M:%S</h2>");
            //client.println("Year: "+String(timeinfo.tm_year+1900)+", Month: " + String(timeinfo.tm_mon+1));
            
            Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
            // Display current state, and ON/OFF buttons for GPIO 16
            // input time 
            client.println("<input type='time' id='myAlarm' name='appti'min='09:00' max='18:00' required>");
            //입력받은 값 저장버튼
            client.println("<button onclick='myfunction()'>save</button>");
            client.println("<p id='input_alarm'>--:--</p>");
            // ____시간 JS 입력
            client.println("<script>var totalTime="+String(timeinfo.tm_hour*3600+timeinfo.tm_min*60+timeinfo.tm_sec)+
             "; setInterval(function(){totalTime++; document.getElementById('timer').innerHTML='NowTime: '+Math.floor(totalTime/3600) + ':' + Math.floor(totalTime%3600/60) + ':' + totalTime%3600%60;}, 1000);");            
            
            client.println("function myfunction() {");
            client.println("myAl = document.getElementById('myAlarm').value;");
            client.println("alert('Alarm saved!'); ");
            client.println("input_alarm.innerHTML = myAl }");
            client.println("if(myAl == totalTime) {");
            client.println("alert('Alarm!');");
            client.println("} </script>");

            
            client.println("<p>GPIO 16 - State " + output16State + "</p>");
            //____버튼
            // If the output16State is off, it displays the ON button
            if (output16State=="off") {
              client.println("<p><a href=\"/16/on\"><button class=\"button\">ON</button></a></p>"); 
            } 
            else {
              client.println("<p><a href=\"/16/off\"><button class=\"button button2\">OFF</button></a></p>"); 
            }
            // Display current state, and ON/OFF buttons for GPIO 17 
            client.println("<p>GPIO 17 - State " + output17State + "</p>"); 
            // If the output17State is off, it displays the ON button
            if (output17State=="off") {
              client.println("<p><a href=\"/17/on\"><button class=\"button\">ON</button></a></p>"); 
            } 
            else {
              client.println("<p><a href=\"/17/off\"><button class=\"button button2\">OFF</button></a></p>"); 
            }
            
            
            
            client.println("</body></html>");
            //The HTTP reponse ends with another blank line
            client.println();
            //Break out of the while loop
            break;
          } //** if (currentLine.length() == 0) {
          else{ // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } //** if (c == '\n'){
        else if (c != '\r') { // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine 
        }
      } //* if (client.available()) { 
    } //** while

    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected."); 
    Serial.println("");
  } //** if (client) {
  //음악 플레이
  if(savePlay){
    Serial.println("__________saved music play__________");
    val_Pairs = EEPROM.read(2);
    Serial.print("val_Pairs :");
    Serial.println(val_Pairs);

    for (int i=0; i<val_Pairs; i++){
      val_ndPairs = EEPROM.read(count);
      count += 1;  
      
      Serial.print("val_ndPairs :");
      Serial.println(val_ndPairs);
      if (val_ndPairs < 10){
        vNote = 0;
        vDur = val_ndPairs;
      }
      else if (val_ndPairs < 100){
        vNote = val_ndPairs/10;
        vDur = val_ndPairs%10;
      }
      else if (val_ndPairs >= 100){
        vNote = val_ndPairs/10;
        //val_ndPairs = val_ndPairs%100;
        vDur = val_ndPairs%10;
      }
      playNote(vNote, vDur);
    }
    savePlay = false;
  } 
} //** loop() {
  
