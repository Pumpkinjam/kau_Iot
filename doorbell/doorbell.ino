#include "esp_http_client.h"

#include <WiFi.h>
#include <base64.h>
#include <esp_wifi.h>

#include "OV7670.h"

const char* ssid = "12345";
const char* password = "hwang1151";
char HOST_ADDRESS[] = "acj2gilk7nyok-ats.iot.ap-northeast-2.amazonaws.com";
char CLIENT_ID[] = "KAU_DOORBELL";
char pTOPIC_NAME[]= "esp32/button";   // publish topic name
int status = WL_IDLE_STATUS;
char payload[512];
const int buzPin = 23;
const int buttonPin = 36;
const int ledChannel = 8;
const long intMil = 1000;
const long sleepMil = 10000;
unsigned long preMil = 0;
const int resolution = 8;
const int duty = 128;

const int SIOD = 21; //SDA
const int SIOC = 22; //SCL

const int VSYNC = 34;
const int HREF = 35;

const int XCLK = 32;
const int PCLK = 33;

const int D0 = 27;
const int D1 = 17;
const int D2 = 16;
const int D3 = 15;
const int D4 = 14;
const int D5 = 13;
const int D6 = 12;
const int D7 = 4;

OV7670 *camera;

String httpreturn;

String encodedImage;
int count = 0;


void wifiConnect(){
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());//++choi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to wifi");
}
/*
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}
*/
void ringBell(int dur){
  Serial.println("ring");
  ledcSetup(ledChannel,932,resolution);
  ledcWrite(ledChannel,duty);
  delay(dur+100);
  ledcSetup(ledChannel,740,resolution);
  ledcWrite(ledChannel,duty);
  delay(dur);
  ledcWrite(ledChannel,0);
  
}


void postHTTP2(){
  esp_http_client_handle_t http_client;
  esp_http_client_config_t config_client = {0};
  config_client.url = "https://7u50sz27f6.execute-api.ap-northeast-2.amazonaws.com/IOT_LABDA2_REST/Image";
  config_client.event_handler = _http_event_handler;
  config_client.method = HTTP_METHOD_POST;
  http_client = esp_http_client_init(&config_client);
  config_client.event_handler = _http_event_handler;
  config_client.method = HTTP_METHOD_POST;

  http_client = esp_http_client_init(&config_client);

   esp_http_client_set_post_field(http_client,(base64::encode(camera->frame, camera->xres * camera->yres * 2)).c_str(), 51200);
 
  esp_http_client_set_header(http_client, "Content-Type", "image/bmp");

  esp_err_t err = esp_http_client_perform(http_client);
  if (err == ESP_OK) {
    Serial.print("esp_http_client_get_status_code: ");
    Serial.println(esp_http_client_get_status_code(http_client));
  }

  esp_http_client_cleanup(http_client);

}
void buttonPush(){
  Serial.println("Button pushed");
  camera->oneFrame();
  ringBell(700);
  
  postHTTP2();
  //Serial.println(base64::encode(camera->frame, camera->xres * camera->yres * 2));
  
  
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
  switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
      Serial.println("HTTP_EVENT_ERROR");
      break;
    case HTTP_EVENT_ON_CONNECTED:
      Serial.println("HTTP_EVENT_ON_CONNECTED");
      break;
    case HTTP_EVENT_HEADER_SENT:
      Serial.println("HTTP_EVENT_HEADER_SENT");
      break;
    case HTTP_EVENT_ON_HEADER:
      Serial.println();
      Serial.printf("HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
      break;
    case HTTP_EVENT_ON_DATA:
      Serial.println();
      Serial.printf("HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      if (!esp_http_client_is_chunked_response(evt->client)) {
        // Write out data
        count = evt->data_len;
        for(int i = 0; i<count;i++){
          httpreturn += ((char*)evt->data)[i];
        }
        //Serial.printf("%.*s\n", evt->data_len, (char*)evt->data);
      }
      break;
    case HTTP_EVENT_ON_FINISH:
      Serial.println(httpreturn);
      Serial.println(httpreturn.length());
      httpreturn="";
      Serial.println("");
      Serial.println("HTTP_EVENT_ON_FINISH");
      break;
    case HTTP_EVENT_DISCONNECTED:
      Serial.println("HTTP_EVENT_DISCONNECTED");
      break;
  }
  return ESP_OK;
}



void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
   camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
   //print_wakeup_reason();
   wifiConnect();
   ledcAttachPin(buzPin, ledChannel);
   pinMode(buttonPin,INPUT);
   //esp_sleep_enable_ext0_wakeup(GPIO_NUM_36,1);
   preMil = millis();
}

void loop() {
  //Serial.println(millis()-preMil);
  // put your main code here, to run repeatedly:
  if((millis()-preMil)>intMil){
    if(digitalRead(buttonPin)){
      preMil=millis();
      buttonPush();
    }
  }
  /*if((millis()-preMil)>sleepMil){
    Serial.println("Wifi_Disconnect");
     esp_wifi_stop();
     delay(100);

    
  }*/
}
/*
static esp_err_t event_handler(esp_http_client_event_t *evt){
  switch(evt->event_id){
    case SYSTEM_EVENT_STA_DISCONNECTED :
      Serial.println("Sleep");
      esp_deep_sleep_start();
      break;
  }
}
*/
