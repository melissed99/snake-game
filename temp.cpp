#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <SD.h>
#include <TouchScreen.h>

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
#define JOY_DEADZONE 400

#define JOY_CENTER 500
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

// smaller numbers yield faster cursor movement
#define JOY_SPEED 64

// the cursor position on the display, stored as the middle pixel of the cursor
int cursorX, cursorY;

// forward declaration for drawing the cursor
void redrawCursor(int newX, int newY, int oldX, int oldY);
void startPage();
void game();
void moveSnake();

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

	if ((p.x>=(DISP_WIDTH/2 - 95)) && (p.x<=255) && (p.y>=115) && (p.y<=175) ) {
		startPage();
		return;
	}
}

void gameOver() {

	tft.fillScreen(0x07E0);

	tft.setCursor(30, 20);
	tft.setTextColor(ILI9341_RED);
	tft.setTextSize(5);
	tft.print("GAME OVER");

	tft.fillRect(DISP_WIDTH/2 - 95, 115, 190, 60, ILI9341_BLACK);
	tft.drawRect(DISP_WIDTH/2 - 95, 115, 190, 60, ILI9341_BLACK);
	tft.setCursor(DISP_WIDTH/2 - 85, 125 );
	tft.setTextColor(ILI9341_WHITE);
	tft.setTextSize(6);
	tft.print("RESET");

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

	//tft.fillScreen(ILI9341_BLACK);
}

struct snakeStruct {
  int x, y;
  char move;
  // up = u, down = d, left = l, right = r
};

snakeStruct snake[100];

void initSnake() {
  snake[0].x = DISP_WIDTH/2;
  snake[0].y = DISP_HEIGHT/2;
  snake[0].move = 'u';
}

void game() {
  tft.fillScreen(ILI9341_BLACK);

  initSnake();
  delay(20);

  // draw apple
  // randomize this location later.
  tft.fillRect(DISP_WIDTH/4, DISP_HEIGHT/4, 5, 5, ILI9341_BLUE);

  // draw initial cursor? snake head? IDK
  //redrawCursor(DISP_WIDTH/2, DISP_HEIGHT/2, DISP_WIDTH/2, DISP_HEIGHT/2);


  while (true) {
    moveSnake();
		int buttonVal = digitalRead(JOY_SEL);
    if (buttonVal == 0) {
      startPage();
      // draw the initial cursor
    }
  }

}

// redraws the patch of edmonton over the older cursor position
// given by "oldX, oldY" and draws the cursor at its new position
// given by "newX, newY"
void redrawCursor(int newX, int newY, int oldX, int oldY) {
  // and now draw the cursor at the new position
  tft.fillRect(oldX, oldY, CURSOR_SIZE, CURSOR_SIZE, ILI9341_BLACK);
  tft.fillRect(newX, newY, CURSOR_SIZE, CURSOR_SIZE, ILI9341_WHITE);
}



void movingSnake() {

    int snakeLength = 1;
		int tempX[snakeLength];
		int tempY[snakeLength];
		int xVal = analogRead(JOY_HORIZ);
	  int yVal = analogRead(JOY_VERT);
		int oldX = xVal;
		int oldY = yVal;

      // constrain and prevent diagonal movement
      if (yVal != oldY) {
          xVal = oldX;
        }
      else if(xVal != oldX) {
        yVal = oldY;
      }

		// MOVING UP
		if (snake[0].move == 'u') {
			for (int i = 0; i < snakeLength; i++) {
				tempX[i] = snake[i].x;
				tempY[i] = snake[i].y;
			}

			snake[0].y -= 5;
			tft.fillRect(snake[0].x, snake[0].y, 5, 5, 0xFFFF);
			tft.fillRect(tempX[(snakeLength) - 1], tempY[(snakeLength) - 1], 5, 5, 0x0000);

			// if change direction to the LEFT
			if ((xVal < JOY_CENTER - JOY_DEADZONE) && yVal == oldY) {
				snake[0].move = 'r';
			}

			else if ((xVal > JOY_CENTER + JOY_DEADZONE) && yVal == oldY) {
				snake[0].move = 'l';
				tft.fillRect(tempX[(snakeLength)-1], tempY[(snakeLength)-1], 5, 5, 0x0000);
			}
		delay(500);
		} // END MOVE UP

		// MOVING DOWN
		else if (snake[0].move == 'd') {
			for (int i = 0; i < snakeLength; i++) {
				tempX[i] = snake[i].x;
				tempY[i] = snake[i].y;
			}

			snake[0].y += 5;
			tft.fillRect(snake[0].x, snake[0].y, 5, 5, 0xFFFF);
			tft.fillRect(tempX[(snakeLength)-1], tempY[(snakeLength)-1], 5, 5, 0x0000);

			// if change direction to the LEFT
			if ((xVal < JOY_CENTER - JOY_DEADZONE) && yVal == oldY) {
				snake[0].move = 'r';
			}

			else if ((xVal > JOY_CENTER + JOY_DEADZONE) && yVal == oldY) {
				snake[0].move = 'l';
				tft.fillRect(tempX[(snakeLength)-1], tempY[(snakeLength)-1], 5, 5, 0x0000);
			}
		delay(500);
	} // END MOVE DOWN

	// MOVING LEFT
	if (snake[0].move == 'l') {
		for (int i = 0; i < snakeLength; i++) {
			tempX[i] = snake[i].x;
			tempY[i] = snake[i].y;
		}

		snake[0].x -= 5;
		tft.fillRect(snake[0].x, snake[0].y, 5, 5, 0xFFFF);
		tft.fillRect(tempX[(snakeLength)-1], tempY[snakeLength-1], 5, 5, 0x0000);

		// if change direction to the UP
		if ((yVal < JOY_CENTER - JOY_DEADZONE) && xVal == oldX) {
			snake[0].move = 'u';
		}

		// change go DOWN
		else if ((yVal > JOY_CENTER + JOY_DEADZONE) && xVal == oldX) {
			snake[0].move = 'd';
			//tft.fillRect(tempX[(snakeLength)-1], tempY[(snakeLength)-1], 5, 5, 0x0000);
		}
	delay(500);
		} // END MOVE LEFT

		// MOVING RIGHT
		if (snake[0].move == 'r') {
			for (int i = 0; i < snakeLength; i++) {
				tempX[i] = snake[i].x;
				tempY[i] = snake[i].y;
			}

			snake[0].x += 5;
			tft.fillRect(snake[0].x, snake[0].y, 5, 5, 0xFFFF);
			tft.fillRect(tempX[snakeLength-1], tempY[(snakeLength)-1], 5, 5, 0x0000);

			// if change direction to the UP
			if ((yVal < JOY_CENTER - JOY_DEADZONE) && xVal == oldX) {
				snake[0].move = 'u';
			}

			// change go DOWN
			else if ((yVal > JOY_CENTER + JOY_DEADZONE) && xVal == oldX) {
				snake[0].move = 'd';
				//tft.fillRect(tempX[(snakeLength)], tempY[(snakeLength)-1], 5, 5, 0x0000);
			}
		delay(500);
	} // END MOVE RIGHT

} // END SNAKEGAME


void moveSnake() {
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);

  // copy the cursor position (to check later if it changed)
  int oldX = cursorX;
  int oldY = cursorY;

  // constrain so the cursor does not go off of the map display window
  cursorX = constrain(cursorX, 0, DISP_WIDTH - CURSOR_SIZE);
  cursorY = constrain(cursorY, 0, DISP_HEIGHT - CURSOR_SIZE);

  while (true) {
    movingSnake();

    // GAME OVER CONDITIONS
    if (snake[0].y > DISP_HEIGHT - CURSOR_SIZE/2 || snake[0].y < 0) {
      gameOver();
    }
    if (snake[0].x > DISP_WIDTH - CURSOR_SIZE/2 || snake[0].x < CURSOR_SIZE) {
      gameOver();
    }

    /*
    for (int i = 1; i < snakeLength; i++) {
      if (snake[0].x == snake[i].x || snake[0].y == snake[i].y) {
        gameOver();
      }
    }
    */
  }


  delay(50);
}


int main() {
	setup();
	//gameOver();
	startPage();

	while (true) {
    moveSnake();
  }

	Serial.end();
	return 0;
}
