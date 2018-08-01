/*
	Chat Program Part 2
*/

#include <Arduino.h>

int digitalInPin = 13;
int analogInPin = A1;
uint32_t bitPattern = 0;
uint32_t pattern[16];




void setup() {
	init();
	pinMode(digitalInPin, OUTPUT);
	Serial.begin(9600);
	Serial3.begin(9600);

}

// this function is used to acquire a random private key
// by reading the voltage from analogue pin A1 and using the & operator
// to compare the bits of that number to 1 to obtain the
// least significant digit of the voltage
uint32_t getPrivateKey() {
	// repeat process 32 times to acquire a random 32 bit number
	for (int i = 0; i < 32; i++){
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

// this function will compute a*b mod m.
// it is also used to account for overflow if a and b are 31 bit numbers
uint32_t mulMod(uint32_t a, uint32_t b, uint32_t m){
	uint32_t sqrVal = a%m;
	uint32_t newB = b%m;
	uint32_t andSqrVal;
	uint32_t changeInB = 1;
	int count=0;
	uint32_t answer = 0;

	// will run until a = 0 and this will skip any unecessary zero bits
	// to reduce the amount of multiplications that need to be run
	while (sqrVal > 0) {
		andSqrVal = sqrVal & 1;
		// if it is the first iteration of the while loop simply assign
		// changeInB as b%m because based on the Mod multplication worksheet
		// we times 2^0 * b == b
		if (count == 0){
			changeInB = newB;
		}
		// for every other iteration, we can simply times the changeInB
		// by 2 to get 2^(i+1) * b
		else{
			changeInB = (changeInB<<1)%m;
		}
		// if the least-significant bit of a == 1 we want to add the answer
		// with the changeInB and ignore the addition if andSqrVal == 0
		if (andSqrVal == 1){
			answer = (answer + changeInB)%m;
		}
		sqrVal = sqrVal>>1;
		count++;
	}
	return answer;
}

// this function will calculate a times a and repeat b times
// while taking the mod of each intermediate step
uint32_t powMod (uint32_t a1, uint32_t b1, uint32_t m1) {
	uint32_t sqrVal1 = a1%m1;
	uint32_t result1 = 1%m1;
	uint32_t newB1 = b1;
	while (newB1 > 0) {
		if (newB1 & 1){
			result1 = mulMod(result1, sqrVal1 , m1);// (result1 * sqrVal1)%m1;

		}
		sqrVal1 =  mulMod(sqrVal1, sqrVal1, m1);// (sqrVal1 * sqrVal1)%m1;
		newB1 = (newB1 >> 1);
	}
	return result1;
}

bool wait_on_serial3( uint8_t nbytes, long timeout ) {
  unsigned long deadline = millis() + timeout;
  while (Serial3.available()<nbytes && (timeout<0 || millis()<deadline))
  {
    delay(1);
  }
  return Serial3.available()>=nbytes;
}

/** Writes an uint32_t to Serial3, starting from the least-significant
 * and finishing with the most significant byte.
 */
void uint32_to_serial3(uint32_t num) {
  Serial3.write((char) (num >> 0));
  Serial3.write((char) (num >> 8));
  Serial3.write((char) (num >> 16));
  Serial3.write((char) (num >> 24));
}

/** Reads an uint32_t from Serial3, starting from the least-significant
 * and finishing with the most significant byte.
 */
uint32_t uint32_from_serial3() {
  uint32_t num = 0;
  num = num | ((uint32_t) Serial3.read()) << 0;
  num = num | ((uint32_t) Serial3.read()) << 8;
  num = num | ((uint32_t) Serial3.read()) << 16;
  num = num | ((uint32_t) Serial3.read()) << 24;
  return num;
}


// this function is the state machine for the client as described on eClass
uint32_t client(uint32_t ckey){
	Serial.println("I am client");
	uint32_t skey;
	enum State {START, WaitingForAck, DataExchange };
	State currentState = START;
	long timeout = 1000;

	while (currentState != DataExchange){
		if (currentState == START){
			Serial3.write('C');
			uint32_to_serial3(ckey);
			currentState = WaitingForAck;
		}
		else if(currentState == WaitingForAck && wait_on_serial3(5,timeout)){
			char ACK = Serial3.read();
			if (ACK == 'A'){
				skey = uint32_from_serial3();
				Serial3.write('A');
			}
			currentState = DataExchange;
		}
		else {
			currentState = START;
		}
	}

	return skey;
}

// this is the state machine for the server as described in eClass
uint32_t server(uint32_t skey){
	Serial.println("I am server");
	Serial.println("Waiting for client...");
	uint32_t ckey;
	enum State { LISTEN, WaitingForKey1, WaitForAck1, WaitingForKey2, WaitForAck2, DataExchange};
	State currentState = LISTEN;
	long timeout = 1000;

	while (currentState != DataExchange){
		if (currentState == LISTEN && wait_on_serial3(1, timeout)){
			char CR = Serial3.read();
			if (CR == 'C'){
				currentState = WaitingForKey1;
			}
		}
		else if (currentState == WaitingForKey1 && wait_on_serial3(4, timeout)){
			ckey = uint32_from_serial3();
			Serial3.write('A');
			uint32_to_serial3(skey);
			currentState = WaitForAck1;
		}
		else if (currentState == WaitForAck1 && wait_on_serial3(1, timeout)){
			char readLetter = Serial3.read();
			if (readLetter == 'A'){
				currentState = DataExchange;
			}
			if (readLetter == 'C'){
				currentState = WaitingForKey2;
			}
		}
		else if (currentState == WaitingForKey2 && wait_on_serial3(4,timeout)){
			ckey = uint32_from_serial3();
			currentState = WaitForAck2;
		}
		else if (currentState == WaitForAck2 && wait_on_serial3(1, timeout)){
			char letterRead = Serial3.read();
			if (letterRead == 'A'){
				currentState = DataExchange;
			}
			if (letterRead == 'C'){
				currentState = WaitingForKey2;
			}
		}
		else {
			currentState = LISTEN;
		}
	}

	return ckey;

}

// this function checks digital pin 13 and determines which Arduino is the
// server and which one is the client
uint32_t serverVsClient(uint32_t enteredKey){
	uint32_t otherKey;
	int pinState = digitalRead(digitalInPin);
	if (pinState == HIGH){
		 otherKey = server(enteredKey);
	}
	else {
		 otherKey = client(enteredKey);
	}
	return otherKey;
}



// this function is used to execute the Diffie Hellman process
uint32_t diffieHellman() {
	// agreed upon values that are used to calculate the personal public key
	const uint32_t p = 2147483647;
	const uint32_t g = 16807;

	uint32_t myPrivateKey = getPrivateKey();
	uint32_t myPublicKey = powMod(g, myPrivateKey, p);


	uint32_t otherPublicKey = serverVsClient(myPublicKey);


	uint32_t sharingSecretKey = powMod(otherPublicKey, myPrivateKey, p);
	return sharingSecretKey;
}

// rolling cipher code given to us in Assignment 1 Description on eClass
uint32_t next_key(uint32_t current_key) {
  const uint32_t modulus = 0x7FFFFFFF; // 2^31-1
  const uint32_t consta = 48271;  // we use that consta<2^16
  uint32_t lo = consta*(current_key & 0xFFFF);
  uint32_t hi = consta*(current_key >> 16);
  lo += (hi & 0x7FFF)<<16;
  lo += hi>>15;
  if (lo > modulus) lo -= modulus;
  return lo;
}




// this function is used to allow the users to communicate with each other
// that will encrypt their messages so hackers can not steal their conversation
// and then decrypt the messages so the other user can understand what they said
// for our purposes, the chat will encrypt, decrypt and send the messages
// byte by byte (character by chracter)
void chat(uint32_t sharedSecretKey) {
	Serial.println("----------------------------------");
	Serial.println("You are now connected!");
	while(true) {
		uint32_t encryptingKey = sharedSecretKey;
		uint32_t decryptingKey = sharedSecretKey;



		// the portion in this if statement allows user 2 to receive an
		// encrypted character from user 1
		if (Serial3.available() > 0) {
			uint8_t encryptedCharReceived = Serial3.read();
			// using the XOR operator and the shared secret key that was determined
			// by the Diffie Hellman function, this will decrypt the characters
			uint8_t decryptedCharReceived = encryptedCharReceived ^ ((uint8_t) decryptingKey);
			// also want to return to the beginning of a new line when
			// carriage return is pressed
			if (decryptedCharReceived == 13) {
				Serial.println();
			}
			else {
				Serial.write(decryptedCharReceived);
			}
			decryptingKey = next_key(decryptingKey);
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
			uint8_t encryptedCharSent = d ^ ((uint8_t) encryptingKey);
			if (encryptedCharSent == 13) {
				Serial3.println();
			}
			else {
				Serial3.write(encryptedCharSent);
			}
			encryptingKey = next_key(encryptingKey);
		}
	}
}

int main() {
	setup();
	Serial.println("Welcome to Arduino Chat Program v4.20");
	uint32_t sharedKey = diffieHellman();

	chat(sharedKey);

	Serial.flush();
	Serial3.flush();

	return 0;
}
