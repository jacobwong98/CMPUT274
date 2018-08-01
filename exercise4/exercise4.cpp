/*
	Exercise 4: String Functions
	Student name: Jacob Wong
	CCID: jdw2
	Student ID; 1511712

	This Arduino program is used to calculate the length of a given string
	and print the message backwards. This program also shows how to use
	a header file to "combine" two seperate C++ files to organize our codes
	more cleanly.
*/
#include <Arduino.h>
#include "string_processing.h"

void setup(){
	init();
	Serial.begin(9600);
}

int main(){
	setup();
	// calls the function that tests getStringLength 
	testGetStringLength();
	// example of printBackward function
	Serial.print("hello backwards is : ");
	printBackward("hello");

	Serial.print("abcd(null terminator)abababababsbdhdfhfsn backwards is : ");
	printBackward("abcd\0abababababsbdhdfhfsn");

	Serial.print("_+_)@&#**&^@* backwards is : ");
	printBackward("_+_)@&#**&^@*");

	Serial.end();
	return 0;
}
