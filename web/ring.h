
const int ledChannel = 0;
const int resolution = 8;
const int buzPin = 22;

char sInput;

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

void playNote(int note, int dur) {
    // not a code for user
    if (dur == -1) {
      ledcSetup(ledChannel, 0, resolution);
      ledcWrite(ledChannel, 0);
      delay(1);
      return;
    }

    int duty = 128;
    // means ','
    if (note == -1) {
      duty = 0;
    }
    
    ledcSetup(ledChannel, nFrq[note], resolution);
    ledcWrite(ledChannel, duty);
    //Serial.println(duty == 0 ? -1 : note);
    
    //Serial.printf("%d %d %d\n", note, dur, duty);
    delay(nDur[dur]);
}
