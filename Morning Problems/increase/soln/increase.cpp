#include <iostream>

using namespace std;

int main() {
  int n;
  long long answer = 0;


  cin >> n;
  int array[n-1];
  for (int i = 0; i < n; ++i) {
    cin >> array[i];
  }

  // now compute and print the answer
  for (int count = 0; count < n; count++){
    if (array[count + 1] > array[count]){
      answer = answer + (array[count + 1] - array[count]);

    }
  }

  cout << answer << endl;
  return 0;
}
