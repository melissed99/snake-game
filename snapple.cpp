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

// Cursor size. For best results, use an odd number.
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

uint8_t foodX;
uint8_t foodY;

// void makeFood(){ //
//     foodX = (((rand() % DISP_WIDTH)*4)%DISP_WIDTH);
//     foodY = (((rand() % DISP_HEIGHT)*4)%DISP_HEIGHT);
//
// 		tft.fillRect(foodX, foodY, 10, 10, ILI9341_RED);
//
// }

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

uint32_t randomNum() {
	// creates unsigned 16-bit random private key
	// and returns it
	int analogPin = 1;
	int randomKey = 0;
	uint32_t pinArray[16];
	for (int i = 0; i < 16; ++i) {
		pinArray[i] = analogRead((analogPin));
		randomKey += pinArray[i];
		delay(50);
	}
	return randomKey;
}

struct coordinates {
	uint32_t y;
	uint32_t x;
};

coordinates random_apple() {
	int i = 1;
	coordinates apple;
	uint32_t randomnumX = randomNum();
	uint32_t randomnumY = randomNum();

	apple.x = randomnumX % DISP_WIDTH;
	apple.y = randomnumY % DISP_HEIGHT;

	// apple.x = ((randomnumX % DISP_WIDTH) *5) %DISP_WIDTH;
	// apple.y = ((randomnumY % DISP_HEIGHT) *5) % DISP_HEIGHT;
	//
	// to avoid the apple going to the walls and out of bounds
	// if (apple.x <= 0 || apple.x >= DISP_WIDTH) {
	// 	apple.x = ((rand() %124) * 4) % 124;
	// }
	//
	// if (apple.y <= 0 || apple.y >= DISP_HEIGHT) {
	// 	apple.y = ((rand() %144) *4) % 144;
	// }
	return apple;
}



void game() {
  tft.fillScreen(ILI9341_BLACK);

  initSnake();
	//makeFood();//


	//choosing the position of the apple
	coordinates choose_apple = random_apple();
	//Snake begin = head[];


  delay(20);


  while (true) {
    processJoystick();
		// int buttonVal = digitalRead(JOY_SEL);
    // if (buttonVal == 0) {
    //   startPage();
    //   // draw the initial cursor
    // }

		tft.fillRect(choose_apple.x, choose_apple.y, CURSOR_SIZE, CURSOR_SIZE, ILI9341_CYAN);
		Serial.print("choose_apple.x: ");
		Serial.println(choose_apple.x);
		Serial.print("snakehead.x: " );
		Serial.print(cursorX);
		// Serial.print("choose_apple.y: ");
		// Serial.println(choose_apple.y);
		// Serial.print("snakehead.y: " );
		// Serial.print(cursorY);

		// if (foodX == head[0].x && foodY == head[0].y) { //
		// 	//length = length + 4;
		// 	//choose_apple = random_apple();
		// 	//score++;
		// 	makeFood();
		// }


		//eat apple
		//if ((choose_apple.x >= cursorX) && (choose_apple.x+1<= cursorX) && (choose_apple.y >= cursorY) && (choose_apple.y+1 <= cursorY)) {
		if (choose_apple.x == cursorX){
			tft.fillScreen(ILI9341_RED);
			//length = length + 4;
			// 3int score = 0;
			//choose_apple = random_apple();
			////////tft.fillRect(choose_apple.x, choose_apple.y, 8, 8, ILI9341_CYAN);
			// score +1;
			// Serial.print(score);
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
