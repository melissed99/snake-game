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

#define CURSOR_SIZE 6

// smaller numbers yield faster cursor movement
#define JOY_SPEED 64

// the cursor position on the display, stored as the middle pixel of the cursor
int cursorX, cursorY;

// forward declaration for drawing the cursor
void redrawCursor(int newX, int newY, int oldX, int oldY);
void startPage();
void game();
void processJoystick();

// Use hardware SPI (on Mega2560, #52, #51, and #50) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


struct Snake {
  int x, y;
  char direction;
  // up = u, down = d, left = l, right = r
};

Snake head[100];

void initSnake() {
  head[0].x = DISP_WIDTH/2;
  head[0].y = DISP_HEIGHT/2;
  head[0].direction = 'u';
}

void game() {
  tft.fillScreen(ILI9341_BLACK);

  initSnake();

  tft.println(head[0].x);
  tft.println(head[0].y);
  //tft.print(head[0].direction);

  delay(20);

  // draw apple
  tft.fillRect(DISP_WIDTH/4, DISP_HEIGHT/4, 5, 5, ILI9341_BLUE);
  redrawCursor(DISP_WIDTH/2, DISP_HEIGHT/2, DISP_WIDTH/2, DISP_HEIGHT/2);


  while (true) {
    processJoystick();
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
  tft.fillRect(newX, newY, CURSOR_SIZE, CURSOR_SIZE, ILI9341_WHITE);
}


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

void gameOver(){

	tft.fillScreen(0x07E0);

	tft.setCursor(30, 20);
	tft.setTextColor(ILI9341_RED);
	tft.setTextSize(5);
	tft.print("GAME OVER");

	tft.fillRect(DISP_WIDTH/2 - 95, 115, 190, 60, ILI9341_WHITE);
	tft.drawRect(DISP_WIDTH/2 - 95, 115, 190, 60, ILI9341_BLACK);
	tft.setCursor(DISP_WIDTH/2 - 85, 125 );
	tft.setTextColor(ILI9341_RED);
	tft.setTextSize(6);
	tft.print("RESET");

	while(true){
		checkTouchGameOver();
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

	tft.fillScreen(ILI9341_BLACK);

	startPage();
}

void processJoystick() {
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);

  // copy the cursor position (to check later if it changed)
  int oldX = cursorX;
  int oldY = cursorY;

  // move the cursor, further pushes mean faster movement
  cursorX += (JOY_CENTER - xVal) / JOY_SPEED;
  cursorY += (yVal - JOY_CENTER) / JOY_SPEED;

  // constrain so the cursor does not go off of the map display window
  cursorX = constrain(cursorX, 0, DISP_WIDTH - CURSOR_SIZE);
  cursorY = constrain(cursorY, 0, DISP_HEIGHT - CURSOR_SIZE);

  // redraw the cursor only if its position actually changed
  if (cursorX != oldX || cursorY != oldY) {
    redrawCursor(cursorX, cursorY, oldX, oldY);
  }

  // constrain and prevent diagonal movement
  if (cursorY != oldY) {
      cursorX = oldX;
    }
  else if(cursorX != oldX) {
    cursorY = oldY;
  }

  delay(50);
}


int main() {
	setup();
	//gameOver();
	startPage();

	while (true) {
    processJoystick();
  }

	Serial.end();
	return 0;
}
