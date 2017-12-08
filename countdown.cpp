void countdown() {

	tft.fillScreen(ILI9341_BLACK);
	tft.setCursor(DISP_WIDTH/2 - 70-2, (DISP_HEIGHT + 20)/2 - 30);
	tft.fillRect(DISP_WIDTH/2, DISP_HEIGHT/2 + 10, 1, 1, ILI9341_GREEN);
	tft.setTextSize(5);
	tft.print("READY");
	delay(1000);
	tft.setCursor(DISP_WIDTH/2 - 50+5, (DISP_HEIGHT + 20)/2 - 30);
	tft.fillScreen(ILI9341_BLACK);
	tft.print("SET");
	tft.fillRect(DISP_WIDTH/2, DISP_HEIGHT/2 + 10, 1, 1, ILI9341_GREEN);
	delay(1000);
	tft.setCursor(DISP_WIDTH/2 - 50+5, (DISP_HEIGHT + 20)/2 - 30);
	tft.fillScreen(ILI9341_BLACK);
	tft.print("GO!");
	tft.fillRect(DISP_WIDTH/2, DISP_HEIGHT/2 + 10, 1, 1, ILI9341_GREEN);
	delay(1000);
}
