#include "core.h"
#include "Arduino.h"
#include "kernel.h"
#include "input.h"
#include <LiquidCrystal_I2C.h>

#define MAX_TIME_IN_INTRO_STATE 5000
#define MAX_TIME_IN_STAGE2_STATE 10000
#define MAX_TIME_WAIT_BUTTON 3000
#define MAX_TIME_IN_STAGE1_STATE 3000

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(LCD_ADDRESS, LCD_ROWS, LCD_COL);
String welcome = "Welcome to the React! Game. Press Buttons to Start";
int curr_random;
uint8_t current_round = 1;
/* core business logic */

void initCore(){
  Serial.begin(9600);
}

void scroll(int *puntatore, bool *b, String string){
    digitalWrite(LED_RED01, *b ? LOW : HIGH);
    digitalWrite(LED_RED02, *b ? HIGH : LOW);
    *b = !*b;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(welcome.substring(*puntatore%string.length(), LCD_ROWS + *puntatore%string.length()).c_str());
    (*puntatore)++;
}

void intro(){
  if (isJustEnteredInState()){
    Serial.println("Intro...");
  }
  
  int dt = getCurrentTimeInState();
  if (dt > MAX_TIME_IN_INTRO_STATE){
    changeState(STAGE1_STATE);
  }
  lcd.init();
  lcd.backlight();
  digitalWrite(LED_GREEN01, LOW);
  digitalWrite(LED_GREEN02, LOW);
  int c = 0;
  bool b = true;
  while(!isButtonPressed(0) and !isButtonPressed(1)) {
    // lampeggia i led rossi
    scroll(&c, &b, welcome);
    delay(300);
  }
  lcd.clear();
  changeState(STAGE1_STATE);
}

void stage1(){
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
  for(uint8_t i = 0; i < NUM_BUTTONS; i++) {
    if (isButtonPressed(i)) {
      signPlayerErrorScore(i);
      resetInput();
      goto stage1_restart;
    }
  }
  if (isJustEnteredInState()){
    Serial.println("Stage1...");
    resetInput();
    curr_random = (int)random(0, 3);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(curr_random);
  }
  updateStateTime();

  if(getCurrentTimeInState() > MAX_TIME_IN_STAGE1_STATE / current_round) {
    changeState(FINAL_STATE);
  }
  while(!isButtonPressed(0) or !isButtonPressed(1));
  changeState(STAGE2_STATE);
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

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("P1" + String(getbuttonScore(0)));
  lcd.setCursor(10, 1);
  lcd.print("P2" + String(getbuttonScore(1)));
  calcBlink(p_correct[0], p_correct[1]);
  /* change the state if button 1 is pressed or max time elapsed*/
  if (isButtonPressed(1) || getCurrentTimeInState() > MAX_TIME_IN_STAGE2_STATE / current_round){
    changeState(FINAL_STATE);          
  }
  current_round++;
  changeState(STAGE1_STATE);
}


void finalize(){
  if (isJustEnteredInState()){
    String final_string;
    int N_SEC_FINALM = 10;
    int c = 0;
    bool b = true;
    Serial.println("Finalize...");
    lcd.clear();
    lcd.setCursor(0,0);
    if (getbuttonScore(0) == getbuttonScore(1)) {
      final_string = "Game over - Tie.";
    } else {
      final_string = "Game over - Player " + String(getbuttonScore(0) > getbuttonScore(1) ? "P1" : "P2") + " win.";
    }
    Serial.println(getCurrentTimeInState());
    while(getCurrentTimeInState() < N_SEC_FINALM*100000) {
      updateStateTime();
      scroll(&c, &b, final_string);
      delay(300);
    }
  }
  changeState(INTRO_STATE);
}
