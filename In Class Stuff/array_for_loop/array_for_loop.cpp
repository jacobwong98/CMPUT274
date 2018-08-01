#include <Arduino.h>

int scopeVar = 7;

void setup() {
	init();
	Serial.begin(9600);
}

// prints all contents of an array in reverse order
// if we know the size of the array
// a -= b will compute a-b and store it in a
void printArrayReverse(int someArray[3]) {
	for (int index = 2; index >= 0; index -= 1) {
		Serial.println(someArray[index]);
	}
}

// prints the even-index items in the array
// from 0 up to size - 1
// we don't know the size, so we expect it to be
// supplied as an argument
// ASSUMES: someArray has at least "size" entries
void printArrayBy2(int someArray[], int size) {
	for (int index = 0; index < size; index += 2){
		Serial.println(someArray[index]);
	}
}

// changes both array[0] and var to new values
// var is a copy of the value that was passed
// array is actually a reference to the location
//of the origianl integers
void changeValueTest(int array[]. int var) {
	array[0] += 1;
	var += 1;
}


int main(){
	setup();

	int values[10];
	// now array is not an "int", it is an array of "int"
	values[3] = 5;
	Serial.println(values[3]);

	// the entries of the array are values[0], .... values[9]

	int values2[] = {1, -4, 5};
	// values2 is an array of size 3, initialized with
	// 17, -4, 5
	Serial.println(values2[0]);
	Serial.println(values2[1]);
	Serial.println(values2[2]);
	// Serial.println(values2[3]); 3 is not a valid index
	// Serial.println(values2[-15]);

	Serial.println();

	values2[values2[0]] = 7;
	// Serial.println(values2[0]);
	// Serial.println(values2[1]);
	// Serial.println(values2[2]);

	Serial.println("for loop example");
	// for (initialization; condition; update) {}
	for (int ind = 0; ind < 3; ind++) {
		Serial.println(values2[ind]);
	}
	// what will be output?

	// Serial.println(ind);


	Serial.println("while loop example");
	int ind2 = 0;
	while (ind2 < 3) {
		Serial.println(values2[ind2]);

		// update at the end of the body of the loop
		ind2 = ind2+1;
	}

	Serial.println("ind2 value ");
	Serial.println(ind2);

	Serial.println("scope example");
	for (int ind3 = 0; ind3 < 5; ind3++) {
		int ind2 = ind3*2;
		Serial.println(ind2);

	}

	// the previous ind2 is not gone, it was just not
	// visible in the for loop because we defined
	// ind2 in there as well
	Serial.print("old ind2 ");
	Serial.println(ind2);

	Serial.println("complicated scope example");
	Serial.print("global: ");
	Serial.println(scopeVar);
	if (true){
		int scopeVar = 1;
		Serial.print("in the if block: ");
		Serial.println(scopeVar);
		if (true){
			int scopeVar = 4;
			Serial.print("in the nested if block: ");
			Serial.println(scopeVar);
		}
		if (true){
			Serial.print("in the second nested if: ");
			Serial.println(scopeVar);
		}
		Serial.print("back in the first if: ");
		Serial.println(scopeVar);
	}
	Serial.print("and the global again: ");
	Serial.println(scopeVar);

	Serial.println();

	Serial.println("function call example");
	printArrayReverse(values2);


	// char str[] = {'a', 'b', 'c', 'd', 'e'};

	// an array of characters
	char str[] = "abcde";
	Serial.println("function call example 2")
	printArrayBy2(str,5);





	// flushes and ends serial port communications
	// have the Serial.begin(9600) again again if you still
	// want to use the serial port
	Serial.end();
	return 0;
}
