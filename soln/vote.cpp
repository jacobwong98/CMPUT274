#include <iostream>

using namespace std;

int main() {
  int n = 0;
  int vote[1001];
  int countVote[1001];
  int maxVotes = 0;
  int tieVoteNum = 0;
  int maxVotesCand;
  for (int c = 0; c < 1001; c++){
    countVote[c] = 0;
  }
  for (int c = 0; c < 1001; c++){
    vote[c] = 0;
  }

  // keep reading until we see 0
  while (true) {
    cin >> vote[n];
    if (vote[n] == 0) {
      break;
    }
    ++n;
  }

  // now n == # votes and vote[i] is the i'th vote
  // for 0 <= i <= n-1

  // solve the problem and print the answer
  for (int i = 0; i < n-1; i++){
    for (int b = 0; b < 1001; b++ ){
      if (vote[i] == b){
        countVote[b] = countVote[b] + 1;
        continue;
      }
    }
  }

  while (true){
    int a = 0;
    if (a == 1001){
      break;
    }
    if (maxVotes == 0){
      maxVotes = countVote[a];
      maxVotesCand = a;
    }
    if (countVote[a] > maxVotes){
      maxVotes = countVote[a];
      maxVotesCand = a;
    }
    else if (countVote[a] == maxVotes){
      tieVoteNum == maxVotes;
    }
    a++;
  }


  if (tieVoteNum == maxVotes){
    cout << "tie" << endl;
  }
  else {
    cout << maxVotesCand << endl;
  }


  return 0;
}
