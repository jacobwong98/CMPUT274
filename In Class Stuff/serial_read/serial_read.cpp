#include <Arduino.h>

void setup() {
	init();
	Serial.begin(9600);
}

int someFunction(int a) {
	return 2*a;
}

int someFunction(char x) {
	return x;
}

int main() {
	setup();

	// Serial.read() returns an int, which is
	// converted to a byteRead
	// as long as the int returned is as large as a byte,
	// no problem!
	// char byteRead = Serial.read();

	// // keep rereading from the Serial port until
	// // something was actually recieved
	// while (byteRead == -1) {
	// 	byteRead = Serial.read();
	// }

	// wait until some data has arrived
	while (true) {
		while (Serial.available() == 0) { }
			char byteRead = Serial.read();


			// Serial.print("Recieved: ");
			// Serial.println((int) byteRead);

			// TASK: have the serial monitor go to the start
			// of the next line when enter is pressed
			// in the serial monitor
			if ((int) byteRead == 13) {
				Serial.println();
			}
			else {
				Serial.print(byteRead);
			}
	}
	Serial.flush();

	return 0;
}
