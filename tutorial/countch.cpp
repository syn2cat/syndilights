#include <map>
#include <iostream>
using namespace std;

int main() {
  map<char, int> freqs; 
  char ch;
  
  while (cin .get(ch))
    freqs[ch]++;
 
  int i;
  map<char,int>::iterator it;

  for (i=1, it = freqs.begin(); it != freqs.end(); ++it,++i) {
    switch (it->first) 
      {
      case '\r': cout << "\\r"; break;
      case '\t': cout << "\\t"; break;
      case '\n': cout << "\\n"; break;
      case ' ' : cout  << "Space"; break;
      default: cout << it->first;
    }
    
    cout << "\t" <<  it->second << ((i%4) ? "\t" : "\n");
  }
  // cout << freqs;
}
