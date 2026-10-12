#ifndef __INPUT__
#define __INPUT__

#define LED_RED01 6
#define LED_GREEN01 7
#define LED_RED02 8
#define LED_GREEN02 9
#define NUM_BUTTONS 2
void initInput();
int getbuttonScore(int i);
void resetInput();
bool isButtonPressed(int buttonIndex);
long getLastButtonTime(const int i);
void incPlayerScore(const int i);
void signPlayerErrorScore(const int i);
void calcBlink(const bool p1, const bool p2);
#endif
