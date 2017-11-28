// SNAKE GAME ON ARDUINO

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_ILI9341.h>

#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// physical dimensions of the tft display (# of pixels)
#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240

// width and height (in pixels) of the LCD image
#define LCD_WIDTH 2048
#define LCD_HEIGHT 2048

#define JOY_VERT  A1 // should connect A1 to pin VRx
#define JOY_HORIZ A0 // should connect A0 to pin VRy
#define JOY_SEL   2

#define JOY_CENTER   512
#define JOY_DEADZONE 64

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

void setup() {
  init();

  Serial.begin(9600);

  // not actually used in this exercise, but it's ok to leave it
	pinMode(JOY_SEL, INPUT_PULLUP);

	tft.begin();
	tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);

  startPage();

  while(true) {
		int buttonVal = digitalRead(JOY_SEL);

		if (buttonVal == 0) {
			game();
      // draw the initial cursor
    }
	}

}

void startPage() {
tft.fillScreen(0x8811); // 0x8811
tft.setCursor(DISPLAY_WIDTH/2 - 45, 30);
tft.setTextColor(ILI9341_WHITE);
tft.setTextSize(3);
tft.println("SNAKE");
tft.setTextSize(2);
tft.println(" ");
tft.setTextSize(1);
tft.setCursor(DISPLAY_WIDTH/2 - 65, 55);
tft.println("touch start to begin!");
}

struct Snake {
  int x, y;
  char direction;
  // up = u, down = d, left = l, right = r
};

Snake head[100];

void initSnake() {
  head[0].x = DISPLAY_WIDTH/2;
  head[0].y = DISPLAY_HEIGHT/2;
  head[0].direction = 'u';
}

void game() {
  tft.fillScreen(ILI9341_BLACK);

  initSnake();

  tft.println(head[0].x);
  tft.println(head[0].y);
  tft.print(head[0].direction);

  delay(20);

  // draw apple
  tft.fillRect(DISPLAY_WIDTH/4, DISPLAY_HEIGHT/4, 5, 5, ILI9341_BLUE);
  redrawCursor(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2, DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2);


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
  cursorX = constrain(cursorX, 0, DISPLAY_WIDTH - CURSOR_SIZE);
  cursorY = constrain(cursorY, 0, DISPLAY_HEIGHT - CURSOR_SIZE);

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

  while (true) {
    processJoystick();
  }

	Serial.end();
	return 0;
}
