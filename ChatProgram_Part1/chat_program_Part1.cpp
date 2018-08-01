/*
	Chat Program Part 1
*/

#include <Arduino.h>

int analogInPin = A1;
uint32_t bitPattern = 0;
uint32_t pattern[16];
uint32_t result;


void setup() {
	init();
	Serial.begin(9600);
	Serial3.begin(9600);

}

// this function is used to acquire a random private key
// by reading the voltage from analogue pin A1 and using the & operator
// to compare the bits of that number to 1 to obtain the
// least significant digit of the voltage
uint32_t getPrivateKey() {
	// repeat process 16 times to acquire a random 16 bit number
	for (int i = 0; i < 15; i++){
		uint32_t randomValue = analogRead(analogInPin);
		uint32_t bitPattern = randomValue & 1;
		// stores the first bit into the first position of the array
		if (i == 0){
			pattern[i] = bitPattern;
		}
		// manipulate the first position of the array by shifting the orignal
		// bit 1 bit to the left and adding the bit that we obtained
		// in the following iteration of the for loop
		else {
			pattern[0] = pattern[0] << 1;
			pattern[0] = pattern[0] + bitPattern;
		}
		delay(50);
	}
	return pattern[0];
}

// this function will calculate a times a and repeat b times
// while taking the mod of each intermediate step
uint32_t powMod (uint32_t a, uint32_t b, uint32_t m) {
	a = a%m;
	result = 1%m;
	for (uint32_t i = 0; i < b; i++) {
		result = (result * a) % m;
	}
	return result;
}

// in this function, it will read from the serial monitor the value
// of the other user's public key that the main user enters manually
void getOtherPublicKey(char str[], int len) {
	int index = 0;
	while (index < len-1){
		if (Serial.available() > 0) {

			char otherKey = Serial.read();
			// this function will terminate once the user presses the enter key
			if (otherKey == '\r') {
				Serial.println();
				break;
			}
			else {
				str[index] = otherKey;
				index += 1;
			}
		}
	}
	str[index] = '\0';

}

// this function will ask the user to enter the other user's public key
uint32_t readUnsigned32() {
	char str[32];
	Serial.print("Please enter other user's Public Key: ");
	getOtherPublicKey(str, 32);
	// once getOtherPublicKey(str, 32) it will produce the character array
	// str which is then converted into a unsigned long type using atol(str)
	return atol(str);
}

// this function is used to execute the Diffie Hellman process
uint32_t diffieHellman() {
	// agreed upon values that are used to calculate the personal public key
	const uint32_t p = 19211;
	const uint32_t g = 6;

	uint32_t myPrivateKey = getPrivateKey();
	uint32_t myPublicKey = powMod(g, myPrivateKey, p);

	Serial.print("Your Public key is: ");
	Serial.println(myPublicKey);

	uint32_t otherPublicKey = readUnsigned32();


	uint32_t sharingSecretKey = powMod(otherPublicKey, myPrivateKey, p);
	Serial.print("Your Shared Secret Key is ");
	Serial.println(sharingSecretKey);

	return sharingSecretKey;
}




// this function is used to allow the users to communicate with each other
// that will encrypt their messages so hackers can not steal their conversation
// and then decrypt the messages so the other user can understant what they said
// for our purposes, the chat will encrypt, decrypt and send the messages
// byte by byte (character by chracter)
void chat(uint32_t sharedSecretKey) {
	while(true) {
		// the portion in this if statement allows user 2 to receive an
		// encrypted character from user 1
		if (Serial3.available() > 0) {
			uint8_t encryptedCharReceived = Serial3.read();
			// using the XOR operator and the shared secret key that was determined
			// by the Diffie Hellman function, this will decrypt the characters
			uint8_t decryptedCharReceived = encryptedCharReceived ^ ((uint8_t) sharedSecretKey);
			// also want to return to the beginning of a new line when
			// carriage return is pressed
			if (decryptedCharReceived == 13) {
				Serial.println();
			}
			else {
				Serial.write(decryptedCharReceived);
			}
		}

		// this if statement will read whatever user 1 types in their
		// serial monitor, print the characters on their monitor
		// and encrypt the characters and send it to user 2
		if (Serial.available() > 0) {
			uint8_t d = Serial.read();
			// this if/else prints the characters that user 1 types on their monitor
			if (d == '\r') {
				Serial.println();
			}
			else {
				Serial.write(d);
			}
			// encrypts the characters using XOR and the shared secret key
			// then sends it to user 2
			uint8_t encryptedCharSent = d ^ ((uint8_t) sharedSecretKey);
			if (encryptedCharSent == 13) {
				Serial3.println();
			}
			else {
				Serial3.write(encryptedCharSent);
			}
		}
	}
}

int main() {
	setup();

 	uint32_t sharedKey = diffieHellman();

	delay(100);
 	Serial.println();
	Serial.println("You are now connected!!");

	chat(sharedKey);

	Serial.flush();
	Serial3.flush();

	return 0;
}
