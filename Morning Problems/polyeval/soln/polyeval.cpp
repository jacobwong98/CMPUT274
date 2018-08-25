#include <iostream>


using namespace std;

int main() {
  // Declare your variables
  int exponent;
  int xVal;
  int modNum;
  int sum = 0;
  // Read the input
  cin >> exponent >> xVal >> modNum;
  // Solve the problem
  int coeff[exponent + 1];
  int xExp[exponent + 1];
  for (int i = 0; i <= exponent; i++){
    cin >> coeff[i];
  }
  for (int i = 0; i <= exponent; i++){
    if (i == 0){
      xExp[i] = 1;
    }
    else{
      xExp[i] = (xExp[i-1]*xVal)% modNum;
    }
  }
  for (int i = 0; i <= exponent; i++){
    sum = (sum + coeff[i]*xExp[i])%modNum;
  }
  int modAns = sum % modNum;
  cout << modAns << endl;

  return 0;
}
