/*
	Talking arduino example.
*/

#include <Arduino.h>

const int idPin = 8;

void setup() {
	init();

	pinMode(idPin, INPUT);

	Serial.begin(9600);
	Serial3.begin(9600);
}

void sender() {
	Serial.println("I am the sender");

	Serial3.println("hello");
	Serial3.println("is this working?");
}

void receiver() {
	Serial.println("I am the receiver");
	// wait until some data has arrived
	while (true) {
		// wait until the sender has sent some data
		while (Serial3.available() == 0) { }
		char byteRead = Serial3.read();

			// the sender sends \r\n with println()
			Serial.print(byteRead);


	}
}

int main() {
	setup();

	if (digitalRead(idPin) == LOW){
		sender();
	}
	else {
		receiver();
	}

	// TASK:
	// remove the if/else part above
	// create your own unencrypted chat.
	// have an infinite loop (while(true)) that, in each iteration,
	// checks if something is available from Serial and print it
	// to Serial3, and also if something is available from Serial3
	// and print it to Serial

	Serial3.flush();
	Serial.flush();

	return 0;
}
