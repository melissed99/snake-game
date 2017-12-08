#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>
#include <TouchScreen.h>

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
#define JOY_STEPS_PER_PIXEL 64

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

// Cursor size. For best results, use an odd number.
#define CURSOR_SIZE 5

// forward declaration for various game functions
void startPage();
void game();
int processSnake(int snakeLength, int speed);

// Use hardware SPI (on Mega2560, #52, #51, and #50) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

int counter;
int score = 0;

void setup() {
	init();

	pinMode(JOY_SEL, INPUT_PULLUP);

	Serial.begin(9600);
	tft.begin();
	tft.setRotation(3);
	tft.setRotation(-1);
	tft.setTextWrap(false);
}

void game() {
  tft.fillScreen(ILI9341_BLACK);

	tft.setTextSize(1);
	tft.setCursor(5, 228+1);
	tft.print("SCORE: ");
	tft.setCursor(45, 228+1);
	tft.print(score);
	tft.drawRect(0, DISP_HEIGHT+5, DISP_WIDTH, 15, ILI9341_WHITE);

	int snakeLength = 5;
	initSnake(snakeLength);

	int speed = 0;
	if (counter == 0) {
		speed=300;
		tft.setCursor(260, 229);
		tft.print("EASY MODE");
	}
	if (counter == 1) {
		speed=100;
		tft.setCursor(250, 229);
		tft.print("MEDIUM MODE");
	}
	if (counter == 2) {
		speed=50;
		tft.setCursor(260, 229);
		tft.print("HARD MODE");
	}
	if (counter == 3) {
		speed=10;
		tft.setTextColor(ILI9341_BLUE);
		tft.setCursor(120, 229);
		tft.print("GOTTA GO FAST");
		tft.setTextColor(ILI9341_WHITE);
		tft.setCursor(255, 229);
		tft.print("SONIC MODE");
	}

	//randomize apple position
	coordinates appleLocation = randomizeApple();
	coordinates prizeApple = randomizeApple();
	tft.fillRect(appleLocation.x, appleLocation.y, CURSOR_SIZE, CURSOR_SIZE, ILI9341_RED);
	int eraseTailX = 500;
	int eraseTailY = 500;
	int tail;
  delay(20);

  while (true) {
    tail = processSnake(snakeLength, speed);
		snake[tail].x = eraseTailX;
		snake[tail].y = eraseTailY;

//EAT APPLE
		if (appleLocation.x == snake[0].x && appleLocation.y == snake[0].y){
			//tft.fillRect(appleLocation.x, appleLocation.y, CURSOR_SIZE, CURSOR_SIZE, ILI9341_WHITE);
			appleLocation = randomizeApple();
			for(int i=0; i<snakeLength; i++){

				//to avoid placing an apple on top of the snake's body
				if (appleLocation.x == snake[i].x && appleLocation.y == snake[i].y) {
					appleLocation = randomizeApple();
				}
			}

			tft.fillRect(appleLocation.x, appleLocation.y, CURSOR_SIZE, CURSOR_SIZE, ILI9341_RED);
			snakeLength += 2;
			score++;

			tft.fillRect(40, 228, 20, 10, ILI9341_BLACK);
			tft.setCursor(45, 228+1);
			tft.print(score);
		}


  }
}

int main() {
	setup();
	startPage();

	Serial.end();
	return 0;
}
