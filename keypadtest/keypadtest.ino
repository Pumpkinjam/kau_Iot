#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};
byte rowPins[ROWS] = {21,19,18,5};
byte colPins[COLS] ={17,16,4,15};
//byte rowPins[ROWS] = {19,18,5,17};
//byte colPins[COLS] ={16,4,23,15};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
char keyPressed;
String doorPassward;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("start");
}

void loop() {
  // put your main code here, to run repeatedly:
  keyPressed = customKeypad.getKey();
  if (keyPressed != NO_KEY) {
    if(keyPressed == '*')
    Serial.println("passwward input start");
    while(1){
      keyPressed = customKeypad.getKey();
      if(keyPressed>='0' && keyPressed<='9'){
        Serial.print(keyPressed);
        doorPassward += keyPressed;
        delay(100);
      }
      if (keyPressed == '#'){
        Serial.print('\n');
        Serial.println(doorPassward);
        doorPassward="";
        break;
      }
    }
    
  }
  
      
}
