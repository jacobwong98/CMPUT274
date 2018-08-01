// Student name: Jacob Wong
// CCID: jdw2
// Student ID: 1511712
#include <Arduino.h>
#include "string_processing.h"

// this function will be used to calculate the length of a string
int getStringLength(const char* str){
	int i = 0;

	while(true){
		// will break the infinite while loop when the null terminator has been
		// reached indicating the end of the given string
		if (str[i] == '\0'){
			break;
		}

		i += 1;
	}
	return i;
}

// this function will be used to print the given string backwards
void printBackward(const char* str){
	int sizeString = getStringLength(str);
	while (sizeString >= 0){
		Serial.print(str[sizeString]);
		sizeString -=  1;
	}
	Serial.println();
}

// this function will test whether getStringLength will give the correct length
void getStringLengthTest(const char* a, int expectedNum) {
	int calculatedNum = getStringLength(a);

	if (calculatedNum != expectedNum) {
		Serial.println("error in getStringLength test");
		Serial.print("expected: ");
		Serial.println(expectedNum);
		Serial.print("got: ");
		Serial.println(calculatedNum);
	}

}

// this function calls getStringLengthTest with some example tests
void testGetStringLength(){
	Serial.println("Starting getStringLength Tests");
	getStringLengthTest("hello",5);
	getStringLengthTest("qwertyuiop12345678", 18);
	getStringLengthTest("abcd\0abababababsbdhdfhfsnhsjajskvnakjskdfkjasf", 4);
	getStringLengthTest("838s8d9c&3728", 13);
	getStringLengthTest(" ", 1);
	getStringLengthTest("_+_)@&#**&^@*", 13);
	Serial.println("Tests are done!");
}
