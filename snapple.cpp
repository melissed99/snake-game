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
<<<<<<< HEAD
#define GREEN 0x07E0
#define MAGENTA 0xF81F
=======
>>>>>>> 99cdf6825bdbbc85a97f4e82436154dfd6a88fcf

#define DISP_WIDTH TFT_WIDTH
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

// forward declaration for drawing the cursor
void startPage();
void game();
void processSnake(int snakeLength, int speed);

// Use hardware SPI (on Mega2560, #52, #51, and #50) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

int counter;

void checkTouchStartPage() {
	TSPoint p = ts.getPoint();

	if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
		// no touch, just quit
		return;
	}

	p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
	p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

	counter=0;

//EASY

	if((p.y>=110) && (p.y<=313) && (p.x>=100) && (p.x<=200)){
		counter=0;
		game();
	}

	//MEDIUM
	if((p.y>=5) && (p.y<=100) && (p.x>=100) && (p.x<=200)){
		counter=1;
		game();
	}

	//HARD
	if((p.y>=110) && (p.y<=313) && (p.x>=190)&& (p.x<= 280)){
		counter=2;
		game();
	}

	//SONIC
	if((p.y>=5) && (p.y<=100) && (p.x>=190) && (p.x<= 280)){
		counter=3;
		game();
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

	if ((p.y>=(DISP_WIDTH/2 - 125)) && (p.y<=180) && (p.x>=100) && (p.x<=200) ) {
		startPage();
		return;
	}
}

int score = 0;

void gameOver() {
	tft.fillScreen(GREEN);

	tft.setCursor(30, 20);
	tft.setTextColor(ILI9341_RED);
	tft.setTextSize(5);
	tft.print("GAME OVER");

	tft.fillRect(DISP_WIDTH/2 - 95, 100, 190, 60, GREEN);
	tft.fillRect(DISP_WIDTH/2 - 97, 102, 186, 56, ILI9341_BLACK);
	//tft.drawRect(DISP_WIDTH/2 - 95, 100, 190, 60, ILI9341_BLACK);
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
	tft.fillScreen(ILI9341_BLACK);
	//uint16_t purple = 0x8811;

	tft.setCursor(DISP_WIDTH/2 - 45, 30);
	tft.setTextColor (ILI9341_WHITE);
	tft.setTextSize(3);
	tft.println("SNAKE");

	tft.setTextSize(2);
	tft.setCursor(DISP_WIDTH/2 - 120, 55);
	tft.println("Select a difficulty");
	uint16_t customColour = MAGENTA; //tft.color565(142, 53, 239);
	tft.fillRect(5, 90, (DISP_WIDTH/2)-10, 60 , customColour);
	tft.fillRect(7, 92, (DISP_WIDTH/2)-14, 56 , ILI9341_BLACK);
	tft.setCursor(35,105);
	tft.setTextSize(4);
	tft.print("EASY");
	tft.fillRect(5, 160, (DISP_WIDTH/2)-10, 60 , customColour);
	tft.fillRect(7, 162, (DISP_WIDTH/2)-14, 56 , ILI9341_BLACK);
	tft.setCursor(35, 175);
	tft.print("HARD");
	tft.fillRect((DISP_WIDTH/2)+3, 90, (DISP_WIDTH/2)-10, 60 , customColour);
	tft.fillRect((DISP_WIDTH/2)+5, 92, (DISP_WIDTH/2)-14, 56 , ILI9341_BLACK);
	tft.setCursor(167, 105);
	tft.print("MEDIUM");
	tft.fillRect((DISP_WIDTH/2)+3, 160, (DISP_WIDTH/2)-10, 60 , customColour);
	tft.fillRect((DISP_WIDTH/2)+5, 162, (DISP_WIDTH/2)-14, 56 , ILI9341_BLACK);
	tft.setCursor(180, 175);
	tft.print("SONIC");

	while(true){
		checkTouchStartPage();
	}
}

void setup() {
	init();

  // constrain so the cursor does not go off of the map display window
  //snake[0].x = constrain(cursorX, 0, DISP_WIDTH - CURSOR_SIZE);
  //snake[0].y = constrain(snake[0].y, 0, DISP_HEIGHT - CURSOR_SIZE);

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

uint32_t randomNum() {

	int randomKey = 0;
	srand(millis());
 	randomKey = rand();

	if (randomKey %5 == 0) {
		return randomKey;
	}
	else {
		randomNum();
	}
}

struct coordinates {
	uint32_t x;
	uint32_t y;
};
//int snakeLength = 5;

coordinates randomizeApple() {

	coordinates apple;

	uint32_t randomnumX = randomNum();
	uint32_t randomnumY = randomNum();

	apple.x = randomnumX % DISP_WIDTH;
	apple.y = randomnumY % DISP_HEIGHT;

	//constrain so that the apple will appear within the screen
	// if (apple.x <= 0 || apple.x >= DISP_WIDTH) {
	// 	apple.x = randomnumX % DISP_WIDTH;
	// }
	// if (apple.y <= 0 || apple.y >= DISP_HEIGHT) {
	// 	apple.y = randomnumY % DISP_HEIGHT;
	// }
	// Serial.print(snakeLength);
	//
	// for(int i=0; i<snakeLength; i++){
	// 	//to avoid placing an apple on top of the snake's body
	// 	if (apple.x == snake[i].x && apple.y == snake[i].y) {
	// 		apple.x = randomnumX % DISP_WIDTH;
	// 		apple.y = randomnumY % DISP_HEIGHT;
	// 	}
	// }

// int i=0;
// while(i<snakeLength){
// 	if (apple.x == snake[i].x || apple.y == snake[i].y) {
// 				apple.x = randomnumX % DISP_WIDTH;
// 				apple.y = randomnumY % DISP_HEIGHT;
//	}
// }


	return apple;
}


void initSnake(int snakeLength) {
  // snake[0].x = DISP_WIDTH/2;
  // snake[0].y = DISP_HEIGHT/2;
  snake[0].move = 'u';
	for (int i = 0; i < snakeLength; i++) {
		snake[i].x = DISP_WIDTH/2;
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
	tft.drawLine(0, DISP_HEIGHT+5, DISP_WIDTH, DISP_HEIGHT+5, ILI9341_WHITE);

	int snakeLength = 5;
	initSnake(snakeLength);

	int speed = 0;
	if (counter == 0) {
		speed=300;
	}
	if (counter == 1) {
		speed=100;
	}
	if (counter == 2) {
		speed=50;
	}
	if (counter == 3) {
		speed=10;
	}

	// snake[0].x = constrain(snake[0].x, 0, DISP_WIDTH - CURSOR_SIZE);
	// snake[0].y = constrain(snake[0].y, 0, DISP_HEIGHT - CURSOR_SIZE);

	//randomize apple position
	coordinates appleLocation = randomizeApple();
	tft.fillRect(appleLocation.x, appleLocation.y, CURSOR_SIZE, CURSOR_SIZE, ILI9341_RED);

  delay(20);

  while (true) {
    processSnake(snakeLength, speed);

		// Serial.print("appleLocation.x: ");
		// Serial.print(appleLocation.x);
		// Serial.print("snakehead.x: " );
		// Serial.print(snake[0].x);
		// Serial.print(" ");
		//
		// Serial.print("appleLocation.y: ");
		// Serial.print(appleLocation.y);
		// Serial.print("snakehead.y: " );
		// Serial.println(snake[0].y);

		//eat apple
		if (appleLocation.x == snake[0].x && appleLocation.y == snake[0].y){
			tft.fillRect(appleLocation.x, appleLocation.y, CURSOR_SIZE, CURSOR_SIZE, ILI9341_WHITE);
			appleLocation = randomizeApple();

			Serial.print(snakeLength);

			for(int i=0; i<snakeLength; i++){
				//to avoid placing an apple on top of the snake's body
				if (appleLocation.x == snake[i].x && appleLocation.y == snake[i].y) {

					uint32_t randomnumappX=randomNum();
					uint32_t randomnumappY=randomNum();
					appleLocation.x = randomnumappX % DISP_WIDTH;
					appleLocation.y = randomnumappY % DISP_HEIGHT;
				}
			}


			// for(int i=0; i<snakeLength; i++){
			// 	//to avoid placing an apple on top of the snake's body
			//
			// 	coordinates randomapple = randomizeApple();
			// 	if (apple.x == snake[i].x && apple.y == snake[i].y) {
			// 		appleLocation.x = randomapple % DISP_WIDTH;
			// 		appleLocation.y = randomapple % DISP_HEIGHT;
			// 	}
			// }


				// //to avoid placing an apple on top of the snake's body
				// 	if (appleLocation.x == snake[].x || appleLocation.y == snake[].y) {
				// 		appleLocation = randomizeApple();
				// 	}


			tft.fillRect(appleLocation.x, appleLocation.y, CURSOR_SIZE, CURSOR_SIZE, ILI9341_RED);
			snakeLength += 2;
			// int oldCoord[snakeLength][snakeLength]
			// for i = 0; i < snakeLength; i++
			// 	for j = 0; j < snakeLength; j++
			//		oldCoord[i] = snake[i].x;
			//		oldCoord[j] = snake[j].y;
			score++;
			//speed -= 25;
			tft.fillRect(40, 228, 20, 10, ILI9341_BLACK);
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
	if (snake[0].y - CURSOR_SIZE/2> DISP_HEIGHT || snake[0].y < 0) {
		gameOver();
	}
	if (snake[0].x > DISP_WIDTH|| snake[0].x < 0) {
		gameOver();
	}
	for (int i = 1; i < snakeLength; i++) {
		if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
			gameOver();
		}
	}

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
