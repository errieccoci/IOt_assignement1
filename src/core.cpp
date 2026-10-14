#include "core.h"
#include "Arduino.h"
#include "kernel.h"
#include "input.h"
#include "output.h"

#define MAX_TIME_IN_INTRO_STATE 15000
#define MAX_TIME_IN_STAGE2_STATE 10000
#define MAX_TIME_WAIT_BUTTON 3000
#define MAX_TIME_IN_STAGE1_STATE 3000
#define MAX_TIME_IN_FINAL_STAGE 5000

String welcome = "Welcome to the React! React. Press Buttons to Start";
int curr_random;
uint8_t current_round = 1;
/* core business logic */

void initCore(){
  Serial.begin(9600);
}

void intro(){
  if (isJustEnteredInState()){
    Serial.println("Intro...");
  }
  initLcd();
  digitalWrite(LED_GREEN01, LOW);
  digitalWrite(LED_GREEN02, LOW);
  int c = 0;
  bool b = true;
  while((!isButtonPressed(0) or !isButtonPressed(1)) and getCurrentTimeInState() < MAX_TIME_IN_INTRO_STATE) {
    // lampeggia i led rossi
    scroll(&c, &b, welcome);
    updateStateTime();
    delay(300);
  }
  resetLcd();
  updateStateTime();
  if (getCurrentTimeInState() >= MAX_TIME_IN_INTRO_STATE){
    changeState(FINAL_STATE);
  } else {
    changeState(STAGE1_STATE);
    resetInput();
  }
}

void stage1(){
  const int t_max = MAX_TIME_IN_STAGE1_STATE / (readPotLevel()*current_round);
  /*
  Cosa fa:
  1) Viene generato il numero casuale
  2) Viene fatto il check dei bottoni
  */
  stage1_restart:
  /*
  Se uno dei due bottoni viene premuto prima che possa partire 
  il gioco, allora viene dato errore per quel player, 
  decrementato il suo score e ripartito stage 1
  */
  bool restart = false;
  for(uint8_t i = 0; i < NUM_BUTTONS; i++) {
    if (isButtonPressed(i)) {
      signPlayerErrorScore(i);
      resetInput();
      Serial.println("STAGE1 button is pressed" + String(i));
      restart = true;
    }
  }
  if (restart) goto stage1_restart;
  if (isJustEnteredInState()){
    Serial.println("Stage1...");
    resetInput();
    curr_random = (int)random(0, 3);
    resetLcd();
    printLcd(curr_random);
  }
  updateStateTime();

  while(!isButtonPressed(0) and !isButtonPressed(1) and 
    getCurrentTimeInState() < t_max) {
      updateStateTime();
  }
  updateStateTime();
  Serial.println(getCurrentTimeInState() > t_max);
  if (getCurrentTimeInState() >= t_max){
    changeState(FINAL_STATE);
  } else {
    changeState(STAGE2_STATE);
  }
}

void stage2(){
  const long ts = millis();
  uint8_t p = 0;
  bool p_correct[NUM_BUTTONS] = {true, true};

  if (isJustEnteredInState()){
    Serial.println("Stage2...");
  }
  
  /* Se viene superato questo controllo, 
  vuol dire che hanno smesso di premere */
  if (ts - getLastButtonTime(0) < MAX_TIME_WAIT_BUTTON or 
    ts - getLastButtonTime(1) < MAX_TIME_WAIT_BUTTON) {
      return;
  }

  /*
  Per trovare quale bottone ha finito per primo
  e incrementare il suo score in caso abbia successo
  */
  p = getLastButtonTime(0) > getLastButtonTime(1);
  if (getbuttonScore(p) == curr_random) {
    incPlayerScore(p);
  } else if (getbuttonScore(!p) == curr_random) {
    incPlayerScore(!p);
  }

  /*
  Per coloro che hanno sbagliato, viene segnalato
  errore e decrementato lo score
  */
  for(uint8_t i = 0; i<NUM_BUTTONS; i++) {
   if (isButtonPressed(i) != curr_random) {
    signPlayerErrorScore(i);
    p_correct[i] = false;
  }
  }

  printLcd("P1 " + String(getbuttonScore(0)), 
  "P2 " + String(getbuttonScore(1)));
  calcBlink(p_correct[0], p_correct[1]);
  /* change the state if button 1 is pressed or max time elapsed*/
  updateStateTime();

  if (getCurrentTimeInState() > MAX_TIME_IN_STAGE2_STATE / (readPotLevel()*current_round)){
    changeState(FINAL_STATE);          
  }
  current_round++;
  changeState(STAGE1_STATE);
}


void finalize(){
  if (isJustEnteredInState()){
    String final_string;
    int c = 0;
    bool b = true;
    Serial.println("Finalize...");
    if (getbuttonScore(0) == getbuttonScore(1)) {
      final_string = "Game over - Tie.";
    } else {
      final_string = "Game over - Player " + String(getbuttonScore(0) > getbuttonScore(1) ? "P1" : "P2") + " win.";
    }
    Serial.println(getCurrentTimeInState());
    while(getCurrentTimeInState() < MAX_TIME_IN_FINAL_STAGE) {
      updateStateTime();
      scroll(&c, &b, final_string);
      delay(300);
    }
  }
  changeState(INTRO_STATE);
}
