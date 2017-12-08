#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>

#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */

#include "touchscreen.h"
#include "snake.h"

// TFT display and SD card will share the hardware SPI interface.
// For the Adafruit shield, these are the default.
// The display uses hardware SPI, plus #9 & #10
// Mega2560 Wiring: connect TFT_CLK to 52, TFT_MISO to 50, and TFT_MOSI to 51.
#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6

// joystick pins
#define JOY_VERT A1
#define JOY_HORIZ A0
#define JOY_SEL 2

// width/height of the display when rotated horizontally
#define DISP_WIDTH 320
#define DISP_HEIGHT 220

// constants for the joystick
#define JOY_DEADZONE 64
#define JOY_CENTER 512

// These are the four touchscreen analog pins
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 5   // can be a digital pin
#define XP 4   // can be a digital pin

// Cursor size. For best results, use an odd number.
#define CURSOR_SIZE 5

// forward declaration for various game functions
void startPage();
void game();
int processSnake(int snakeLength, int speed);

// Use hardware SPI (on Mega2560, #52, #51, and #50) and the above for CS/DC
extern Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
extern TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

extern int counter;
extern int score;
extern int speed;
extern int tempX[];
extern int tempY[];
extern int snakeLength;

struct snakeStruct {
  int x, y;
  char move;
  // up = u, down = d, left = l, right = r
};

struct coordinates {
	uint32_t x;
	uint32_t y;
};

snakeStruct snake[100];

uint32_t randomNum();
coordinates randomizeApple();
void initSnake(int snakeLength);
int snakeTail(int tempX[], int tempY[], int snakeLength);
int processSnake(int snakeLength, int speed);

#endif
