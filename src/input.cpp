#include "input.h"
#include "Arduino.h"
#include "config.h"

#include <EnableInterrupt.h>

#define BOUNCING_TIME 100

// #define __DEBUG__
uint8_t inputPins[NUM_BUTTONS] = {BUT01_PIN, BUT02_PIN};
uint8_t ledPins[NUM_LEDS] = {LED_RED01, LED_RED02, LED_GREEN01, LED_GREEN02};
bool buttonPressed[NUM_BUTTONS] = {false, false};
int buttonScore[NUM_BUTTONS] = {0,0};
uint16_t playerScore[NUM_BUTTONS] = {0,0}; 
/* used for debouncing */
long lastButtonPressedTimestamps[NUM_BUTTONS];

void buttonHandler(int i);
void buttonHandler0(){ buttonHandler(0); }
void buttonHandler1(){ buttonHandler(1); }

void (*buttonHandlers[NUM_BUTTONS])() = { buttonHandler0, buttonHandler1 };

int getbuttonScore(int i) {
  return buttonScore[i];
}
void buttonHandler(int i){
  long ts = millis();
  if (ts - lastButtonPressedTimestamps[i] > BOUNCING_TIME){
    lastButtonPressedTimestamps[i] = ts;
    int status = digitalRead(inputPins[i]);
    if (status == HIGH && !buttonPressed[i]) { 
        buttonPressed[i] = true;
        buttonScore[i]++;
    }
  }
}

long getLastButtonTime(const int i) {
  return lastButtonPressedTimestamps[i];
}
void initInput(){
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(inputPins[i], INPUT);  
    enableInterrupt(inputPins[i], buttonHandlers[i], CHANGE);       
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  playerScore[0] = 0;
  playerScore[1] = 0;
}

void signPlayerErrorScore(const int i) {
  const int p = i == 0 ? LED_RED01 : LED_RED02;
  playerScore[i]--;
  blink(p);
}

void blink(const int p) {
  digitalWrite(p, HIGH);
  delay(2000);
  digitalWrite(p, LOW);
}
void incPlayerScore(const int i) {
  const int p = i == 0 ? LED_GREEN01 : LED_GREEN02;
  playerScore[i]++;
  blink(p);
}

void resetInput(){
  long ts = millis();
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttonPressed[i] = false; 
    buttonScore[i] = 0;     
    lastButtonPressedTimestamps[i] = ts;    
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

bool isButtonPressed(int buttonIndex){
  return buttonPressed[buttonIndex];
}

bool hasBeenPressed(int index){
  return buttonPressed[index]; 
}




