#include <iostream>
#include <cmath> // for sqrt

using namespace std;

unsigned int number;
// given a positive integer n <= 4,000,000 this returns the smallest
// integer d such that d*d > n
//
// not needed to solve the problem, but some might find it helpful
unsigned int upper_sqrt(unsigned int n) {
  unsigned int d(sqrt((double) n));

  // should iterate at most one or twice
  while (d*d <= n) {
    ++d;
  }

  return d;
}

int main() {
  // read in the integer
  cin >> number;
  // NOTE: look *carefully* at the maximum value of the number
  // in the problem description
  unsigned int plusOne = upper_sqrt(number);
  // see if the integer it is prime
  if (number == 1){
    cout << "not prime" << endl;
    return 0;
  }
  if (number == 2){
    cout << "prime" << endl;
    return 0;
  }
  if (number % 2 == 0){
    cout << "not prime" << endl;
    return 0;
  }
  bool answer = true;

  for (unsigned int i = 3; i < plusOne; i += 2){
    if (number % i == 0){
      answer = false;
      break;
    }
  }

  // print the answer
  if (answer){
    cout << "prime" << endl;
  }
  else {
    cout << "not prime" << endl;
  }

  return 0;
}
