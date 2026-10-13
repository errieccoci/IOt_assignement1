#ifndef __OUTPUT__
#define __OUTPUT__

#define LCD_ADDRESS 0x27 // corrisponde al pin A4
#define LCD_ROWS 20
#define LCD_COL 4

void initLcd();
void resetLcd(const int x = 0, const int y = 0);
void printLcd(const String msg_left, const String msg_right = "");
void printLcd(const long msg);
void scroll(int *puntatore, bool *b, String string);

#endif 