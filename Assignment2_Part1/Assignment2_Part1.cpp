// Assignment 2 Part 1: Restaurant Finder

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
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
#define JOY_DEADZONE 48

#define CURSOR_SIZE 9

// These constants are for the 2048 by 2048 map.
#define MAP_WIDTH 2048
#define MAP_HEIGHT 2048
#define LAT_NORTH 5361858l
#define LAT_SOUTH 5340953l
#define LON_WEST -11368652l
#define LON_EAST -11333496l

#define REST_START_BLOCK 4000000
#define NUM_RESTAURANTS 1066

struct restaurant {
  int32_t lat;
  int32_t lon;
  uint8_t rating; // from 0 to 10
  char name[55];
};

Sd2Card card;

struct RestDist {
  uint16_t index; // index of restaurant from 0 to NUM_RESTAURANTS-1
  uint16_t dist;  // Manhatten distance to cursor position
};

// initialize variable and structures used for caluculating Manhatten distance
RestDist rest_dist[NUM_RESTAURANTS];
restaurant restBlock1[8];
restaurant allRestInfo;
uint32_t restBlockNew  = 0;
int16_t xCursCoord;
int16_t yCursCoord;


// the cursor position on the display
int oldCursorX, oldCursorY;
int cursorX, cursorY;
int yegMiddleXNew = YEG_SIZE/2 - (DISPLAY_WIDTH - 48)/2;
int yegMiddleYNew = YEG_SIZE/2 - DISPLAY_HEIGHT/2;
int modeNum = 0;
int oldButtonVal = HIGH;
int oldButtonVal1 = HIGH;
int highlightedName = 0;




// forward declaration functions
void redrawCursor(uint16_t colour);
void change0to1();

void setup() {
  init();

  Serial.begin(9600);

	pinMode(JOY_SEL, INPUT_PULLUP);

	tft.begin();

  Serial.print("Initializing SPI communication for raw reads...");
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println("failed! Is the card inserted properly?");
    while (true) {}
  }
  else {
    Serial.println("OK!");
  }

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
	lcd_image_draw(&yegImage, &tft, yegMiddleXNew, yegMiddleYNew,
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


// this will all the functions that are required for moving the cursor around the
// map (aka Mode 0)
void mode0(){

	  int xVal = analogRead(JOY_HORIZ);
	  int yVal = analogRead(JOY_VERT);
	  int buttonVal = digitalRead(JOY_SEL);
	  int compareJoySubtract = JOY_CENTER - JOY_DEADZONE;
	  int compareJoyAddition = JOY_CENTER + JOY_DEADZONE;

	  oldCursorX = constrain(oldCursorX, 0 + CURSOR_SIZE/2, DISPLAY_WIDTH - CURSOR_SIZE/2 - 49);
	  oldCursorY = constrain(oldCursorY, 0 + CURSOR_SIZE/2, DISPLAY_HEIGHT - CURSOR_SIZE/2);



	  // now move the cursor
	  // the larger multiplication will change the cursorY value more because
	  // this indicates that the joystick was moved more
	  if (yVal < JOY_CENTER - 7*JOY_DEADZONE){
	    cursorY -= 7;
	  }
	  else if (yVal > JOY_CENTER + 7*JOY_DEADZONE){
	    cursorY += 7;
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
	    cursorX += 7;
	  }
	  else if (xVal > (JOY_CENTER + 7*JOY_DEADZONE)){
	    cursorX -= 7;
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
	    lcd_image_draw(&yegImage, &tft, yegMiddleXNew + oldCursorX - CURSOR_SIZE/2,
	                  yegMiddleYNew + oldCursorY - CURSOR_SIZE/2,
	                  oldCursorX - CURSOR_SIZE/2, oldCursorY - CURSOR_SIZE/2,
	                  CURSOR_SIZE, CURSOR_SIZE);

	      // once the square of the map is redrawn, we assign the "new" cursor values
	      // to the old values
	      oldCursorX = cursorX;
	      oldCursorY = cursorY;
	    }

	  redrawCursor(ILI9341_RED);

    // this will move the total map over one screen size to the left when the
    // cursor hits the left side of the screen
		if (cursorX == 0 + CURSOR_SIZE/2 && yegMiddleXNew > 0){
      yegMiddleXNew = yegMiddleXNew - DISPLAY_WIDTH;
      yegMiddleXNew = constrain(yegMiddleXNew, 0, YEG_SIZE - DISPLAY_WIDTH);

			lcd_image_draw(&yegImage, &tft, yegMiddleXNew, yegMiddleYNew,
                  	0, 0, DISPLAY_WIDTH - 48, DISPLAY_HEIGHT);
      // redraws the cursor on the centre of the display
			cursorX = (DISPLAY_WIDTH - 48)/2;
			oldCursorX = (DISPLAY_WIDTH - 48)/2;
      cursorY = DISPLAY_HEIGHT/2;
      oldCursorY = DISPLAY_HEIGHT/2;
			redrawCursor(ILI9341_RED);
		}
    // move the total map one screen size to the right when the cursor
    // hits the right side of the screen
		if (cursorX == DISPLAY_WIDTH - CURSOR_SIZE/2 - 49 && yegMiddleXNew < YEG_SIZE - DISPLAY_WIDTH){
      yegMiddleXNew = yegMiddleXNew + DISPLAY_WIDTH - 48;
      yegMiddleXNew = constrain(yegMiddleXNew, 0, YEG_SIZE - DISPLAY_WIDTH + 48);

		  lcd_image_draw(&yegImage, &tft, yegMiddleXNew, yegMiddleYNew,
		               0, 0, DISPLAY_WIDTH - 48, DISPLAY_HEIGHT);

			cursorX = (DISPLAY_WIDTH - 48)/2;
			oldCursorX = (DISPLAY_WIDTH - 48)/2;
      cursorY = DISPLAY_HEIGHT/2;
      oldCursorY = DISPLAY_HEIGHT/2;
			redrawCursor(ILI9341_RED);
		}
    // move the total map one screen size up when the cursor hits the top
    // of the display screen.
		if (cursorY == 0 + CURSOR_SIZE/2 && yegMiddleYNew > 0){
      yegMiddleYNew = yegMiddleYNew - DISPLAY_HEIGHT;
      yegMiddleYNew = constrain(yegMiddleYNew , 0, YEG_SIZE - DISPLAY_HEIGHT);

		  lcd_image_draw(&yegImage, &tft, yegMiddleXNew, yegMiddleYNew,
	                  0, 0, DISPLAY_WIDTH - 48, DISPLAY_HEIGHT);
			cursorY = DISPLAY_HEIGHT/2;
			oldCursorY = DISPLAY_HEIGHT/2;
      cursorX = (DISPLAY_WIDTH - 48)/2;
      oldCursorX = (DISPLAY_WIDTH - 48)/2;
			redrawCursor(ILI9341_RED);
		}
    // move the total map one screen down when the cursor hits the bottom
    // of the display screen.
		if (cursorY == DISPLAY_HEIGHT - CURSOR_SIZE/2 && yegMiddleYNew < YEG_SIZE - DISPLAY_HEIGHT){
      yegMiddleYNew = yegMiddleYNew + DISPLAY_HEIGHT;
      yegMiddleYNew = constrain(yegMiddleYNew, 0, YEG_SIZE - DISPLAY_HEIGHT);

			lcd_image_draw(&yegImage, &tft, yegMiddleXNew, yegMiddleYNew,
		                 0, 0, DISPLAY_WIDTH - 48, DISPLAY_HEIGHT);

			cursorY = DISPLAY_HEIGHT/2;
			oldCursorY = DISPLAY_HEIGHT/2;
      cursorX = (DISPLAY_WIDTH - 48)/2;
      oldCursorX = (DISPLAY_WIDTH - 48)/2;
			redrawCursor(ILI9341_RED);
		}
    // checks if the joystick has been pressed or not
    if (buttonVal != oldButtonVal && buttonVal == HIGH){
      // record the current X and Y position of the cursor relative to the YEG map
      xCursCoord = yegMiddleXNew + cursorX;
      yCursCoord = yegMiddleYNew + cursorY;
      change0to1();
    }
    oldButtonVal = buttonVal;
	  delay(20);
}


// These functions convert between x/y map position and lat/lon
// (and vice versa.)
int32_t x_to_lon(int16_t x) {
    return map(x, 0, MAP_WIDTH, LON_WEST, LON_EAST);
}

int32_t y_to_lat(int16_t y) {
    return map(y, 0, MAP_HEIGHT, LAT_NORTH, LAT_SOUTH);
}

int16_t lon_to_x(int32_t lon) {
    return map(lon, LON_WEST, LON_EAST, 0, MAP_WIDTH);
}

int16_t lat_to_y(int32_t lat) {
    return map(lat, LAT_NORTH, LAT_SOUTH, 0, MAP_HEIGHT);
}

void getRestaurantFast(int restIndex1, restaurant* restPtr1) {
  uint32_t blockNum1 = REST_START_BLOCK + restIndex1/8;

  // if the restaurant index (blockNum1 in a sense) is not within a
  // restaurant block (8 restaurants) then we want to call the next/previous block
  // and replace the current block
  if (blockNum1 != restBlockNew){

    while (!card.readBlock(blockNum1, (uint8_t*) restBlock1)) {
      Serial.println("Read block failed, trying again.");
    }
    // Assign the block number to the new block to keep track of which block
    // that we are currently in.
    restBlockNew = blockNum1;
  }
  *restPtr1 = restBlock1[restIndex1 % 8];

}

// Swap two restaurants of RestDist struct
void swap_rest(RestDist *ptr_rest1, RestDist *ptr_rest2) {
  RestDist tmp = *ptr_rest1;
  *ptr_rest1 = *ptr_rest2;
  *ptr_rest2 = tmp;
}

// Selection sort
// rest_dist is an array of RestDist, from rest_dist[0] to rest_dist[len-1]
void ssort(RestDist *rest_dist, int len) {
  for (int i = len-1; i >= 1; --i) {
    // Find the index of furthest restaurant
    int max_idx = 0;
    for (int j = 1; j <= i; ++j) {
      if(rest_dist[j].dist > rest_dist[max_idx].dist) {
        max_idx = j;
      }
    }
    // Swap it with the last element of the sub-array
    swap_rest(&rest_dist[i], &rest_dist[max_idx]);
  }
}

void drawName(int index) {

	tft.setCursor(0, 8*index);

	if (index == highlightedName) {
		tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
	}
	else {
		tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	}
  tft.println(allRestInfo.name);

}

// setup for changing from mode 0 to mode 1
void change0to1(){
  // reset the highlightedName to be the first index so when drawn it will
  // highlight the first restaurant
  highlightedName = 0;
  modeNum = 1;
  // will go through all the restaurants and calculate their Manhatten distances
  for (int i = 0; i < NUM_RESTAURANTS; i++){
    getRestaurantFast(i, &allRestInfo);
    rest_dist[i].index = i;
    int16_t xRestCoord = lon_to_x(allRestInfo.lon);
    int16_t yRestCoord = lat_to_y(allRestInfo.lat);
    rest_dist[i].dist = abs(xCursCoord - xRestCoord) + abs(yCursCoord - yRestCoord);
  }
  // Selection sort restaurants from closest distance to furthest distance from
  // the cursor position
  ssort(rest_dist, NUM_RESTAURANTS);
  tft.fillScreen(ILI9341_BLACK);
  // will draw the closest 30 restaurants
  for (int i = 0; i < 30; i++){
    getRestaurantFast(rest_dist[i].index, &allRestInfo);
      drawName(i);
  }
}

void change1to0(){
  tft.fillScreen(ILI9341_BLACK);
  modeNum = 0;
  getRestaurantFast(rest_dist[highlightedName].index, &allRestInfo);
  // change lat/lon of selected restaurant to y/x for comparision and mapping
  int16_t selectRestX = lon_to_x(allRestInfo.lon);
  int16_t selectRestY = lat_to_y(allRestInfo.lat);
  // will centre the selected restaurant on the middle of the display screen
  yegMiddleXNew = selectRestX - (DISPLAY_WIDTH - 48)/2 ;
  yegMiddleYNew = selectRestY - DISPLAY_HEIGHT/2;
  yegMiddleXNew = constrain(yegMiddleXNew, 0, YEG_SIZE - DISPLAY_WIDTH + 48);
  yegMiddleYNew = constrain(yegMiddleYNew, 0, YEG_SIZE - DISPLAY_HEIGHT);
  lcd_image_draw(&yegImage, &tft, yegMiddleXNew, yegMiddleYNew,
                0, 0, DISPLAY_WIDTH - 48, DISPLAY_HEIGHT);

  // checks if the x coordinate for selected restaurant is outside the lefthand side
  // of the YEG map
  if (selectRestX < 0){
    cursorX = CURSOR_SIZE/2;
  }
  // checks if the selected restaurant x coordinate is within half a display
  // screen width (excluding black bar) of the lefthand side of the YEG map
  else if (selectRestX > 0 && selectRestX < (DISPLAY_WIDTH - 48)/2 ){
    cursorX = selectRestX;
  }
  // checks if the selected restaurant x coordinate is outside the righthand side
  // of the YEG map
  else if (selectRestX > YEG_SIZE - 48){
    cursorX = DISPLAY_WIDTH - 48 - CURSOR_SIZE/2;
  }
  // checks is the selected restaurant x coordinate is within half a display screen
  // width (excluding black bar) of the righthand side of the YEG map
  else if (selectRestX < YEG_SIZE && selectRestX > yegMiddleXNew + (DISPLAY_WIDTH - 48)/2){
    cursorX = selectRestX - yegMiddleXNew;
  }
  // recenter cursorX for every other case not seen above.
  else {
    cursorX = (DISPLAY_WIDTH - 48)/2;
    oldCursorX = (DISPLAY_WIDTH - 48)/2;
  }

  // check if the selected restaurant Y coordinate is outside the top of YEG map
  if (selectRestY < 0){
    cursorY = CURSOR_SIZE/2;
  }
  // check if the selected restaurant Y coordinate is within half a screen height from
  // the top of the YEG map
  else if (selectRestY > 0 && selectRestY < DISPLAY_HEIGHT/2){
    cursorY = selectRestY;
  }
  // check if the selected restaurant Y coordinate is outside the bottom of YEG map
  else if (selectRestY > YEG_SIZE){
    cursorY = DISPLAY_HEIGHT - CURSOR_SIZE/2;
  }
  // check if the selected restaurant Y coordinate is within half a screen height from
  // the bottom of the YEG map
  else if (selectRestY < YEG_SIZE && selectRestY > yegMiddleYNew + DISPLAY_HEIGHT/2){
    cursorY = selectRestY - yegMiddleYNew;
  }
  // will recenter the cursorY for any other case not seen above
  else {
  cursorY = DISPLAY_HEIGHT/2;
  oldCursorY = DISPLAY_HEIGHT/2;
  }


  redrawCursor(ILI9341_RED);


}
// function for viewing restaurants and scrolling through them.
void mode1(){
  int yValMode1 = analogRead(JOY_VERT);
  int buttonVal1 = digitalRead(JOY_SEL);
  // checks if the joystick has been moved down
  if (yValMode1 < JOY_CENTER - 3*JOY_DEADZONE){
    highlightedName -= 1;
    // wrap around case for 0'th restaurant to 29'th restaurant
    if (highlightedName == -1){
      // should change 0 index restaurant to black background
      getRestaurantFast(rest_dist[highlightedName + 1].index, &allRestInfo);
      drawName(highlightedName + 1);
      // changes restaurant 29 to white background
      highlightedName = 29;
      getRestaurantFast(rest_dist[highlightedName].index, &allRestInfo);
      drawName(highlightedName);
    }
    else {
      // should unhighlight previous above restaurant and highlight next one below
      getRestaurantFast(rest_dist[highlightedName + 1].index, &allRestInfo);
      drawName(highlightedName + 1);
      getRestaurantFast(rest_dist[highlightedName].index, &allRestInfo);
      drawName(highlightedName);
    }
  }
  // checks if the joystick has been moved up
  if (yValMode1 > JOY_CENTER + 3*JOY_DEADZONE){
    highlightedName += 1;
    // wrap around case from 29'th restaurant to 0'th restaurant
    if (highlightedName == 30){
      // change the 29'th restaurant to black background
      getRestaurantFast(rest_dist[highlightedName - 1].index, &allRestInfo);
      drawName(highlightedName - 1);
      highlightedName = 0;
      // change the 0'th restaurant to white background
      getRestaurantFast(rest_dist[highlightedName].index, &allRestInfo);
      drawName(highlightedName);
    }
    else {
      // unhighlight previous below restaurant and highlight next one above
    getRestaurantFast(rest_dist[highlightedName - 1].index, &allRestInfo);
    drawName(highlightedName - 1);
    getRestaurantFast(rest_dist[highlightedName].index, &allRestInfo);
    drawName(highlightedName);
    }
  }
  //checks if the joystick was pressed
  if (buttonVal1 != oldButtonVal1 && buttonVal1 == HIGH){
    // setup for transition from mode 1 to mode 0
    change1to0();
  }
  oldButtonVal1 = buttonVal1;

  delay(20);
}


void processJoystick() {
  if (modeNum == 0){
	 mode0();
  }
  else {
    mode1();
  }
}

int main() {
	setup();

  while (true) {
    processJoystick();
  }

	Serial.end();
	return 0;
}
