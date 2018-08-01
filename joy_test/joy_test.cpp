#include <Arduino.h>
#define JOY_HORIZ A0
#define JOY_VERT A1
#define JOY_SEL 2

#define JOY_CENTER 524
#define JOY_DEADZONE 64

void setup() {
	init();

	pinMode(JOY_SEL, INPUT_PULLUP);

	Serial.begin(9600);
}

void joystickReadings(){
	int xVal = analogRead(JOY_HORIZ);
	int yVal = analogRead(JOY_VERT);
	int buttonVal = digitalRead(JOY_SEL);

	// x value increase as we push left and decrease for right
	Serial.print("xVal: ");
	Serial.println(xVal);

	// y value increases as we push down and decrease for up
	Serial.print("yVal: ");
	Serial.println(yVal);

	// button value == 0 when pressed and 1 when not
	Serial.print("buttonVal: ");
	Serial.println(buttonVal);

	Serial.println("----------------");
}

void printDirection(){
	int xVal = analogRead(JOY_HORIZ);
	int yVal = analogRead(JOY_VERT);
	int buttonVal = digitalRead(JOY_SEL);
	bool stickPushed = false;

	if (xVal > JOY_CENTER + JOY_DEADZONE){
		Serial.print("left ");
		stickPushed = true;
	}
	if (xVal < JOY_CENTER - JOY_DEADZONE){
		Serial.print("right ");
		stickPushed = true;
	}
	if (yVal > JOY_CENTER + JOY_DEADZONE){
		Serial.print("down ");
		stickPushed = true;
	}
	if (yVal < JOY_CENTER - JOY_DEADZONE){
		Serial.print("up ");
		stickPushed = true;
	}
	// if (abs(xVal - JOY_CENTER) <= JOY_DEADZONE &&
	// 		abs(yVal - JOY_CENTER) <= JOY_DEADZONE){
	// 	Serial.print("center ");
	// }
	if (stickPushed != true){
		Serial.print("center ");
	}
	if (buttonVal == 0){
		Serial.print("button pressed ");
	}
	Serial.println();
}

int main() {
	setup();
	while (true){
		printDirection();
		// joystickReadings();
		delay(1000);
	}

	Serial.end();

	return 0;
}
