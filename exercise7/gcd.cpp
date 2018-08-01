#include "gcd.h"

unsigned int gcd(unsigned int a, unsigned int b) {
	// base case for recursion
	if (a == 0){
		return b;
	}
	// Euclid algorithm that shows that the gcd is b mod a and a
	else{
		return gcd(b % a, a);
	}
}
