#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include "snake.h"

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

coordinates randomizeApple() {

	coordinates apple;

	uint32_t randomnumX = randomNum();
	uint32_t randomnumY = randomNum();

	apple.x = randomnumX % DISP_WIDTH;
	apple.y = randomnumY % DISP_HEIGHT;

	return apple;
}


void initSnake(int snakeLength) {
  snake[0].move = 'u';
	for (int i = 0; i < snakeLength; i++) {
		snake[i].x = DISP_WIDTH/2;
		snake[i].y = DISP_HEIGHT/2 + 5*i;
	}
}

int snakeTail(int tempX[], int tempY[], int snakeLength) {
  for (int i = 1; i < snakeLength; i++) {
    snake[i].x = tempX[i-1];
    snake[i].y = tempY[i-1];
    tft.fillRect(snake[i].x, snake[i].y, 5, 5, 0xFFFF);
  }
}

int processSnake(int snakeLength, int speed) {
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);
  // copy the joystick orientation(?)
  int oldX = xVal; //cursorX;
  int oldY = yVal; //cursorY;

	/**** PROCESS SNAKE MOVEMENT AND DRAWING/REDRAWING ****/
	int tempX[snakeLength], tempY[snakeLength];

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
		if (xVal < JOY_CENTER - JOY_DEADZONE) {
			if (yVal < JOY_CENTER - JOY_DEADZONE || yVal > JOY_CENTER + JOY_DEADZONE) {
				snake[0].move = 'u';
			}
      else {
				snakeTail(tempX, tempY, snakeLength);
      	snake[0].move = 'r';
			}
    }
    else if (xVal > JOY_CENTER + JOY_DEADZONE) {
			if (yVal < JOY_CENTER - JOY_DEADZONE || yVal > JOY_CENTER + JOY_DEADZONE) {
				snake[0].move = 'u';
			}
      else {
				snakeTail(tempX, tempY, snakeLength);
      	snake[0].move = 'l';
			}
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
			if (yVal < JOY_CENTER - JOY_DEADZONE || yVal > JOY_CENTER + JOY_DEADZONE) {
				snake[0].move = 'd';
			}
			else {
				snakeTail(tempX, tempY, snakeLength);
				snake[0].move = 'r';
			}
		}
		else if ((xVal > JOY_CENTER + JOY_DEADZONE) && yVal == oldY) {
			if (yVal < JOY_CENTER - JOY_DEADZONE || yVal > JOY_CENTER + JOY_DEADZONE) {
				snake[0].move = 'd';
			}
			else {
				snakeTail(tempX, tempY, snakeLength);
				snake[0].move = 'l';
				//tft.fillRect(tempX[(snakeLength)-1], tempY[(snakeLength)-1], 5, 5, 0x0000);
			}
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
		if (xVal < JOY_CENTER - JOY_DEADZONE || xVal > JOY_CENTER + JOY_DEADZONE) {
			snake[0].move = 'l';
		}
		else {
    	snakeTail(tempX, tempY, snakeLength);
    	snake[0].move = 'u';
		}
	}
  else if ((yVal > JOY_CENTER + JOY_DEADZONE) && xVal == oldX) {
		if (xVal < JOY_CENTER - JOY_DEADZONE || xVal > JOY_CENTER + JOY_DEADZONE) {
			snake[0].move = 'l';
		}
		else {
    	snakeTail(tempX, tempY, snakeLength);
    	snake[0].move = 'd';
		}
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
			if (xVal < JOY_CENTER - JOY_DEADZONE || xVal > JOY_CENTER + JOY_DEADZONE) {
				snake[0].move = 'r';
			}
			else {
	    	snakeTail(tempX, tempY, snakeLength);
	    	snake[0].move = 'u';
			}
		}
	  else if ((yVal > JOY_CENTER + JOY_DEADZONE) && xVal == oldX) {
			if (xVal < JOY_CENTER - JOY_DEADZONE || xVal > JOY_CENTER + JOY_DEADZONE) {
				snake[0].move = 'r';
			}
			else {
	    	snakeTail(tempX, tempY, snakeLength);
	    	snake[0].move = 'd';
			}
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
	return snakeLength;
}
