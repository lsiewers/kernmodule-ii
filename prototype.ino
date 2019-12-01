//
// matrix
//
#include "LedControl.h"

LedControl lc=LedControl(12,11,10,4);

float screenBrightness = 8;

//
// rgb led
//
int rPin = 3;
int gPin = 5;
int bPin = 6;
float rgbValue[3] = {0.0, 0.0, 0.0};
boolean fadeIn = false;

//
// light sensor (LDR)
//
int ldrPin = A3;
int ldrValue = 0;

//
// states
//
boolean pulledUp = false; // if there is too much light
boolean sleeping = false;
int pulledUpTimer = 0;
int pulledDownTimer = 0;

void setup() {
  Serial.begin(9600);

  // matrix
  matrixInit();

  // rgb
  rgbInit();
  //  rgbOrange();

  // LDR
  pinMode(ldrPin, INPUT);
}

void loop() { 
  // LDR
  readLDR();

  // RGB led
  if(sleeping) { rgbSleeping(); }
  else { rgbOrange(); }

  delay(40); // small delay gives a more natural feel
}


//
// matrix function 
//

void matrixInit() {
  //we have already set the number of devices when we created the LedControl
  int devices=lc.getDeviceCount();
  lc.shutdown(0,true);
  lc.shutdown(3,true);
  
  /*The MAX72XX is in power-saving mode on startup*/
  lc.shutdown(1,false);
  lc.shutdown(2,false);
}

void setSmiley() {
  /* Set the brightness to a medium values */
  lc.setIntensity(1, screenBrightness);
  lc.setIntensity(2, screenBrightness);

  // the smiley is centered, and therefore split in 2 screens
  // mouth
  lc.setRow(2, 1, B00000011);
  lc.setRow(1, 1, B11000000);

  // corners
  lc.setLed(2, 2, 5, true);
  lc.setLed(1, 2, 2, true);
  lc.setLed(2, 3, 4, true);
  lc.setLed(1, 3, 3, true);
  
  // eyes
  lc.setLed(1, 6, 1, true);
  lc.setLed(2, 6, 6, true);
}

void clearMatrix() {
  lc.clearDisplay(1);
  lc.clearDisplay(2);
}

//
// rgb functions
//

void rgbInit() {
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);

  analogWrite(rPin, 0);
  analogWrite(gPin, 0);
  analogWrite(bPin, 0);
}

void rgbOrange() {
  float fadeSpeed = 0.5;

  // increase Red value if not 255
  if(rgbValue[0] < 255) {
     rgbValue[0] += fadeSpeed;
    analogWrite(rPin, rgbValue[0]);
  }

  // change Green value if not 7
  if(rgbValue[1] < 7) {
    rgbValue[1] += fadeSpeed/2;
  } else if (rgbValue > 7) {
    rgbValue[1] -= fadeSpeed;
  }
  analogWrite(gPin, rgbValue[1]);
}

void rgbSleeping() {
  float fadeSpeed = 1;

  // breath-like fading
  // if fadeIn, increase values
  // else decrease
  if(fadeIn) {
//      Serial.println(rgbValue[0]);
    if(rgbValue[0] <= 255) {
      rgbValue[0] += fadeSpeed*2;
      analogWrite(rPin, rgbValue[0]);
    }
    if(rgbValue[1] <= 180.0) {
      rgbValue[1] += fadeSpeed*1.75;
      analogWrite(gPin, rgbValue[1]);
    }
    if(rgbValue[2] <= 255.0) {
      rgbValue[2] += fadeSpeed*2;
      analogWrite(bPin, rgbValue[2]);
    } 
    if(rgbValue[0] >= 255.0 && rgbValue[1] >= 180.0 && rgbValue[2] >= 255.0 ) {
      fadeIn = false;
    }
  } else {
    if(rgbValue[0] > 0.0) {
      rgbValue[0] -= fadeSpeed;
      analogWrite(rPin, rgbValue[0]);
    }
    if(rgbValue[1] > 0.0) {
      rgbValue[1] -= fadeSpeed*0.75;
      analogWrite(gPin, rgbValue[1]);
    }
    if(rgbValue[2] > 0.0) {
      rgbValue[2] -= fadeSpeed;
      analogWrite(bPin, rgbValue[2]);
    }
    if(rgbValue[0] <= 0 && rgbValue[1] <= 0 && rgbValue[2] <= 0) {
      fadeIn = true;
    }
  }
}

//
// light sensor functions
//
void readLDR() {
  ldrValue = analogRead(ldrPin);
//  Serial.println(ldrValue);

  // if too light, it is considered pulled up
  if(ldrValue > 30 && !pulledUp) {
    // if too light, set timer for small delay, so it feels more natural
    if(pulledUpTimer == 0) { pulledUpTimer = millis(); }
//    Serial.println(pulledUpTimer);
    if(millis() - pulledUpTimer > 1000) {
      pulledUp = true; 
      pulledUpTimer = 0;  
      setSmiley();
    } // add little delay to smile
    if(sleeping) { sleeping = false; } 
    
  }
  // if too dark for at least 3 seconds, fall asleep
  if (ldrValue <= 30 && pulledUp) {
    if(pulledDownTimer == 0) { pulledDownTimer = millis(); }
    if(millis() - pulledDownTimer > 3000) {
      pulledUp = false;
      sleeping = true;
      pulledDownTimer = 0;
      clearMatrix();
    }
    
  }
}
