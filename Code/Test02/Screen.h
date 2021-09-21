#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define TFT_DC 9
#define TFT_CS 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void screen_init(){
   tft.begin();
   tft.setTextColor(ILI9341_WHITE);
   tft.fillScreen(ILI9341_BLACK);
}

void display_heartrate(int rate){
    Serial.println(rate);
  tft.fillRect(0, 0, tft.width(),tft.height() / 2, ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(5);
  tft.print(rate);
  tft.setTextSize(3);
  tft.println("/min");
}
