void game() {
  tft.fillScreen(ILI9341_BLACK);

  initSnake();

  tft.println(snake[0].x);
  tft.println(snake[0].y);
  tft.print(snake[0].move);

  delay(20);

  // draw apple
  tft.fillRect(DISPLAY_WIDTH/4, DISPLAY_HEIGHT/4, 5, 5, ILI9341_BLUE);
  // randomize this location later
  redrawCursor(cursorX/2, cursorY/2, cursorX/4, cursorY/4);


  while (true) {
    moveSnake();
		int buttonVal = digitalRead(JOY_SEL);
    if (buttonVal == 0) {
      startPage();
      // draw the initial cursor
    }
  }

}

void moveUp() {
  int length = 5;
  int lastX[length];
  int lastY[length];
  for (int i = 0; i < length; i++) {
     lastX[i] = snake[i].x;
     lastY[i] = snake[i].y;
  }

  // decrease y coordinate of snake
  snake[0].y -= 5;
  //draw snake
for (int i = 0; i < 5; i++){	tft.fillRect(snake[i].x, snake[i].y, 5, 5, 0xFFFF);
  delay(500);
  // erase tail
  if (snake[0].y == 0) {
    tft.fillRect(snake[0].x, snake[0].y, 5, 5, ILI9341_RED);
    //tft.fillRect(lastX[(length)-1], lastY[(length)-1], 5, 5, ILI9341_RED);
  }
  else {
    tft.fillRect(snake[i].x, snake[i].y, 5, 5, 0x0000);
    //tft.fillRect(lastX[(length)-1], lastY[(length)-1], 5, 5, ILI9341_BLACK);
  }}

}

void moveSnake() {
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);

  // copy the cursor position (to check later if it changed)
  int oldX = cursorX;
  int oldY = cursorY;

  // move cursor, change direction?
  // cursorX += (JOY_CENTER - xVal) / JOY_SPEED;
  // cursorY += (yVal - JOY_CENTER) / JOY_SPEED;
  if (yVal < JOY_CENTER - JOY_DEADZONE) { // joystick UP
    cursorY -= 2; // decrease the y coordinate of the cursor //
  }
  else if (yVal > JOY_CENTER + JOY_DEADZONE) { // joystick DOWN
    cursorY += 2; // increase the y coordinate of the cursor
  }

  if (xVal > JOY_CENTER + JOY_DEADZONE) {
    cursorX -= 1; // remember the x-reading increases as we push left
  }
  else if (xVal < JOY_CENTER - JOY_DEADZONE) {
    cursorX += 1;
  }

  // constrain so the cursor does not go off of the map display window
  cursorX = constrain(cursorX, 0, DISPLAY_WIDTH - CURSOR_SIZE/2);
  cursorY = constrain(cursorY, 0, DISPLAY_HEIGHT - CURSOR_SIZE/2);

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

  if (snake[0].move == 0) {
    //moveDown();
  }

  else if (snake[0].move == 1) {
    moveUp();
    //oldY += 1;
    //redrawCursor(cursorX, cursorY, oldX, oldY);
    //cursorY += 2;

  }

  // else if (snake[0].move == 2) {
  //   moveLeft();
  // }
  //
  // else if (snake[0].move == 3) {
  //   moveRight();
  // }

  delay(50);
}

int main() {
	setup();

  while (true) {
    moveSnake();
  }

	Serial.end();
	return 0;
}
