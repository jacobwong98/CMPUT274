#include <iostream>

using namespace std;

int main() {
  int d1, d2, d3, d4;
  int d1Position, d2Position, d3Position, d4Position;
  int sumDig;

  cin >> d1 >> d2 >> d3 >> d4;

  // Put your code here!
  d1Position = 1 * d1;
  d2Position = 2 * d2;
  d3Position = 3 * d3;
  d4Position = 4 * d4;

  sumDig = d1Position + d2Position + d3Position + d4Position;
  char Yes[] = "yes";
  if (sumDig%5 == 0){
    cout << Yes << endl;
  }
  else {
    cout << "no" << endl;
  }

  return 0;
}
