/*

*/

#include <Arduino.h>

void setup() {
	init();
	Serial.begin(9600);
}

void readString(char str[], int len) {
	/*
	  YOUR TASK:
		- Read characters from Serial, one at a time, until '\r' is encountered.
		- Add the characters to str[], one at a time, while reading.
		- Once done, add the null terminator '\0' to the end of the string so far.
		- Stop reading characters if you already read len-1 of them. Just return
		  the string constructed so far (with '\0' added to the end).

		Tip: use "break" to break out of a loop.
		example:

		while (true) {
		  if (something) {
			  break; // will exit the loop and resume just below the loop's closing brace
		  }
	  }
		EXECUTION CONTINUES FROM HERE AFTER THE "break".
	*/
	// for (int ind = 0; ind < len-1; ind++ ) {
	// 	if(Serial.available() > 0){ }
	// 		char byteRead = Serial.read();
	//
	//
	// 		if ((int) byteRead == 13) {
	// 			str[ind] = 0;
	// 			break;
	// 		}
	// 		else {
	// 			str[ind] = byteRead;
	// 	}
	// }

	int index = 0;
	while (index < len - 1) {
		// if something is waiting to be read
		if (Serial.available() > 0) {
			char byteRead = Serial.read();
			if (byteRead == '\r') {
				break;
			}
			else {
				str[index] = byteRead;
				index += 1;
			}
		}
	}
	str[index] = '\0';
}


// read an interger from the serial monitor
// reads characters until enter is pressed
int readInteger() {
	char str[32];
	readString(str, 32);
	return atoi(str);
}

int main() {
	setup();



	while (true) {
		int number = readInteger();

		Serial.print("I read integer: ")
		Serial.println(number);
	}

	// a value from -128 to 127
	int8_t byte = 123;

	// a vlaue from 0 to 255
	uint8_t uByte = 255;

	// what will this print?
	Serial.print(uByte+1);
	// this would be 0, because the oveflow "wraps around"
	// the sequence 0, 1 ,2, ...., 255

	// from -2^15 tp 2^15-1
	int16_t anInt = -1234;
	// from 0 to 2^16-1
	uint16_t anUnsignedInt = 42345;

	int32_t aLong = 123456;


	uint32_t aUnLong = 300000000;


	/*
		Alternative to Serial.flush().
		Main difference: the serial port is no longer available, you have to call
		Serial.begin(9600) again if you want to use it. Useful if you want to use
		the TX0/RX0 pins for something else, but we never have to.
	*/
	Serial.end();

	return 0;
}
