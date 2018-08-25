#include <iostream>

using namespace std;

int main() {
  // Declare your variables
  int n;

  // Read the input
  cin >> n;
  int number1[n];
  int number2[n];

    for (int i = 0; i < n; i++){
      cin >> number1[i];
    }
    for (int i = 0; i < n; i++){
      cin >> number2[i];
    }

  int result[n];
  // Solve the problem
  for (int i = n - 1; i >= 0; i--){
    if (number1[i] - number2[i] >= 0 ){
      result[i] = number1[i] - number2[i];
    }
    else {
      number1[i - 1] = number1[i - 1] - 1;
      number1[i] = number1[i] + 10;
      result[i] = number1[i] - number2[i];
    }
  }

  for (int i = 0; i < n - 1; i++){
    cout << result[i] << " ";
  }
  cout << result[n - 1] << endl;
  return 0;
}
