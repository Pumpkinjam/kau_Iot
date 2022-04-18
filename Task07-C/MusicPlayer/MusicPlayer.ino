#include <EEPROM.h>
#define EEPROM_SIZE 900
#define COMMAND_SIZE 300

const int ledChannel = 0;
const int resolution = 8;
const int buzPin = 22;

char sInput;

// Serial Moniter input string
char line[COMMAND_SIZE];

int nFrq[] = {
  //0
  262 /*C*/, 277, 
  //2
  294 /*D*/, 311, 
  //4
  330 /*E*/, 
  //5
  349 /*F*/, 370, 
  //7
  392 /*G*/, 415, 
  //9
  440 /*A*/, 466, 
  //b
  494 /*B*/, 
  //c
  523 /*C5*/, 554,
  //e
  587 /*D5*/, 622,
  //g
  659 /*E5*/,
  //h
  698 /*F5*/, 740,
  //j
  784 /*G5*/, 831,
  //l
  880 /*A5*/, 932,
  //n
  988 /*B5*/,
  //o
  1047 /*C6*/
};

int nDur[] = {2000, 1500, 1000, 750, 500, 375, 250};

// character to int ('0' ~ '9' and 'a' ~ 'z')
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

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  ledcAttachPin(buzPin, ledChannel);

  Serial.println("setup complete.");

  int asdf = 0;
  asdf = 1;
  //code for saving new score
  if (asdf && EEPROM.read(0) == 0xAA) {
    Serial.println("EEPROM set to 0");
    EEPROM.write(0, 0x00);
    EEPROM.write(1, 0x00);
    EEPROM.commit();
  }
}

void loop(){
  
  //Serial.println("Starting loop");
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
