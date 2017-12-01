#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>
#include <TouchScreen.h>

#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
//#include <time.h>       /* time */

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
#define TFT_WIDTH 320
#define TFT_HEIGHT 240

#define DISP_WIDTH TFT_WIDTH
#define DISP_HEIGHT TFT_HEIGHT

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

// forward declaration for drawing the cursor
void startPage();
void game();
void processSnake(int snakeLength, int speed);

// Use hardware SPI (on Mega2560, #52, #51, and #50) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

void checkTouchStartPage() {
	TSPoint p = ts.getPoint();

	if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
		// no touch, just quit
		return;
	}

	p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
	p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

	if ((p.x>=(DISP_WIDTH/2 - 95)) && (p.x<=255) && (p.y>=115) && (p.y<=175) ) {
		game();
		return;
	}
}

void checkTouchGameOver() {
	TSPoint p = ts.getPoint();

	if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
		// no touch, just quit
		return;
	}
	p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
	p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

	if ((p.x>=(DISP_WIDTH/2 - 95)) && (p.x<=255) && (p.y>=100) && (p.y<=160) ) {
		startPage();
		return;
	}
}

int score = 0;

void gameOver() {
	tft.fillScreen(0x07E0);

	tft.setCursor(30, 20);
	tft.setTextColor(ILI9341_RED);
	tft.setTextSize(5);
	tft.print("GAME OVER");

	tft.fillRect(DISP_WIDTH/2 - 95, 100, 190, 60, ILI9341_BLACK);
	tft.drawRect(DISP_WIDTH/2 - 95, 100, 190, 60, ILI9341_BLACK);
	tft.setCursor(DISP_WIDTH/2 - 85, 110 );
	tft.setTextColor(ILI9341_WHITE);
	tft.setTextSize(6);
	tft.print("RESET");

	tft.setCursor(DISP_WIDTH/2 - 65, 190);
	tft.setTextColor (ILI9341_WHITE);
	tft.setTextSize(3);
	tft.println("SCORE: ");
	tft.setCursor(DISP_WIDTH/2 + 45, 190);
	tft.print(score);

	// reset score
	score = 0;

	while(true){
		checkTouchGameOver();
	}
}

void startPage() {
	tft.fillScreen(0x8811);

	tft.setCursor(DISP_WIDTH/2 - 45, 30);
	tft.setTextColor (ILI9341_WHITE);
	tft.setTextSize(3);
	tft.println("SNAKE");

	tft.setTextSize(1);
	tft.setCursor(DISP_WIDTH/2 - 65, 55);
	tft.println("Touch Start To Begin!");

	tft.fillRect(DISP_WIDTH/2 - 95, 115, 190, 60, ILI9341_BLACK);
	tft.drawRect(DISP_WIDTH/2 - 95, 115, 190, 60, ILI9341_BLACK);
	tft.setCursor(DISP_WIDTH/2 - 85, 125 );
	tft.setTextColor(ILI9341_WHITE);
	tft.setTextSize(6);
	tft.print("START");

	while(true){
		checkTouchStartPage();
	}
}

void setup() {
	init();

  // constrain so the cursor does not go off of the map display window
  cursorX = constrain(cursorX, 0, DISP_WIDTH - CURSOR_SIZE);
  cursorY = constrain(cursorY, 0, DISP_HEIGHT - CURSOR_SIZE);

	pinMode(JOY_SEL, INPUT_PULLUP);

	Serial.begin(9600);
	tft.begin();
	tft.setRotation(3);
	tft.setRotation(-1);
	tft.setTextWrap(false);
}

struct snakeStruct {
  int x, y;
  char move;
  // up = u, down = d, left = l, right = r
};

snakeStruct snake[100];

uint32_t randomNum1() {

	int randomKey = 0;
 	randomKey = rand();
	//return randomKey;
	if (randomKey %5 == 0) {
		return randomKey;
	}
	else {
		randomNum1();
	}
}

struct coordinates {
	uint32_t x;
	uint32_t y;
};

coordinates random_apple() {
	int i = 1;
	coordinates apple;

	uint32_t randomnumX = randomNum1();
	uint32_t randomnumY = randomNum1();

	apple.x = randomnumX % DISP_WIDTH;
	apple.y = randomnumY % DISP_HEIGHT;

	//constrain so that the apple will appear within the screen
	if (apple.x <= 0 || apple.x >= DISP_WIDTH) {
		apple.x = randomnumX % DISP_WIDTH;
	}
	if (apple.y <= 0 || apple.y >= DISP_HEIGHT) {
		apple.y = randomnumY % DISP_HEIGHT;
	}
	return apple;
}

void initSnake(int snakeLength) {
  // snake[0].x = DISP_WIDTH/2;
  // snake[0].y = DISP_HEIGHT/2;
  snake[0].move = 'u';
	for (int i = 0; i < snakeLength) {
		snake[i].x = DISP_WIDTH/2 + 5*i;
		snake[i].y = DISP_HEIGHT/2 + 5*i;
	}
}

void game() {
  tft.fillScreen(ILI9341_BLACK);

	tft.setTextSize(1);
	tft.setCursor(5, 228);
	tft.print("SCORE: ");
	tft.setCursor(45, 228);
	tft.print(score);

	int snakeLength = 5;
	initSnake(snakeLength);
	//int score = 0;
	int speed = 300;
	//randomize apple position
	coordinates choose_apple = random_apple();
	tft.fillRect(choose_apple.x, choose_apple.y, CURSOR_SIZE, CURSOR_SIZE, ILI9341_RED); // lol why CYAN

  delay(20);

  while (true) {
    processSnake(snakeLength, speed);

		// tft.setCursor(15, 228);
		// tft.print(score);

		// Serial.print("choose_apple.x: ");
		// Serial.print(choose_apple.x);
		// Serial.print("snakehead.x: " );
		// Serial.print(snake[0].x);
		// Serial.print(" ");
    //
		// Serial.print("choose_apple.y: ");
		// Serial.print(choose_apple.y);
		// Serial.print("snakehead.y: " );
		// Serial.println(snake[0].y);

		//eat apple
		if (choose_apple.x == snake[0].x && choose_apple.y == snake[0].y){
			choose_apple = random_apple();
			tft.fillRect(choose_apple.x, choose_apple.y, CURSOR_SIZE, CURSOR_SIZE, ILI9341_RED);
			snakeLength += 2;
			score++;
			speed -= 25;
			Serial.print(speed);
			//Serial.println(score);
			tft.fillRect(40, 225, 10, 10, ILI9341_BLACK);
			tft.setCursor(45, 228);
			tft.print(score);
		}
  }
}

int* snakeTail(int tempX[], int tempY[], int snakeLength) {
  for (int i = 1; i < snakeLength; i++) {
    snake[i].x = tempX[i-1];
    snake[i].y = tempY[i-1];
    tft.fillRect(snake[i].x, snake[i].y, 5, 5, 0xFFFF);
  }
  return tempX, tempY;
}

void processSnake(int snakeLength, int speed) {
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);
  // copy the joystick orientation(?)
  int oldX = xVal; //cursorX;
  int oldY = yVal; //cursorY;

	/**** PROCESS SNAKE MOVEMENT AND DRAWING/REDRAWING ****/
	int tempX[snakeLength], tempY[snakeLength];

	// *** restrain diagonal movement
  //doesn't actually do anything but not sure
	if (yVal != oldY) {
			xVal = oldX;
	}
	else if (xVal != oldX) {
		yVal = oldY;
	}

  // ***** SNAKE MOVE UP
  if (snake[0].move == 'u') {
		// store previous snake position in temp array
		// useful for erasing snake
    for (int i = 0; i < snakeLength; i++) {
      tempX[i] = snake[i].x;
      tempY[i] = snake[i].y;
    }

    snake[0].y -= 5;
    tft.fillRect(snake[0].x, snake[0].y, 5, 5, 0xFFFF);
    snakeTail(tempX, tempY, snakeLength);
    tft.fillRect(tempX[(snakeLength) - 1], tempY[(snakeLength) - 1], 5, 5, 0x0000);

    // if joystick tilted horizontally, change movement according to direction
    if ((xVal < JOY_CENTER - JOY_DEADZONE) && yVal == oldY) {
      snakeTail(tempX, tempY, snakeLength);
      snake[0].move = 'r';
    }
    else if ((xVal > JOY_CENTER + JOY_DEADZONE) && yVal == oldY) {
      snakeTail(tempX, tempY, snakeLength);
      snake[0].move = 'l';
      //tft.fillRect(tempX[(snakeLength)-1], tempY[(snakeLength)-1], 5, 5, 0x0000);
    }
  delay(speed);
  }

  // ***** SNAKE MOVE DOWN
  else if (snake[0].move == 'd') {
    for (int i = 0; i < snakeLength; i++) {
      tempX[i] = snake[i].x;
      tempY[i] = snake[i].y;
    }

    snake[0].y += 5;
    tft.fillRect(snake[0].x, snake[0].y, 5, 5, 0xFFFF);
    snakeTail(tempX, tempY, snakeLength);
    tft.fillRect(tempX[(snakeLength)-1], tempY[(snakeLength)-1], 5, 5, 0x0000);

    // if joystick tilted horizontally, change movement according to direction
    if ((xVal < JOY_CENTER - JOY_DEADZONE) && yVal == oldY) {
      snakeTail(tempX, tempY, snakeLength);
      snake[0].move = 'r';
    }
    else if ((xVal > JOY_CENTER + JOY_DEADZONE) && yVal == oldY) {
      snakeTail(tempX, tempY, snakeLength);
      snake[0].move = 'l';
      tft.fillRect(tempX[(snakeLength)-1], tempY[(snakeLength)-1], 5, 5, 0x0000);
    }
  delay(speed);
}

// ***** SNAKE MOVE LEFT
if (snake[0].move == 'l') {
  for (int i = 0; i < snakeLength; i++) {
    tempX[i] = snake[i].x;
    tempY[i] = snake[i].y;
  }

  snake[0].x -= 5;
  tft.fillRect(snake[0].x, snake[0].y, 5, 5, 0xFFFF);
  snakeTail(tempX, tempY, snakeLength);
  tft.fillRect(tempX[(snakeLength)-1], tempY[snakeLength-1], 5, 5, 0x0000);

  // if joystick tilted vertically, change movement according to direction
  if ((yVal < JOY_CENTER - JOY_DEADZONE) && xVal == oldX) {
    snakeTail(tempX, tempY, snakeLength);
    snake[0].move = 'u';
  }
  else if ((yVal > JOY_CENTER + JOY_DEADZONE) && xVal == oldX) {
    snakeTail(tempX, tempY, snakeLength);
    snake[0].move = 'd';
    //tft.fillRect(tempX[(snakeLength)-1], tempY[(snakeLength)-1], 5, 5, 0x0000);
  }
delay(speed);
  }

  // ***** SNAKE MOVE RIGHT
  if (snake[0].move == 'r') {
    for (int i = 0; i < snakeLength; i++) {
      tempX[i] = snake[i].x;
      tempY[i] = snake[i].y;
    }

    snake[0].x += 5;
    tft.fillRect(snake[0].x, snake[0].y, 5, 5, 0xFFFF);
    snakeTail(tempX, tempY, snakeLength);
    tft.fillRect(tempX[snakeLength-1], tempY[(snakeLength)-1], 5, 5, 0x0000);

    // if joystick tilted vertically, change movement according to direction
    if ((yVal < JOY_CENTER - JOY_DEADZONE) && xVal == oldX) {
      snakeTail(tempX, tempY, snakeLength);
      snake[0].move = 'u';
    }
    else if ((yVal > JOY_CENTER + JOY_DEADZONE) && xVal == oldX) {
      snakeTail(tempX, tempY, snakeLength);
      snake[0].move = 'd';
      //tft.fillRect(tempX[(snakeLength)], tempY[(snakeLength)-1], 5, 5, 0x0000);
    }
  delay(speed);
}

	/*****************************************************/

	// ************ GAME OVER CONDITIONS *************** //
	if (snake[0].y > DISP_HEIGHT - CURSOR_SIZE/2 || snake[0].y < 0) {
		gameOver();
	}
	if (snake[0].x > DISP_WIDTH - CURSOR_SIZE/2 || snake[0].x < CURSOR_SIZE) {
		gameOver();
	}

	/*****///GAME OVER CONDITION IF SNAKE RUNS INTO ITSELF

}


int main() {
	setup();
	startPage();

	while (true) {
		game();
  }

	Serial.end();
	return 0;
}
