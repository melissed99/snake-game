#ifndef __TOUCHSCREEN_H__
#define __TOUCHSCREEN_H__

#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>

#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include "touchscreen.h"

// TFT display and SD card will share the hardware SPI interface.
// For the Adafruit shield, these are the default.
// The display uses hardware SPI, plus #9 & #10
// Mega2560 Wiring: connect TFT_CLK to 52, TFT_MISO to 50, and TFT_MOSI to 51.
#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6

// width/height of the display when rotated horizontally
#define DISP_WIDTH 320
#define DISP_HEIGHT 220

// These are the four touchscreen analog pins
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 5   // can be a digital pin
#define XP 4   // can be a digital pin

// calibration data for the touch screen, obtained from documentation
// the minimum/maximum possible readings from the touch point
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// thresholds to determine if there was a touch
#define MINPRESSURE   10
#define MAXPRESSURE 1000

// The touchscreen
extern TouchScreen ts;
extern int counter;
extern int score;


// function declarations
void checkTouchStartPage();
void checkTouchGameOver();
void gameOver();
void startPage();

#endif
