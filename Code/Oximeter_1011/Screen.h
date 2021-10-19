/*
 * This file includes the function to control the screen.
 */
#define screen 1

#ifndef constant
#include "Constant.h"
#endif

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_CLK   2
#define OLED_MOSI   3
#define OLED_RESET 4
#define OLED_DC    5
#define OLED_CS    6

Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void OLEDinit(){
  OLED.begin(SSD1306_SWITCHCAPVCC);
  OLED.fillScreen(SSD1306_BLACK);
  OLED.setTextColor(SSD1306_WHITE);
  OLED.setTextSize(2);
}

void OLEDtitle(){
  OLED.setCursor(0, 0);
  OLED.setTextSize(1);
  OLED.print("   HR      SpO2%");
  OLED.display();
}

void OLEDheartrate(int HR){
  OLED.fillRect(6, 16, OLED.width() / 2,OLED.height(), SSD1306_BLACK);
  OLED.setCursor(6, 16);
  OLED.setTextSize(3);
  OLED.print(HR);
}

void OLEDR(int R){
  OLED.fillRect(64, 16, 100,OLED.height(), SSD1306_BLACK);
  OLED.setCursor(64, 16);
  OLED.setTextSize(3);
  OLED.print(R);

}
