#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>
#include <TouchScreen.h>

#include <stdio.h>
#include <stdlib.h>     /* to use rand */

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

// structure that stores and represents each individual piece of the snake
// i.e. each part of the snake moves in a certain direction at different coordinates
struct snakeStruct {
  uint32_t x, y;
  char move; // dictates direction snake will move
  					// up = u, down = d, left = l, right = r
};

// structure that stores coordinates for an apple
struct coordinates {
	uint32_t x;
	uint32_t y;
};

// forward declaration for drawing the cursor
void startPage();
void initSnake(int snakeLength);
int displaySpeedScore();
void game();
int processSnake(int snakeLength, int speed);
void checkGameOver();
uint32_t randomNum();
coordinates randomizeApple();

// Use hardware SPI (on Mega2560, #52, #51, and #50) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// declaration of global variables/structures to be used throughout game
int counter;
int score = 0;
int snakeLength;
snakeStruct snake[100];

// ======================================================
// FUNCTIONS FOR START PAGE & GAME OVER TOUCHSCREEN PAGES
// ======================================================

//function that checks which difficulty the player selected
void checkTouchStartPage() {
	TSPoint p = ts.getPoint();

	if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
		// no touch, just quit
		return;
	}

	p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
	p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

	counter = 0;

	// EASY
	if((p.y >= 110) && (p.y <= 313) && (p.x >= 100) && (p.x <= 200)){
		counter = 0;
		game();
	}

	// MEDIUM
	if((p.y >= 5) && (p.y <= 100) && (p.x >= 100) && (p.x <= 200)){
		counter = 1;
		game();
	}

	// HARD
	if((p.y >= 110) && (p.y <= 313) && (p.x >= 190)&& (p.x <= 280)){
		counter = 2;
		game();
	}

	// SONIC
	if((p.y >= 5) && (p.y <= 100) && (p.x >= 190) && (p.x <= 280)){
		counter = 3;
		game();
	}
}

// function that checks if the player touches the reset button to restart the game
void checkTouchGameOver() {
	TSPoint p = ts.getPoint();

	if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
		// no touch, just quit
		return;
	}
	p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
	p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

	if ((p.y >= (DISP_WIDTH/2 - 125)) && (p.y <= 180) && (p.x >= 100) && (p.x <= 200)) {
		startPage();
		return;
	}
}

//displays the start page and the difficulty selection
void startPage() {
	tft.fillScreen(ILI9341_BLACK);

	tft.setCursor(DISP_WIDTH/2 - 45, 30);
	tft.setTextColor (ILI9341_WHITE);
	tft.setTextSize(3);
	tft.println("SNAKE");

	tft.setTextSize(2);
	tft.setCursor(DISP_WIDTH/2 - 120, 55);
	tft.println("Select a difficulty");

	tft.drawRect(5, 90, (DISP_WIDTH/2)-10, 60 , ILI9341_MAGENTA);
	tft.drawRect(7, 92, (DISP_WIDTH/2)-14, 56 , ILI9341_MAGENTA);
	tft.setCursor(35,105);
	tft.setTextSize(4);
	tft.print("EASY");

	tft.drawRect(5, 160, (DISP_WIDTH/2)-10, 60 , ILI9341_MAGENTA);
	tft.drawRect(7, 162, (DISP_WIDTH/2)-14, 56 , ILI9341_MAGENTA);
	tft.setCursor(35, 175);
	tft.print("HARD");

	tft.drawRect((DISP_WIDTH/2)+3, 90, (DISP_WIDTH/2)-10, 60 ,ILI9341_MAGENTA);
	tft.drawRect((DISP_WIDTH/2)+5, 92, (DISP_WIDTH/2)-14, 56 , ILI9341_MAGENTA);
	tft.setCursor(167, 105);
	tft.print("MEDIUM");

	tft.drawRect((DISP_WIDTH/2)+3, 160, (DISP_WIDTH/2)-10, 60 , ILI9341_MAGENTA);
	tft.drawRect((DISP_WIDTH/2)+5, 162, (DISP_WIDTH/2)-14, 56 , ILI9341_MAGENTA);
	tft.setCursor(180, 175);
	tft.print("SONIC");

	while(true){
		checkTouchStartPage();
	}
}

//displays the game over screen along with the final score of the player and a reset button is the player wants to play again
void gameOver() {
	tft.fillScreen(ILI9341_BLACK);

	tft.setCursor(30, 20);
	tft.setTextColor(ILI9341_RED);
	tft.setTextSize(5);
	tft.print("GAME OVER");

	tft.drawRect(DISP_WIDTH/2 - 96, 100, 192, 60, ILI9341_GREEN);
	tft.drawRect(DISP_WIDTH/2 - 94, 102, 188, 56, ILI9341_GREEN);
	tft.setCursor(DISP_WIDTH/2 - 85, 110);
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

// ===================================================================
// MAIN GAME FUNCTION
// ===================================================================
// first erases the start page, then initiates
// various variables and then enters an infinite loop so that the
// snake is always moving, erasing its tail, checks for game over
// conditions, and checks whether or not the snake has eaten an
// apple on the LCD screen. if snake has eaten an apple, score is
// incremented and a new apple is randomly placed onto the LCD screen
// ===================================================================

void game() {
  tft.fillScreen(ILI9341_BLACK);
	int snakeLength = 5;
	int speed = displaySpeedScore();
	initSnake(snakeLength);
	//randomize apple position
	coordinates appleLocation = randomizeApple();
	tft.fillRect(appleLocation.x, appleLocation.y, CURSOR_SIZE, CURSOR_SIZE, ILI9341_RED);
	int eraseTailX = 500;
	int eraseTailY = 500;
	int tail;
  delay(20);

  while (true) {
    tail = processSnake(snakeLength, speed);

		// erase the tail of the snake and set its coordinates to be outside of the
		// screen since reseting would cause flashing at (0, 0)
		snake[tail].x = eraseTailX;
		snake[tail].y = eraseTailY;


		// check if there is a collision between the snake head and apple
		// if there is a collision, draw a new apple, erase eaten apple,
		// increment the score and snake length. if new apple is drawn
		// over top any part of the snake, redraw another new apple
		if (appleLocation.x == snake[0].x && appleLocation.y == snake[0].y){
			appleLocation = randomizeApple();

      //to avoid placing an apple on top of the snake's body
			for(int i = 0; i < snakeLength; i++){
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
		checkGameOver();
  }
}

// =======================================================
// HELPER FUNCTION THAT DRAWS SCORE AND CURRENT GAME MODE
// DURING GAMEPLAY
// function returns an integer that will dictate level of
// difficulty of the game mode selected
// =======================================================

int displaySpeedScore() {
	tft.setTextSize(1);
	tft.setCursor(5, 229);
	tft.print("SCORE: ");
	tft.setCursor(45, 229);
	tft.print(score);
	tft.drawRect(0, DISP_HEIGHT + 5, DISP_WIDTH, 15, ILI9341_WHITE);

	int speedNum = 0;
	if (counter == 0) {
		speedNum = 300;
		tft.setCursor(260, 229);
		tft.print("EASY MODE");
	}
	if (counter == 1) {
		speedNum=100;
		tft.setCursor(250, 229);
		tft.print("MEDIUM MODE");
	}
	if (counter == 2) {
		speedNum=50;
		tft.setCursor(260, 229);
		tft.print("HARD MODE");
	}
	if (counter == 3) {
		speedNum=10;
		tft.setTextColor(ILI9341_BLUE);
		tft.setCursor(120, 229);
		tft.print("GOTTA GO FAST");
		tft.setTextColor(ILI9341_WHITE);
		tft.setCursor(255, 229);
		tft.print("SONIC MODE");
	}
	return speedNum;
}

// ===============================
// APPLE DRAWING HELPER FUNCTIONS
// ===============================

/* function that generates a random number using rand(), a library was added so that this could be used
the randomKey generated was mod by 5 since the cursor size of the snake and the movement increases by 5.With this the
apple was aligned with the snakes Movement. If the the randomKey % 5 did not equal 0 then the randomNum function
was recursively until a randomKey was found that was divisible by 5 */

uint32_t randomNum() {
	int randomKey = 0;
	//srand(millis());
 	randomKey = rand();
	if (randomKey % 5 == 0) {
		return randomKey;
	}
	else {
		randomNum();
	}
}

coordinates randomizeApple() {
	coordinates apple;

	uint32_t randomnumX = randomNum();
	uint32_t randomnumY = randomNum();

	apple.x = randomnumX % DISP_WIDTH;
	apple.y = randomnumY % (DISP_HEIGHT - 5);
	//FIXME idk but like the thing that happens when at the bottom of the screen
	// and the gameover condition thing i changed back so when u touch the bottom u still DIE
	// but i added - 5 to the apple.y thing so that the apple never draws at the bottom and i doubt
	// they will check trying to move across the bottom of the screen lol

	// after randomizing location return its coordinates
	return apple;
}

// =======================
// SNAKE HELPER FUNCTIONS
// =======================

// initialize the snake struct at the start of the game
// snake will initially move up with its head positioned at the origin
// and the rest of its body straight down vertically
void initSnake(int snakeLength) {
  snake[0].move = 'u';
	for (int i = 0; i < snakeLength; i++) {
		snake[i].x = DISP_WIDTH/2;
		snake[i].y = DISP_HEIGHT/2 + 5*i;
	}
}

// stores previous position of each snake part in a trailling
// array and then draws it so as to give the appearance of a
// snake moving without flashing/constant redrawing
void snakeTail(int trailX[], int trailY[], int snakeLength) {
  for (int i = 1; i < snakeLength; i++) {
    snake[i].x = trailX[i - 1];
    snake[i].y = trailY[i - 1];
    tft.fillRect(snake[i].x, snake[i].y, 5, 5, ILI9341_WHITE);
  }
}

/* Function processes snake drawing, movement and direction change(s).
	four different casess depending on the current diretion snake
	is moving. for each case, each position of the each part of the
	snake is stored into a 'trailing' array that follows the head
	as the snake's head coordinate is incremented, the snake's head
	is redrawn in its new position, followed by the trailing array
	which then draws the rest of the body. the tail is then erased.
	function returns the length of the snake so that tail is erased
	and updated every time snakelength increases.

	NOTE: joystick can only be tilted in one direction (i.e. horizontally
	or vertically). if tilted diagonally, direction will not change.
*/
int processSnake(int snakeLength, int speed) {
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);
	int trailX[snakeLength], trailY[snakeLength];

  // *** SNAKE MOVING IN UPWARD DIRECTION *** //
  if (snake[0].move == 'u') {
		// store snake trail/body following head
    for (int i = 0; i < snakeLength; i++) {
      trailX[i] = snake[i].x;
      trailY[i] = snake[i].y;
    }

    snake[0].y -= 5;
    tft.fillRect(snake[0].x, snake[0].y, 5, 5, ILI9341_WHITE);
    snakeTail(trailX, trailY, snakeLength);
    tft.fillRect(trailX[(snakeLength) - 1], trailY[(snakeLength) - 1], 5, 5, 0x0000);

    // change movement according to direction
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

  // *** SNAKE MOVING IN DOWNWARD DIRECTION *** //
  else if (snake[0].move == 'd') {
    for (int i = 0; i < snakeLength; i++) {
      trailX[i] = snake[i].x;
      trailY[i] = snake[i].y;
    }

    snake[0].y += 5;
    tft.fillRect(snake[0].x, snake[0].y, 5, 5, ILI9341_WHITE);
    snakeTail(trailX, trailY, snakeLength);
    tft.fillRect(trailX[(snakeLength)-1], trailY[(snakeLength)-1], 5, 5, 0x0000);

    // change movement according to direction
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
			}
		}

  	delay(speed);
}

// *** SNAKE MOVING IN LEFTWARD DIRECTION *** //
if (snake[0].move == 'l') {
  for (int i = 0; i < snakeLength; i++) {
    trailX[i] = snake[i].x;
    trailY[i] = snake[i].y;
  }

  snake[0].x -= 5;
  tft.fillRect(snake[0].x, snake[0].y, 5, 5, ILI9341_WHITE);
  snakeTail(trailX, trailY, snakeLength);
  tft.fillRect(trailX[(snakeLength)-1], trailY[snakeLength-1], 5, 5, 0x0000);

  // change movement according to direction
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

  // *** SNAKE MOVING IN RIGHTWARD DIRECTION *** //
  if (snake[0].move == 'r') {
    for (int i = 0; i < snakeLength; i++) {
      trailX[i] = snake[i].x;
      trailY[i] = snake[i].y;
    }

    snake[0].x += 5;
    tft.fillRect(snake[0].x, snake[0].y, 5, 5, ILI9341_WHITE);
    snakeTail(trailX, trailY, snakeLength);
    tft.fillRect(trailX[snakeLength-1], trailY[(snakeLength)-1], 5, 5, 0x0000);

    // change movement according to direction
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

// checks gameover conditions. if snake collides with any edge on the
// LCD display (i.e. top, bottom, leftmost, rightmost) then the game ends.
// if the snake head collides with any part of itself/body, then the game
// also ends.
void checkGameOver() {
	if (snake[0].y >= DISP_HEIGHT || snake[0].y < 0) {
		gameOver();
	}

	if (snake[0].x > DISP_WIDTH || snake[0].x < 0) {
		gameOver();
	}

	for (int i = 1; i < snakeLength; i++) {
		if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
			gameOver();
		}
	}
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
