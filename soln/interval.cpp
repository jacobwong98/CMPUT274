#include <iostream>

using namespace std;

int main() {
  int low1, high1, low2, high2;
  cin >> low1 >> high1 >> low2 >> high2;

  if (high1 < low2 || high2 < low1){
    cout << "disjoint" << endl;
  }
  else {
    cout << "intersect" << endl;
  }


  return 0;
}
