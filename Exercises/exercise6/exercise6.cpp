// Exercise 6: Restaurants and Pointers

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>

#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6

#define REST_START_BLOCK 4000000
#define NUM_RESTAURANTS 1066

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// different than SD
Sd2Card card;

struct restaurant {
  int32_t lat;
  int32_t lon;
  uint8_t rating; // from 0 to 10
  char name[55];
};
restaurant restBlock1[8];
uint32_t restBlockNew  = 0;
uint32_t totalSlowTime = 0;
uint32_t totalFastTime = 0;


void setup() {
  init();
  Serial.begin(9600);

  // including this seems to fix some SD card readblock errors
  // (at least on the old display)
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);

  // The following initialization (commented out) is
  // not needed for just raw reads from the SD card, but you should add it later
  // when you bring the map picture back into your assignment
  // (both initializations are required for the assignment)

  // initialize the SD card
  /*
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed! Is the card inserted properly?");
    while (true) {}
  }
  else {
    Serial.println("OK!");
  }
  */

  // initialize SPI (serial peripheral interface)
  // communication between the Arduino and the SD controller

  Serial.print("Initializing SPI communication for raw reads...");
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println("failed! Is the card inserted properly?");
    while (true) {}
  }
  else {
    Serial.println("OK!");
  }
}


// read the restaurant at position "restIndex" from the card
// and store at the location pointed to by restPtr
void getRestaurant(int restIndex, restaurant* restPtr) {
  // calculate the block containing this restaurant
  uint32_t blockNum = REST_START_BLOCK + restIndex/8;
  restaurant restBlock[8];

  // Serial.println(blockNum);

  // fetch the block of restaurants containing the restaurant
  // with index "restIndex"
  while (!card.readBlock(blockNum, (uint8_t*) restBlock)) {
    Serial.println("Read block failed, trying again.");
  }
  // Serial.print("Loaded: ");
  // Serial.println(restBlock[0].name);

  *restPtr = restBlock[restIndex % 8];
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



int main() {
  setup();

  restaurant rest3;
  restaurant restFast;

  // Test 1: Calling all restaurants in increasing order
  Serial.println("Starting getRestaurant");
  uint32_t timerSlowStart = millis();
  for (int i = 0; i < NUM_RESTAURANTS; ++i) {
    getRestaurant(i, &rest3);
  }
  uint32_t timerSlowEnd = millis();
  totalSlowTime = timerSlowEnd - timerSlowStart;
  Serial.print("Total time for getRestaurant: ");
  Serial.println(totalSlowTime);

  Serial.println("Starting getRestaurantFast");
  uint32_t timerFastStart = millis();
  for (int c = 0; c < NUM_RESTAURANTS; ++c) {
    getRestaurantFast(c, &restFast);
  }
  uint32_t timerFastEnd = millis();
  totalFastTime = timerFastEnd - timerFastStart;
  Serial.print("Total time for getRestaurantFast: ");
  Serial.println(totalFastTime);



  // Test 2: Calling 20 restaurants in descending order
  Serial.println("Starting getRestaurant Reverse");
  uint32_t timerSlowStart1 = millis();
  for (int i = 20; i > 0; i--) {
    getRestaurant(i, &rest3);
  }
  uint32_t timerSlowEnd1 = millis();
  uint32_t totalSlowTime1 = timerSlowEnd1 - timerSlowStart1;
  Serial.print("Total time for getRestaurant Reverse: ");
  Serial.println(totalSlowTime1);

  Serial.println("Starting getRestaurantFast Reverse");
  uint32_t timerFastStart1 = millis();
  for (int c = 20; c > 0; c--) {
    getRestaurantFast(c, &restFast);
  }
  uint32_t timerFastEnd1 = millis();
  uint32_t totalFastTime1 = timerFastEnd1 - timerFastStart1;
  Serial.print("Total time for getRestaurantFast Reverse: ");
  Serial.println(totalFastTime1);


  Serial.end();

  return 0;
}
