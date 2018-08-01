#include <Arduino.h>

void setup() {
	init();
	Serial.begin(9600);
}

int main(){
	setup();

	Serial.println("hello world!");
	Serial.println("second message");

	// an example of using a string literal variable
	const char* message = "string literal";
	Serial.println(message);

	// single quotes for a character literal
	// char character = "a"; ERROR
	// char character = 'dskjfbskdbf'; works??
	char character = 'z';
	Serial.println(character);

	Serial.print("goto\r\nnewline\r\n");
	Serial.print("\"double quotes\"");
	Serial.println();

	// carriage return + linefeed examples

	Serial.println("carriage\rreturn");
	Serial.println("line\nfeed");

	int numBytes = Serial.print("abcd");
	Serial.println(numBytes);

	numBytes = Serial.println("hi");
	Serial.println(numBytes);


	// wait until the Serial port has printed all
	// "buffered" messages
	Serial.flush();

	return 0;
}
