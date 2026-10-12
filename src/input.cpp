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
  playerScore[i]--;
}

void incPlayerScore(const int i) {
  playerScore[i]++;
}

void calcBlink(const bool p1, const bool p2) {
  /*
  Se p1 è true allora sceglierò di accendere il led green 
  di player1, altrimenti rosso, idem per p2. 
  Funzione utile nel momento in cui, alla fine dello stage2
  si vorrà segnalare con un blink per ciascun player se
  esso ha fatto un errore oppure ha vinto il round.
  */
  const bool pin_p1 = p1 ? LED_GREEN01 : LED_RED01;
  const bool pin_p2 = p2 ? LED_GREEN02 : LED_RED02;
  // blink totale di 2 secondi
  const int del_tot = 2000; 
  const int del_it = 300;
  const int it = del_tot / 2*del_it;

  for (uint8_t i = 0; i < it; i++) {
    digitalWrite(pin_p1, HIGH);
    digitalWrite(pin_p2, HIGH);
    delay(del_it);
    digitalWrite(pin_p1, LOW);
    digitalWrite(pin_p2, LOW);
    delay(del_it);
  }

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




