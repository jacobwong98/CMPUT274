#include <iostream>
#include "gcd.h"
using namespace std;

int main(){
	unsigned int num1;
	unsigned int num2;

	cout << "Enter two nonnegative integers (at least one positive): ";
	cin >> num1 >> num2;
	unsigned int calcGCD = gcd(num1, num2);

	cout << "The greatest common divisor of " << num1 << " "<< "and "
			 << num2 << " " << "is " << calcGCD << "." << endl;

}
