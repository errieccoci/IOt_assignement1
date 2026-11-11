#include "core.h"
#include "Arduino.h"
#include "kernel.h"
#include "input.h"

#define MAX_TIME_IN_INTRO_STATE 10000
#define MAX_TIME_IN_STAGE2_STATE 10000
#define MAX_WAIT_RANDOM 5000
#define TIME_BYE_MESSAGE 10000
// Time over that the player doesn't press the button, the round is over
#define MAX_WAIT_BUTTON 2000

const String WELCOME_MSG = "Welcome to React! Game. Press Buttons to Start";
const String BYE_MESSAGE_BASE = "Game Over - ";
const String WINNER_IS = "The winner is ";
const String TIE = "Tie";
const String GO = "Go!";
int MAX_TIME_STAGE2_CALCULATED;
int level = 1;
int num_round = 1;
int number;

/* core business logic */

void initCore(){
  Serial.begin(9600);
}

/**
 * @brief Starting stage of the game.
 * 
 * The two red leds are fading and the greens one are off.
 * The display show the message
 * "Wellcome to React! Grame. Press Buttons to Start" animated.
 * The game start when both buttons are pressed.
 * 
 */
void intro(){
  static float i = 0;
  static int startIndex = 0;

  if (isJustEnteredInState()){
    Serial.println("Intro...");
    printLCD(WELCOME_MSG);
    i = 0;
    startIndex = 0;
  }
  
  int dt = getCurrentTimeInState();
  float val =(sin(i) + 1.0) * 127.5;

  if (dt > MAX_TIME_IN_INTRO_STATE){
    printLCD("Sleep Mode");
    sleepMode();
    disableSleepMode();
    changeState(INTRO_STATE);
  }
  if (isButtonPressed(0) && isButtonPressed(1)){
    changeState(STAGE1_STATE);
  }
  level = map(readPot(), 10, 1000, 1, 4);
  printLedGreen(false, false);
  printLedRed(val*0.5, (255-val)*0.5);
  printLCD(WELCOME_MSG, startIndex, 
    startIndex + 
    (WELCOME_MSG.length() - LCD_COLUMNS > 0 ? LCD_COLUMNS : WELCOME_MSG.length()));
    i += 0.40;
    startIndex = (startIndex + 1) % WELCOME_MSG.length();
    delay(200);
  }
  
  /**
   * @brief Choose, after random time between 0 and 5000 ms, a random
   * number to display. During this time, a "Go!" message
   * is displayed on the LCD and all the leds are off.
   * After choosing the number, it is displayed and passed at STAGE 2.
   * 
   */
void stage1(){
  const float wait = getRandom(1000, MAX_WAIT_RANDOM);
  number = getRandom(1, 3);
  disableButtonScoring();
    
  if (isJustEnteredInState()){
    MAX_TIME_STAGE2_CALCULATED = (int) MAX_TIME_IN_STAGE2_STATE*2 / (num_round * level);
    Serial.println("Stage1... ");
    resetInput();
    printLCD(GO);
    num_round++;
  }
  updateStateTime();
  if (isButtonPressed(0) && isButtonPressed(1)){
    changeState(INTRO_STATE);          
  }

  if (getCurrentTimeInState() > (int)wait) {
    printLCD(String(number));
    enableButtonScoring();
    changeState(STAGE2_STATE);
  }

  for(int i = 0; i < NUM_BUTTONS; i++) {
    if (isButtonPressed(i)) {
      modifyScore(
        i == 0 ? PENALITY : NONE,
        i == 1 ? PENALITY : NONE,
        false,
        false
      );
      changeState(STAGE1_STATE);
    }
  }
}

/**
 * @brief While no button is pressed at the round or some
 * button is pressed, return. After that, if the players have enought
 * time, is calculated the round result, founding who won and who lost.
 * If the players have no time enought, the FINAL STATE is called.
 * 
 */
void stage2(){

  if (isJustEnteredInState()){
    Serial.println("Stage2...");
  }

  if (getCurrentTimeInState() > MAX_TIME_STAGE2_CALCULATED){
    modifyScore(NONE, NONE, false, false);
    changeState(FINAL_STATE);
    return;
  }

  /*
  Eeturns if players are still pressing buttons 
  or if no one has pressed a button yet
  */
  if (
    (millis() - getLastButtonPressedTimestamp(0) < MAX_WAIT_BUTTON ||
    millis() - getLastButtonPressedTimestamp(1) < MAX_WAIT_BUTTON) || 
    (!getButtonScore(0) && !getButtonScore(1))
  ) return;
  // if a button is pressed, now it will not affect the button score
  disableButtonScoring();

  const int faster =(int)(getLastButtonPressedTimestamp(0) > getLastButtonPressedTimestamp(1));
  int res[NUM_BUTTONS] = {NONE, NONE};
  if (getButtonScore(0) > number) res[0] = PENALITY;
  if (getButtonScore(1) > number) res[1] = PENALITY;
  res[0] = faster == 0 && res[0] != PENALITY && getButtonScore(0) == number ? WIN : res[0];
  res[1] = faster == 1 && res[1] != PENALITY && getButtonScore(1) == number ? WIN : res[1];
  modifyScore(res[0], res[1], true, true);
  changeState(STAGE1_STATE);
}

/**
 * @brief Display on the lcd the final result of the game, 
 * in particular who won (or there is a TIE).
 * 
 */
void finalize(){
  if (isJustEnteredInState()){
    Serial.println("Finalize...");
  }
  updateStateTime();
  const String final_message = BYE_MESSAGE_BASE + (
    getPlayerScore(0) > getPlayerScore(1) ? "P0" : (
      getButtonScore(0) < getPlayerScore(1) ? "P1" : TIE 
    )
  );
  int i = 0;
  while (getCurrentTimeInState() < TIME_BYE_MESSAGE)
  {
    printLCD(final_message, i, 
      i + LCD_COLUMNS < (signed int)final_message.length() ? 
        i + LCD_COLUMNS : final_message.length());
    i++;
    delay(250);
  }
  printLCD("", 0, 0);
}
