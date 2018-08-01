// Assignment 2 Part 2: Restaurant Finder

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>
#include "lcd_image.h"

#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6
#define YP A2
#define XM A3
#define YM 5
#define XP 4
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
#define MINPRESSURE 10
#define MAXPRESSURE 1000

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
RestCache cache;

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

int touchY, touchX;
int ratingCursor, selectedRatingOld, sortedRating;
int selectedRating = 1;
int tracker = 0;
uint16_t sortedRatingIndex[30];
int previousHighlightedName, nextHighlightedName;
int page = 0;
int endIndex;

// forward declaration functions
void redrawCursor(uint16_t colour);
void change0to1();
void drawRating(int check, int ratingCursor1);

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
	lcd_image_draw(&yegImage, &tft, yegMiddleXNew, yegMiddleYNew,
                 0, 0, DISPLAY_WIDTH - 48, DISPLAY_HEIGHT);

  // initial cursor position is the middle of the screen
  oldCursorX = (DISPLAY_WIDTH - 48)/2;
  oldCursorY = DISPLAY_HEIGHT/2;
  cursorX = (DISPLAY_WIDTH - 48)/2;
  cursorY = DISPLAY_HEIGHT/2;

  redrawCursor(ILI9341_RED);
	cache.cachedBlock = 0;
}
// touch_coordinates lec 13
void checkTouch(){
	int topCircle = 17;
	int bottomCircle = 53;
	TSPoint touch = ts.getPoint();
	if (touch.z < MINPRESSURE || touch.z > MAXPRESSURE){
		return;
	}
	touchY = map(touch.x, TS_MINX, TS_MAXX, 0, DISPLAY_HEIGHT - 1);
	touchX = map(touch.y, TS_MINY, TS_MAXY, DISPLAY_WIDTH - 1, 0);
	if (touchX > DISPLAY_WIDTH - 42 && touchX < DISPLAY_WIDTH - 6){
		if (touchY > topCircle && touchY < bottomCircle){
			selectedRating = 5;
		}
		else if (touchY > (topCircle + 44) && touchY < (bottomCircle + 44)){
			selectedRating = 4;
		}
		else if (touchY > (topCircle + 88) && touchY < (bottomCircle + 88)){
			selectedRating = 3;
		}
		else if (touchY > (topCircle + 132) && touchY < (bottomCircle + 132)){
			selectedRating = 2;
		}
		else if (touchY > (topCircle + 176) && touchY < (bottomCircle + 176)){
			selectedRating = 1;
		}

	}
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
	checkTouch();
	int counter = 5;
	if (selectedRating != selectedRatingOld) {
		for(int index = 0; index < 5; index++){
			ratingCursor = (44*index)+ 24;
			drawRating(counter, ratingCursor);
			tft.print(counter);
			counter -= 1;
		}
		selectedRatingOld = selectedRating;
	}
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

void getRestaurantFast(restaurant* ptr, int i, Sd2Card* card, RestCache* cache) {
	uint32_t block = REST_START_BLOCK + i/8;
	if (block != cache->cachedBlock) {
		while (!card->readBlock(block, (uint8_t*) cache->block)) {
			Serial.print("readblock failed, try again");
		}
		cache->cachedBlock = block;
	}
	*ptr = cache->block[i%8];

}

// Swap two restaurants of RestDist struct
void swap_rest(RestDist& ptr_rest1, RestDist& ptr_rest2) {
  RestDist tmp = ptr_rest1;
  ptr_rest1 = ptr_rest2;
  ptr_rest2 = tmp;
}
// void swap_rest(RestDist *ptr_rest1, RestDist *ptr_rest2) {
//   RestDist tmp = *ptr_rest1;
//   *ptr_rest1 = *ptr_rest2;
//   *ptr_rest2 = tmp;
// }

// Selection sort
// rest_dist is an array of RestDist, from rest_dist[0] to rest_dist[len-1]
// void ssort(RestDist *rest_dist, int len) {
//   for (int i = len-1; i >= 1; --i) {
//     // Find the index of furthest restaurant
//     int max_idx = 0;
//     for (int j = 1; j <= i; ++j) {
//       if(rest_dist[j].dist > rest_dist[max_idx].dist) {
//         max_idx = j;
//       }
//     }
//     // Swap it with the last element of the sub-array
//     swap_rest(&rest_dist[i], &rest_dist[max_idx]);
//   }
// }

int pivot(RestDist rest_dist[], int n, int pi){
  swap_rest(rest_dist[pi], rest_dist[n-1]);
  int lo = 0;
  int hi = n - 2;

  while (hi >=lo){
    if (rest_dist[lo].dist <= rest_dist[n - 1].dist){
      lo += 1;
    }
    else if (rest_dist[hi].dist > rest_dist[n - 1].dist){
      hi -= 1;
    }
    else {
      swap_rest(rest_dist[lo], rest_dist[hi]);
    }
  }
  swap_rest(rest_dist[lo], rest_dist[n - 1]);
  return lo;
}

void qsort(RestDist rest_dist[], int n){
  if (n <= 1){
    return;
  }
  int pivotMid = (n - 1) / 2;
  int newPivot = pivot(rest_dist, n, pivotMid);
  qsort(rest_dist, newPivot);
  qsort(rest_dist + newPivot + 1, n - 1 - newPivot);
}

void drawRating(int check, int ratingCursor1){
	tft.setTextSize(3);
	tft.drawCircle(DISPLAY_WIDTH - 24, ratingCursor1 + 11, 18, ILI9341_RED);
	tft.setCursor(DISPLAY_WIDTH - 31, ratingCursor1);
	if (check >= selectedRating){
		tft.fillCircle(DISPLAY_WIDTH - 24, ratingCursor1 + 11, 17, ILI9341_WHITE);
		tft.setTextColor(ILI9341_BLACK);
	}
	else {
		tft.fillCircle(DISPLAY_WIDTH - 24, ratingCursor1 + 11, 17, ILI9341_BLACK);
		tft.setTextColor(ILI9341_WHITE);
	}

}

void drawName(int index) {
	tft.setTextSize(1);
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
  page = 0;
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
  qsort(rest_dist, NUM_RESTAURANTS);
  tft.fillScreen(ILI9341_BLACK);
  // will draw the closest 30 restaurants
	int c = 0;
	tracker = 0;
  while (true){
		getRestaurantFast(rest_dist[c].index, &allRestInfo);
		sortedRating = max(floor((allRestInfo.rating + 1)/2), 1);
		if (sortedRating >= selectedRating){
			drawName(tracker);
			sortedRatingIndex[tracker] = c;
			tracker += 1;
		}
		c += 1;
		if (tracker > 29){
			break;
		}
  }

}

void change1to0(){
  tft.fillScreen(ILI9341_BLACK);
  modeNum = 0;
  getRestaurantFast(rest_dist[nextHighlightedName].index, &allRestInfo);
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
	int counter = 5;
	for(int index = 0; index < 5; index++){
		ratingCursor = (44*index)+ 24;
		drawRating(counter, ratingCursor);
		tft.print(counter);
		counter -= 1;
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
    previousHighlightedName = sortedRatingIndex[highlightedName + 1];
    nextHighlightedName = sortedRatingIndex[highlightedName];
    // wrap around case for 0'th restaurant to 29'th restaurant
    if (highlightedName == -1 && page != 0){
      // // should change 0 index restaurant to black background
      // getRestaurantFast(rest_dist[previousHighlightedName].index, &allRestInfo);
      // drawName(highlightedName + 1);
      // // changes restaurant 29 to white background
      // highlightedName = 29;
      // getRestaurantFast(rest_dist[nextHighlightedName].index, &allRestInfo);
      // drawName(highlightedName);
      tft.fillScreen(ILI9341_BLACK);
      int k = sortedRatingIndex[highlightedName + 1] - 1;
      tracker = 29;
      highlightedName = 29;
      page -= 1;
      while (true){
        getRestaurantFast(rest_dist[k].index, &allRestInfo);
        sortedRating = max(floor((allRestInfo.rating + 1)/2), 1);
        Serial.println(sortedRating);
        if (sortedRating >= selectedRating){
          drawName(tracker);
          sortedRatingIndex[tracker] = k;
          tracker -= 1;
        }
        k -= 1;
        if (tracker < 0){
          break;
        }
      }
    }
    else if (highlightedName == -1 && page == 0){
      highlightedName = 0;
    }
    else {
      // should unhighlight previous above restaurant and highlight next one below
      getRestaurantFast(rest_dist[previousHighlightedName].index, &allRestInfo);
      drawName(highlightedName + 1);
      getRestaurantFast(rest_dist[nextHighlightedName].index, &allRestInfo);
      drawName(highlightedName);
    }
  }
  // checks if the joystick has been moved up
  if (yValMode1 > JOY_CENTER + 3*JOY_DEADZONE){
    highlightedName += 1;
    previousHighlightedName = sortedRatingIndex[highlightedName - 1];
    nextHighlightedName = sortedRatingIndex[highlightedName];
    // wrap around case from 29'th restaurant to 0'th restaurant
    if (highlightedName == 30){
      // // change the 29'th restaurant to black background
      // getRestaurantFast(rest_dist[previousHighlightedName].index, &allRestInfo);
      // drawName(highlightedName - 1);
      // highlightedName = 0;
      // // change the 0'th restaurant to white background
      // getRestaurantFast(rest_dist[nextHighlightedName].index, &allRestInfo);
      // drawName(highlightedName);
      tft.fillScreen(ILI9341_BLACK);
      endIndex = sortedRatingIndex[highlightedName - 1] + 1;
      tracker = 0;
      highlightedName = 0;
      page += 1;
      while (true){
        getRestaurantFast(rest_dist[endIndex].index, &allRestInfo);
        sortedRating = max(floor((allRestInfo.rating + 1)/2), 1);
        if (sortedRating >= selectedRating){
          drawName(tracker);
          sortedRatingIndex[tracker] = endIndex;
          tracker += 1;
        }
        endIndex += 1;
        if (tracker > 29 || endIndex == NUM_RESTAURANTS){
          break;
        }
      }
    }
    else if (highlightedName == tracker && endIndex == NUM_RESTAURANTS){
      highlightedName = tracker - 1;
    }
    else {
      // unhighlight previous below restaurant and highlight next one above
    getRestaurantFast(rest_dist[previousHighlightedName].index, &allRestInfo);
    drawName(highlightedName - 1);
    getRestaurantFast(rest_dist[nextHighlightedName].index, &allRestInfo);
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
