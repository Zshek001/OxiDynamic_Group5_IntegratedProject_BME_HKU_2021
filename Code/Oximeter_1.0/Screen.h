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

  static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

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
  OLED.fillRect(0, 16, OLED.width() / 2,OLED.height(), SSD1306_BLACK);
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

void OLEDError(){
  OLED.fillRect(6,16,100,OLED.height(),SSD1306_BLACK);
  OLED.setCursor(6, 16);
  OLED.setTextSize(2);
  OLED.println("Finger Out");
}

void OLEDBTOFF(){
  OLED.fillRect(110,0,128,16,SSD1306_BLACK);
}

void OLEDBTON(){
  OLED.setCursor(110,0);
  OLED.setTextSize(1);
  OLED.print("BT");
}
