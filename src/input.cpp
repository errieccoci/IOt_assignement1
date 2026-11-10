#include "input.h"
#include "Arduino.h"
#include "config.h"

#include <EnableInterrupt.h>
#include <LiquidCrystal_I2C.h>

#define BOUNCING_TIME 30
#define FAID_PENALITY 2000

uint8_t inputPins[NUM_BUTTONS] = {BUT01_PIN, BUT02_PIN};
uint8_t greenLed[NUM_BUTTONS] = {GREEN1_PIN, GREEN2_PIN};
uint8_t redLed[NUM_BUTTONS] = {RED1_PIN, RED2_PIN};
bool scoreEnabled = false;
bool buttonPressed[NUM_BUTTONS] = {false, false};
int playerScore[NUM_BUTTONS] = {0, 0};
int buttonScore[NUM_BUTTONS] = {0, 0};

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(LCD_ADDRESS, 20, 4);

/* used for debouncing */
long lastButtonPressedTimestamps[NUM_BUTTONS];

void buttonHandler(int i);
void buttonHandler0(){ buttonHandler(0); }
void buttonHandler1(){ buttonHandler(1); }

void (*buttonHandlers[NUM_BUTTONS])() = { buttonHandler0, buttonHandler1 };

void buttonHandler(int i){
  long ts = millis();
  if (ts - lastButtonPressedTimestamps[i] > BOUNCING_TIME){
    lastButtonPressedTimestamps[i] = ts;
    int status = digitalRead(inputPins[i]);
    if (status == HIGH && !buttonPressed[i]) { 
        buttonPressed[i] = true;
        if (scoreEnabled) {
          buttonScore[i]++;
        }
    }
    if (status == LOW && buttonPressed[i]) { 
        buttonPressed[i] = false;
    } 

  }
}

void initInput(){
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(inputPins[i], INPUT); 
    pinMode(greenLed[i], OUTPUT);
    pinMode(redLed[i], OUTPUT); 
    digitalWrite(greenLed[i], LOW);
    analogWrite(redLed[i], 0);
    enableInterrupt(inputPins[i], buttonHandlers[i], CHANGE);       
  }
  scoreEnabled = false;
  lcd.init();
  lcd.backlight();
  randomSeed(analogRead(A0));
}

void printLCD(const String msg, const int startIndex = 0, const int endIndex = LCD_COLUMNS) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg.substring(startIndex, endIndex));
}


void resetInput(){
  long ts = millis();
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttonPressed[i] = false;      
    lastButtonPressedTimestamps[i] = ts;    
  }
  lcd.clear();
  for (int i = 0; i < NUM_BUTTONS; i++) {
    digitalWrite(greenLed[i], LOW);
    analogWrite(redLed[i], 0);
    buttonScore[i] = 0;
  }
}
bool isButtonPressed(int buttonIndex){
  return buttonPressed[buttonIndex];
}

bool hasBeenPressed(int index){
  return buttonPressed[index]; 
}

void printLedGreen(const bool s1, const bool s2) {
  digitalWrite(greenLed[0], s1 ? HIGH : LOW);
  digitalWrite(greenLed[1], s2 ? HIGH : LOW);
}

void printLedRed(const int s1, const int s2) {
  analogWrite(redLed[0], s1);
  analogWrite(redLed[1], s2);
}

float getRandom(const int min, const int max){
  return random(min, max + 1);
}

void enableButtonScoring(){
  scoreEnabled = true;
}

void disableButtonScoring(){
  scoreEnabled = false;
}

int getPlayerScore(const int p) {
  return playerScore[p];
}

long getLastButtonPressedTimestamp(const int buttonIndex) {
  return lastButtonPressedTimestamps[buttonIndex];
}

int getButtonScore(const int buttonIndex) {
  return buttonScore[buttonIndex];
}

/**
 * @brief Function that translate the result of a round as
 * WIN, PENALITY or NONE in a brief. If there is a win for
 * the player, the score is increased and the green led will
 * be turned on, else the score will be decreased and the red
 * led will be turned on as faiding. 
 * 
 * @param p0 the player0 result (WIN, PENALITY, NONE)
 * @param p2 the player1 result (WIN, PENALITY, NONE)
 * @param show_lcd specify if showing the players score on the lcd
 * 
 */
void modifyScore(const int p0, const int p1, const bool show_lcd) {
  playerScore[0] += p0;
  playerScore[1] += p1;
  const long now = millis();
  float i = 0;
  if (show_lcd) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("P1: " + (String)getPlayerScore(0));
    lcd.setCursor(LCD_COLUMNS - String("P2: " + String(getPlayerScore(1))).length(), 0);
    lcd.print("P2: " + (String)getPlayerScore(1));
  }
  while (millis() - now < FAID_PENALITY)
  {
    float val = (sin((i+=0.2)) + 1.0) *127.5*0.5;
    printLedGreen(p0 == WIN ? val : 0, p1 == WIN ? val : 0);
    printLedRed(p0 == PENALITY ? val: 0, p1 == PENALITY ? val : 0);
    delay(10);
  }
}

int readPot() {
  return analogRead(POT_PIN);
}
