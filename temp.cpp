#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>
#include <TouchScreen.h>

#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */

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

// forward declaration for various functions used for the game
void startPage();
void gameOver();
void checkTouchStartPage();
void checkTouchGameOver();
uint32_t randomNum();
void game();
int processSnake(int snakeLength, int speed);
void checkGameOver();

// Use hardware SPI (on Mega2560, #52, #51, and #50) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// structure that stores and represents each individual piece of the snake
// i.e. each part of the snake moves in a certain direction at different coordinates
struct snakeStruct {
  uint32_t x, y;
  char move; // dictates direction snake will move
  					// up = u, down = d, left = l, right = r
};

// structure that stores coordinates for an apple
struct appleCoordinates {
	uint32_t x;
	uint32_t y;
};

// declaration of global variables/structures to be used throughout game
int counter;
int score = 0;
int snakeLength;
snakeStruct snake[100];



void game() {
	tft.fillScreen(ILI9341_BLACK);
	snakeLength = 5;
	//randomize apple position and draw it
	appleCoordinates appleLocation = randomizeApple();
	int speed = displaySpeedScore();
	int eraseTailX = 500;
	int eraseTailY = 500;
	int tail;
	initSnake(snakeLength);

  while (true) {
    tail = processSnake(snakeLength, speed);


		snake[tail].x = eraseTailX;
		snake[tail].y = eraseTailY;


		if (appleLocation.x == snake[0].x && appleLocation.y == snake[0].y);
			appleLocation = randomizeApple();

			for (int i = 0; i < snakeLength; i++){
				// to avoid placing an apple on top of the snake's body
				if (appleLocation.x == snake[i].x && appleLocation.y == snake[i].y) {
					appleLocation = randomizeApple();
				}
			}

			tft.fillRect(apple.x, apple.y, CURSOR_SIZE, CURSOR_SIZE, ILI9341_RED);
			snakeLength += 2;
			score++;
			tft.fillRect(40, 228, 20, 10, ILI9341_BLACK);
			tft.setCursor(45, 229);
			tft.print(score);
		}

  }
}

void snakeTail(int trailX[], int trailY[], int snakeLength) {
  for (int i = 1; i < snakeLength; i++) {
    snake[i].x = trailX[i-1];
    snake[i].y = trailY[i-1];
    tft.fillRect(snake[i].x, snake[i].y, 5, 5, ILI9341_WHITE);
  }
}

/**** PROCESS SNAKE MOVEMENT AND DRAWING/REDRAWING ****/
int processSnake(int snakeLength, int speed) {
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);
	int trailX[snakeLength], trailY[snakeLength];

  // ***** SNAKE MOVE UP
  if (snake[0].move == 'u') {
		// store previous snake position in trailorary array
		// useful for erasing snake
    for (int i = 0; i < snakeLength; i++) {
      trailX[i] = snake[i].x;
      trailY[i] = snake[i].y;
    }

    snake[0].y -= 5;
    tft.fillRect(snake[0].x, snake[0].y, 5, 5, ILI9341_WHITE);
    snakeTail(trailX, trailY, snakeLength);
    tft.fillRect(trailX[(snakeLength) - 1], trailY[(snakeLength) - 1], 5, 5, ILI9341_BLACK);

    // if joystick tilted horizontally, change movement according to direction
		if (xVal < JOY_CENTER - JOY_DEADZONE) {
			if (yVal < JOY_CENTER - JOY_DEADZONE || yVal > JOY_CENTER + JOY_DEADZONE) {
				snake[0].move = 'u';
			}
      else {
				snakeTail(trailX, trailY, snakeLength);
      	snake[0].move = 'r';
			}
    }
    else if (xVal > JOY_CENTER + JOY_DEADZONE) {
			if (yVal < JOY_CENTER - JOY_DEADZONE || yVal > JOY_CENTER + JOY_DEADZONE) {
				snake[0].move = 'u';
			}
      else {
				snakeTail(trailX, trailY, snakeLength);
      	snake[0].move = 'l';
			}
    }
  delay(speed);
  }

  // ***** SNAKE MOVE DOWN
  else if (snake[0].move == 'd') {
    for (int i = 0; i < snakeLength; i++) {
      trailX[i] = snake[i].x;
      trailY[i] = snake[i].y;
    }

    snake[0].y += 5;
    tft.fillRect(snake[0].x, snake[0].y, 5, 5, ILI9341_WHITE);
    snakeTail(trailX, trailY, snakeLength);
    tft.fillRect(trailX[(snakeLength)-1], trailY[(snakeLength)-1], 5, 5, ILI9341_BLACK);

    // if joystick tilted horizontally, change movement according to direction
		if (xVal < JOY_CENTER - JOY_DEADZONE) {
			if (yVal < JOY_CENTER - JOY_DEADZONE || yVal > JOY_CENTER + JOY_DEADZONE) {
				snake[0].move = 'd';
			}
			else {
				snakeTail(trailX, trailY, snakeLength);
				snake[0].move = 'r';
			}
		}
		else if (xVal > JOY_CENTER + JOY_DEADZONE) {
			if (yVal < JOY_CENTER - JOY_DEADZONE || yVal > JOY_CENTER + JOY_DEADZONE) {
				snake[0].move = 'd';
			}
			else {
				snakeTail(trailX, trailY, snakeLength);
				snake[0].move = 'l';
				//tft.fillRect(trailX[(snakeLength)-1], trailY[(snakeLength)-1], 5, 5, ILI9341_BLACK);
			}
		}
  delay(speed);
}

// ***** SNAKE MOVE LEFT
if (snake[0].move == 'l') {
  for (int i = 0; i < snakeLength; i++) {
    trailX[i] = snake[i].x;
    trailY[i] = snake[i].y;
  }

  snake[0].x -= 5;
  tft.fillRect(snake[0].x, snake[0].y, 5, 5, ILI9341_WHITE);
  snakeTail(trailX, trailY, snakeLength);
  tft.fillRect(trailX[(snakeLength)-1], trailY[snakeLength-1], 5, 5, ILI9341_BLACK);

  // if joystick tilted vertically, change movement according to direction
	if (yVal < JOY_CENTER - JOY_DEADZONE) {
		if (xVal < JOY_CENTER - JOY_DEADZONE || xVal > JOY_CENTER + JOY_DEADZONE) {
			snake[0].move = 'l';
		}
		else {
    	snakeTail(trailX, trailY, snakeLength);
    	snake[0].move = 'u';
		}
	}
  else if (yVal > JOY_CENTER + JOY_DEADZONE) {
		if (xVal < JOY_CENTER - JOY_DEADZONE || xVal > JOY_CENTER + JOY_DEADZONE) {
			snake[0].move = 'l';
		}
		else {
    	snakeTail(trailX, trailY, snakeLength);
    	snake[0].move = 'd';
		}
  }
delay(speed);
  }

  // ***** SNAKE MOVE RIGHT
  if (snake[0].move == 'r') {
    for (int i = 0; i < snakeLength; i++) {
      trailX[i] = snake[i].x;
      trailY[i] = snake[i].y;
    }

    snake[0].x += 5;
    tft.fillRect(snake[0].x, snake[0].y, 5, 5, ILI9341_WHITE);
    snakeTail(trailX, trailY, snakeLength);
    tft.fillRect(trailX[snakeLength-1], trailY[(snakeLength)-1], 5, 5, ILI9341_BLACK);

    // if joystick tilted vertically, change movement according to direction
		if (yVal < JOY_CENTER - JOY_DEADZONE) {
			if (xVal < JOY_CENTER - JOY_DEADZONE || xVal > JOY_CENTER + JOY_DEADZONE) {
				snake[0].move = 'r';
			}
			else {
	    	snakeTail(trailX, trailY, snakeLength);
	    	snake[0].move = 'u';
			}
		}
	  else if (yVal > JOY_CENTER + JOY_DEADZONE) {
			if (xVal < JOY_CENTER - JOY_DEADZONE || xVal > JOY_CENTER + JOY_DEADZONE) {
				snake[0].move = 'r';
			}
			else {
	    	snakeTail(trailX, trailY, snakeLength);
	    	snake[0].move = 'd';
			}
	  }
  delay(speed);
}
	return snakeLength;
}

void setup() {
	init();

	pinMode(JOY_SEL, INPUT_PULLUP);

	Serial.begin(9600);
	tft.begin();
	tft.setRotation(3);
	tft.setRotation(-1);
	tft.setTextWrap(false);
}

int main() {
	setup();
	startPage();

	Serial.end();
	return 0;
}
