#include <iostream>
#include <algorithm> // for max()

using namespace std;

int main() {
  // declare variables
  int arraysize;
  int maxConsec = 1;
  int consecChain = 1;
  // read the input
  cin >> arraysize;
  int array[arraysize];
  for (int i = 0; i < arraysize; i++){
    cin >> array[i];
  }
  // compute the answer
  for (int i = 0; i < arraysize - 1; i++){
    if (array[i] == array[i + 1]){
      consecChain += 1;
    }
    else {
      consecChain = 1;
    }
    if (consecChain > maxConsec){
      maxConsec = consecChain;
    }
  }
  // print the output
  cout << maxConsec << endl;

  return 0;
}
