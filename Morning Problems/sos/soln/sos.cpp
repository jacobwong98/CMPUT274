#include <iostream>
#include <cmath> // for sqrt

using namespace std;

// Compute the greatest integer d such that d*d <= n
// (i.e. the floor of the square root).
//
// You may use this function or ignore it, it's up to you.
unsigned int integer_sqrt(unsigned int x) {
  unsigned int d(sqrt(x));

  // should iterate at most once, probably none
  while ((d+1)*(d+1) <= x) {
    ++d;
  }

  // probably does not iterate even once
  while (d*d > x) {
    --d;
  }

  // now at this point we know (d+1)*(d+1) > x yet d*d <= x

  return d;
}

int main() {
  // declare your variables
  unsigned int number;
  // read the input
  cin >> number;
  unsigned int lowSqrt = integer_sqrt(number);
  // solve the problem
  if (number == 0 || number == 1 || number == 2){
    cout << "sum of squares" << endl;
    return 0;
  }
  if (sqrt(number) == lowSqrt){
    cout << "sum of squares" << endl;
    return 0;
  }
  bool answer = false;
  for (unsigned int i = 0; i < lowSqrt;i++){
    unsigned int minus = number - (i*i);
    if (sqrt(minus) == integer_sqrt(minus)){
      answer = true;
    }
  }
  // print the answer
  if (answer){
    cout << "sum of squares" << endl;
  }
  else {
    cout << "not sum of squares" << endl;
  }

  return 0;
}
