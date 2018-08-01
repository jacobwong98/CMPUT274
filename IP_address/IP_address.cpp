/*
	IPv4 Address
*/

#include <Arduino.h>

void setup() {
	init();
	Serial.begin(9600);
}

void readString(char str[], int len) {
	int index = 0;
	Serial.print("Enter the unsigned 32 bit representation of the IPv4 address: ");
	while (index < len - 1) {
		if (Serial.available() > 0) {
			char byteRead = Serial.read();
			Serial.print(byteRead);
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

uint32_t readUnsigned32() {
	char str[32];
	readString(str, 32);
	return atol(str);
}



uint8_t getOctet(uint32_t ipAddress, uint8_t octet) {
	uint8_t octetNumber = octet * 8;
	uint8_t selectedOctet = ipAddress >> octetNumber;
	return selectedOctet;

}

void getOctetTest(uint32_t ipAddress, uint8_t octet, uint8_t expected)
{
	uint8_t calculated = getOctet(ipAddress, octet);
	if (calculated != expected) {
		Serial.println("Error in getOctet test");
		Serial.print("Expected: ");
		Serial.println(expected);
		Serial.print("Got: ");
		Serial.println(calculated);
	}
}

void testGetOctet() {
	Serial.println("Starting tests");
	getOctetTest(1, 0, 1);
	getOctetTest(1, 1, 0);
	getOctetTest(1060985650, 3, 63);
	getOctetTest(4294901244, 2, 254);
	getOctetTest(4278456321, 1, 16);
	getOctetTest(4281726382, 4, 0);
	getOctetTest(33554431, 3, 1);
	getOctetTest(8589934591, 2, 255);
	getOctetTest(3619714131, 3, 215);
	Serial.println("Tests done!");
}



int main() {
	setup();
	testGetOctet();

	while (true) {
		uint32_t enteredIP = readUnsigned32();
		Serial.println();
		for (int i = 0; i < 4; i++) {
			uint8_t octetSeries = getOctet(enteredIP, i);
			Serial.print("Octet ");
			Serial.print(i);
			Serial.print(": ");
			Serial.println(octetSeries);
			Serial.println();
		}
	}


	Serial.flush();
	return 0;
}
