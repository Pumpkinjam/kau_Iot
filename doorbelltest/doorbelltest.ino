

const int buzPin = 23;
const int buttonPin = 15;
const int ledChannel = 0;
const long intMil = 1000;
unsigned long preMil = 0;
const int resolution = 8;
const int duty = 128;
void ringBell(int dur){
  Serial.println("ring");
  ledcSetup(ledChannel,466,resolution);
  ledcWrite(ledChannel,duty);
  delay(dur);
  ledcSetup(ledChannel,370,resolution);
  ledcWrite(ledChannel,duty);
  delay(dur);
  ledcWrite(ledChannel,0);
  
}

void buttonPush(){
  Serial.println("Button pushed");
  ringBell(700);
  
}

void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
   ledcAttachPin(buzPin, ledChannel);
   pinMode(buttonPin,INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  if((millis()-preMil)>intMil){
    if(digitalRead(buttonPin)){
      preMil=millis();
      buttonPush();
    }
  }
  

}
