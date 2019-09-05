#include "iAddr.h"
#include <iostream>
#include <string>
using namespace std;

int mian () {
  string str = "10.108.0.211";
  iAddr s(str);

  cout << s.getAddr() << endl;
  return 0;
}
