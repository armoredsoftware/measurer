#include <stdio.h>
#include <unistd.h>

int main() {
  int a = 0;
  int b = 1;
  int c = 2;
  
  while (1) {
    char filename[10];
    sprintf(filename, "%d.out", getpid());
    
    FILE *f;
    f = fopen(filename, "a");
    fprintf(f,"a=%d b=%d c=%d *c=%d=%08x\n",a,b,c,&c,&c);
    fclose(f);

    c++;
    sleep(3);    

    if (a) {
      fork();
      a = 0;
    }

  }

}
