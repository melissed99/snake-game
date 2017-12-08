#include "touchscreen.h"

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
		counter=2;
		game();
	}

	//SONIC
	if((p.y >= 5) && (p.y <= 100) && (p.x >= 190) && (p.x <= 280)){
		counter = 3;
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

	if ((p.y >= (DISP_WIDTH/2 - 125)) && (p.y <= 180) && (p.x >= 100) && (p.x <= 200) ) {
		startPage();
		return;
	}
}

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
