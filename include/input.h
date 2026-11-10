#ifndef __INPUT__
#define __INPUT__

#include <Arduino.h>
#include "config.h"

const uint8_t PENALITY = -1;
const uint8_t NONE = 0;
const uint8_t WIN = 1;

void initInput();
void resetInput();
bool isButtonPressed(int buttonIndex);
void printLedGreen(const bool s1, const bool s2);
void printLedRed(const int s1, const int s2);
void printLCD(const String msg, const int startIndex = 0, const int endIndex = LCD_COLUMNS);
float getRandom(const int min, const int max);
void enableButtonScoring();
void disableButtonScoring();
int getPlayerScore(const int p);
long getLastButtonPressedTimestamp(const int buttonIndex);
int getButtonScore(const int buttonIndex);
void modifyScore(const uint8_t p1, const uint8_t p2, const bool show_lcd);
int readPot();
#endif
