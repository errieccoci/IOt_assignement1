#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include "output.h"
#include "input.h"
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(LCD_ADDRESS, LCD_ROWS, LCD_COL);

void initLcd() {
  lcd.init();
  lcd.backlight();
}
void resetLcd(const int x = 0, const int y = 0) {
    lcd.clear();
    lcd.setCursor(x, y);
}
void printLcd(const String msg_left, const String msg_right = "") {
    lcd.setCursor(0,0);
    lcd.print(msg_left);
    lcd.setCursor(LCD_COL - msg_right.length(), 1);
    lcd.print(msg_right);
}

void printLcd(const long msg) {
    lcd.print(msg);
    
}

void scroll(int *puntatore, bool *b, String string){
    digitalWrite(LED_RED01, *b ? LOW : HIGH);
    digitalWrite(LED_RED02, *b ? HIGH : LOW);
    *b = !*b;
    resetLcd();
    printLcd(string.substring(*puntatore%string.length(), LCD_ROWS + *puntatore%string.length()).c_str());
    (*puntatore)++;
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