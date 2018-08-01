#include <iostream>
#include <algorithm> // for max()

using namespace std;

int main() {
  int arraysize;
  int changeMax = 0;
  int change = 0;
  cin >> arraysize;
  // declare your variables
  int array[arraysize];
  // read the input
  for (int i = 0; i < arraysize; i++){
    cin >> array[i];
  }


  // compute the answer
  for (int i = 0; i < arraysize - 1; i++){
    if (array[i] > array[i + 1]){
      change = change + (array[i] - array[i+1]);
    }
    else{
      change = 0;
    }
    if (change > changeMax){
      changeMax = change;
    }
  }
  // output the answer
  cout << changeMax << endl;
  return 0;
}
