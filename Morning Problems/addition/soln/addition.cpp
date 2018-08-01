#include <iostream>

using namespace std;

int main() {
  // Declare your variables
  int n;


  // Read the input
  cin >> n;
  int number1[n];
  int number2[n];
  int result[n + 1];
  int carry = 0;

  if (n == 1){
    cin >> number1[1];
    cin >> number2[1];
  }
  else {
    for (int i = 1; i < n; i++){
      cin >> number1[i];
    }
    for (int i = 1; i < n; i++){
    cin >> number2[i];
    }
  }

  for (int i = n-1; n >= 0; n--){
    result[n] = number1[n] + number2[n] + carry;
    if (result[n] > 9){
      carry = 1;
      result[n] = result[n] - 10;
    }
    else {
      carry = 0;
    }
  }


  // Solve the problem
  for (int i = 0; i < n + 1; i++){
    cout << result[i] << " ";
  }

  return 0;
}
