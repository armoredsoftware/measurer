#include <stdlib.h>
#include <signal.h>
int test_stop;
  
void main() {
  for(test_stop=1;test_stop;) {}
  int a = 1;
  a=2;
  a=3;
  a=4;
  a=5;
  a=6;
  a=7;
  a=8;
  a=9;
  a=10;
  for(test_stop=1;test_stop;) {}
}
