/*
	A simple demonstration of drawing a section of the large map of Edmonton (yeg-big.lcd),
	and then scrolling it over one "screen".
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


#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

#define YEG_SIZE 2048

lcd_image_t yegImage = {"yeg-big.lcd" , YEG_SIZE, YEG_SIZE};

void setup(){
	init();
	tft.begin();

	Serial.begin(9600);

	Serial.print("initializing SD card...");
	if (!SD.begin(SD_CS)){
		while (true){}
	}
	Serial.print("OK!");


	tft.setRotation(3);

	// black is 0, i.e. 0 == tft.color565(0,0,0)
	tft.fillScreen(0);

}

int main (){
	setup();

	int yegMiddleX = YEG_SIZE/2 - DISPLAY_WIDTH/2;
	int yegMiddleY = YEG_SIZE/2 - DISPLAY_HEIGHT/2;

	// &yegImage is a pointer to the location (in memory)
	// of yegImage
	lcd_image_draw(&yegImage, &tft,
								// upper left corner in the image to draw
								yegMiddleX, yegMiddleY,
								// upper left corner of the screen to draw in
								0, 0,
								// width and height of the patch of the image to draw
								DISPLAY_WIDTH - 64, DISPLAY_HEIGHT);

	delay(3000);
	// once screen down from above
	lcd_image_draw(&yegImage, &tft,
								// upper left corner in the image to draw
								yegMiddleX, yegMiddleY + DISPLAY_HEIGHT/2,
								// upper left corner of the screen to draw in
								0, 0,
								// width and height of the patch of the image to draw
								DISPLAY_WIDTH - 64, DISPLAY_HEIGHT);

	Serial.end();
	return 0;
}
