/*
	Demonstrating cursor movement over the map of Edmonton. You will improve over
  what we do in the next weekly exercise.
*/

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_ILI9341.h>
#include "lcd_image.h"

#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240
#define YEG_SIZE 2048

lcd_image_t yegImage = { "yeg-big.lcd", YEG_SIZE, YEG_SIZE };

#define JOY_VERT  A1 // should connect A1 to pin VRx
#define JOY_HORIZ A0 // should connect A0 to pin VRy
#define JOY_SEL   2

#define JOY_CENTER   512
#define JOY_DEADZONE 64

#define CURSOR_SIZE 9

// the cursor position on the display
int oldCursorX, oldCursorY;
int cursorX, cursorY;



// forward declaration for redrawing the cursor
void redrawCursor(uint16_t colour);

void setup() {
  init();

  Serial.begin(9600);

	pinMode(JOY_SEL, INPUT_PULLUP);

	tft.begin();

	Serial.print("Initializing SD card...");
	if (!SD.begin(SD_CS)) {
		Serial.println("failed! Is it inserted properly?");
		while (true) {}
	}
	Serial.println("OK!");

	tft.setRotation(3);

  tft.fillScreen(ILI9341_BLACK);

  // draws the centre of the Edmonton map, leaving the rightmost 48 columns black
	int yegMiddleX = YEG_SIZE/2 - (DISPLAY_WIDTH - 48)/2;
	int yegMiddleY = YEG_SIZE/2 - DISPLAY_HEIGHT/2;
	lcd_image_draw(&yegImage, &tft, yegMiddleX, yegMiddleY,
                 0, 0, DISPLAY_WIDTH - 48, DISPLAY_HEIGHT);

  // initial cursor position is the middle of the screen
  oldCursorX = (DISPLAY_WIDTH - 48)/2;
  oldCursorY = DISPLAY_HEIGHT/2;
  cursorX = (DISPLAY_WIDTH - 48)/2;
  cursorY = DISPLAY_HEIGHT/2;

  redrawCursor(ILI9341_RED);
}

void redrawCursor(uint16_t colour) {
  // Limit the range of the cursor so that the cursor will not go past the borders
  // See README file for explanation as to why the number is 49 below
  cursorX = constrain(cursorX, 0 + CURSOR_SIZE/2, DISPLAY_WIDTH - CURSOR_SIZE/2 - 49);
  cursorY = constrain(cursorY, 0 + CURSOR_SIZE/2, DISPLAY_HEIGHT - CURSOR_SIZE/2);
  tft.fillRect(cursorX - CURSOR_SIZE/2, cursorY - CURSOR_SIZE/2,
               CURSOR_SIZE, CURSOR_SIZE, colour);
}

void processJoystick() {
  // newx > < oldx

  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);
  int buttonVal = digitalRead(JOY_SEL);
  int yegMiddleX = YEG_SIZE/2 - (DISPLAY_WIDTH - 48)/2;
	int yegMiddleY = YEG_SIZE/2 - DISPLAY_HEIGHT/2;
  int compareJoySubtract = JOY_CENTER - JOY_DEADZONE;
  int compareJoyAddition = JOY_CENTER + JOY_DEADZONE;

  oldCursorX = constrain(oldCursorX, 0 + CURSOR_SIZE/2, DISPLAY_WIDTH - CURSOR_SIZE/2 - 49);
  oldCursorY = constrain(oldCursorY, 0 + CURSOR_SIZE/2, DISPLAY_HEIGHT - CURSOR_SIZE/2);



  // now move the cursor
  // the larger multiplication will change the cursorY value more because
  // this indicates that the joystick was moved more
  if (yVal < JOY_CENTER - 7*JOY_DEADZONE){
    cursorY -= 5;
  }
  else if (yVal > JOY_CENTER + 7*JOY_DEADZONE){
    cursorY += 5;
  }
  else if (yVal < JOY_CENTER - 5*JOY_DEADZONE){
    cursorY -= 3;
  }
  else if (yVal > JOY_CENTER + 5*JOY_DEADZONE){
    cursorY += 3;
  }
  else if (yVal < compareJoySubtract) {
    cursorY -= 1; // decrease the y coordinate of the cursor
  }
  else if (yVal > compareJoyAddition) {
    cursorY += 1;
  }

  // remember the x-reading increases as we push left
  // same reason for multiplication for x
  if (xVal < (JOY_CENTER - 7*JOY_DEADZONE)){
    cursorX += 5;
  }
  else if (xVal > (JOY_CENTER + 7*JOY_DEADZONE)){
    cursorX -= 5;
  }
  else if (xVal < (JOY_CENTER - 5*JOY_DEADZONE)){
    cursorX += 3;
  }
  else if (xVal > (JOY_CENTER + 5*JOY_DEADZONE)){
    cursorX -= 3;
  }
  else if (xVal > compareJoyAddition) {
    cursorX -= 1;
  }
  else if (xVal < compareJoySubtract) {
    cursorX += 1;
  }

  // this statement will check if the old cursorX or cursorY has changed compared
  // to the "new" cursorX or cursorY value. If there is no change in the values
  // then the cursor will not blink
  if (oldCursorX != cursorX || oldCursorY != cursorY){
    // this will draw the specific square of the map that the cursor "moves over"
    lcd_image_draw(&yegImage, &tft, yegMiddleX + oldCursorX - CURSOR_SIZE/2,
                  yegMiddleY + oldCursorY - CURSOR_SIZE/2,
                  oldCursorX - CURSOR_SIZE/2, oldCursorY - CURSOR_SIZE/2,
                  CURSOR_SIZE, CURSOR_SIZE);

      // once the square of the map is redrawn, we assign the "new" cursor values
      // to the old values
      oldCursorX = cursorX;
      oldCursorY = cursorY;
    }

  redrawCursor(ILI9341_RED);

  delay(20);

}

int main() {
	setup();

  while (true) {
    processJoystick();
  }

	Serial.end();
	return 0;
}
