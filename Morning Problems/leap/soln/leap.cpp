#include <iostream>

using namespace std;

int main() {
  int year;
  cin >> year;
  // char answer[4];

  // Put your code here!
  if ((year%4 == 0 && year%100 != 0) || year%400 == 0){
    cout << "yes" << endl;
  }
  else {
    cout << "no" << endl;
  }


  return 0;
}
