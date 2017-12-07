# snake-game
Final Project for CMPUT 274
Snake Game on Arduino

Team Members: Raymond Sarinas (1476504), Melisse Doroteo (1499913)

The Arduino and the display will be wired the exact same as how we did it
for Assignment 2.

Accessories:
1 x Arduino Mega Board (AMG)
1 x Joystick
1x LCD touchscreen

Wiring instructions:
For screen: from bottom to top on the right side (the side with the pins).

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

For joystick:
Joystick    Board/Arduino
GND        GND (board bus)
5V        5V (board bus)
VRX        Analog Pin 1
VRY        Analog Pin 0
SW        Digital Pin 2

How to run code:
Upload file into terminal by first typing in "make upload"

How to play:
First select the level of difficulty to play where each level varies in the speed of the snake. The objective of the game
is for the snake to eat the apple. For each apple eaten the more points and the longer the snake. However, if the snake comes into contact with its own body or any of the walls then game over.

NOTES
- There is a rare occurrence where the apple will generate over the snakes body. Since the snake moves over the body, it will
print a black square onto the apple and the apple can no longer be seen. This issue could not be fixed in time.


Milestone 1: ✔️ COMPLETED NOV 28
- Draw and move around snake (prevent diagonal movement?) ✔️
- Draw an apple. ✔️
- Draw startup and game-over screens. ✔️

Milestone 2: ✔️ COMPLETED NOV 29
- Randomize location of apple each time eaten. ✔️
- When snake eats apple, make snake longer. ✔️

Milestone 3: ✔️ COMPLETED DEC 1
- Endgame conditions if snake hits corner/edge of screen. ✔️
- Endgame conditions if snake hits itself. ✔️
- Draw and record score with new boundaries. ✔️
- Implement different levels of difficulty (i.e. varying snake speed)✔️

Milestone 4: SORT OF COMPLETED DEC 6 ✔️
- Draw special apple that appears on screen for only a few seconds ✔️
- Fix flashing snaketail issue/bug ✔️
- Fix apple drawing over body issue/bug
- Make screen graphics look nicer? idk maybe

Bonus Milestone:
- Implement Deque for Snake Movement

SUBMIT CODE BY DEC 7/8
- Clean up code
- Add comments
- Make README

TO DO:
- [] implement deque for snake movement
- [] fix bug where apple could potentially be drawn underneath snake
- [x] fix bug where apple just sometimes does not draw at all?
- [x] flashing bug fixed (but not really)
- [] clean code / delete extra code, change variable names to make more sense
- [] add comments, make README
