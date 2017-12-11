# snake-game
Final Project for CMPUT 274
Snake Game on Arduino

Team Members: Raymond Sarinas (1476504), Melisse Doroteo (1499913)

Game Description:
Our project aims to recreate a common video game, Snake. The player controls a
moving rectangle that can only move in 4 directions (up, down, left, or right).
As the rectangle moves forward, it leaves a trail behind it which resembles the
movement of a snake. The goal of the game is to get the snake to "consume" as
many "apples" (red squares) that are randomly placed onto the screen in which
the snake is moving in. Eating each apple increments the player's score, and as
more apples are consumed, the length and trail of the snake gets longer making
it more difficult to control and navigate around the plane to eat more apples.
The game ends if the the snake runs/hits any of the edges of the screen, or if
the "head" or front end of the snake runs into any other part of its self/trail.

Sources/References:
- Used our assignment 2 part 1 solution as base code to develop this game.
- Other documents such as that for the touchscreen testing, and randomizing numbers
  were also used as reference to develop this game from Zac's Google Drive.

Accessories:
1 x Arduino Mega Board (AMG)
1 x Joystick
1x LCD touchscreen

Wiring Instructions:
The Arduino and the display will be wired the exact same as how we did it
for Assignment 2.

For Screen: from bottom to top on the right side (the side with the pins).

Board Pin <---> Arduino Pin
===========================
GND             GND
Vin             5V
3Vo             NOT CONNECTED
CLK             52
MISO            50
MOSI            51
CS              10
D/C             9
RST             NOT CONNECTED
Lite            NOT CONNECTED
Y+              A2 (analog pin)
X+              4  (digital pin)
Y-              5  (digital pin)
X-              A3 (analog pin)
IM0 - IM3       NOT CONNECTED (they expect a 3.3v signal, DON'T CONNECT!)
CCS             6
CD              NOT CONNECTED

For Joystick:

Joystick <---> Board/Arduino
==============================
GND            GND (board bus)
5V             5V (board bus)
VRX            Analog Pin 1
VRY            Analog Pin 0
SW             Digital Pin 2

How to Run Code:
Upload file into terminal by first typing in "make upload"

How to Play:
First select the level of difficulty to play where each level varies in the
speed of the snake. The objective of the game is for the snake to eat the apple.
For each apple eaten the more points and the longer the snake. However, if the
snake comes into contact with its own body or any of the walls then game over.

NOTES (Including Unresolved Bugs)
- There is a rare occurrence where the apple will generate over the snakes body.
  Since the snake moves over the body, it will print a black square onto the
  apple and the apple can no longer be seen. This issue could not be fixed in time.
- Because the snake's coordinates moves down the screen in increments of 5, when
  the snake reaches the bottom of the screen, just above the displayed score/mode,
  the snake cannot move along the bottom screen unlike on all of the other screen
  edges. If the player attempts to move the snake along the bottom edge of the
  screen, the game will end. This issue could not be resolved on time due to
  likely having to modify too many different aspects of the game.

MILESTONES COMPLETED TO CREATE GAME
Milestone 1: ✔️ COMPLETED NOV 24
- [x] Draw and move around snake.
- [x] Draw startup and game-over screens.

Milestone 2: ✔️ COMPLETED NOV 29
- [x] Draw an apple on the screen.
- [x] Randomize location of apple each time eaten.
- [x] When snake eats apple, make snake longer.

Milestone 3: ✔️ COMPLETED DEC 3
- [x] Implement Endgame conditions.
- [x] Draw and record score with new boundaries.
- [x] Implement different levels of difficulty (i.e. varying snake speed).
- [ ] Fix various bugs that might have arisen.

Milestone 4: ✔️ COMPLETED DEC 7
- [x] Clean and organize code.
- [x] Add comments to code and complete README.

BONUS MILESTONE: ❌ INCOMPLETE
- [x] Draw special apple that appears on screen for only a few seconds.
- [x] Implement deque to be used for snake movement.
- [x] Make screen graphics look nicer?
