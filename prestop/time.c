#include <stdio.h>
#include <time.h>

//#define _GNU_SOURCE
#include <dlfcn.h>
typedef int (*orig_lsm_f_type)(int (*main) (int, char * *, char * *), int argc, char * * ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (* stack_end));
typedef void (*orig_exit_f_type)(int status) __attribute__((noreturn));
typedef int (*orig_main_f_type)(int, char * *, char * *);

//time_t start, stop;
clock_t start, stop;

void exit(int status)
{
  //time(&stop);
  stop = clock();
  
  //printf("\n\nElapsed time = %.0f\n",difftime(stop,start));
  printf("\n\nElapsed time = \n%f\n",((double)stop-start)/CLOCKS_PER_SEC);
  
  orig_exit_f_type orig_exit;
  orig_exit = (orig_exit_f_type)dlsym(RTLD_NEXT,"exit");
  orig_exit(status);
}

orig_main_f_type orig_main;

int timed_main(int argc, char ** argv, char ** huh) {
  //time(&start);
  start = clock();
  
  orig_main(argc,argv,huh);

  //time(&stop);
  stop = clock();
  printf("\n\nElapsed time = \n%f\n",((double)stop-start)/CLOCKS_PER_SEC);

  return 0;
}


int start_stop;
int __libc_start_main(int (*main) (int, char * *, char * *), int argc, char * * ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (* stack_end)) {
  for(start_stop=1;start_stop;){}
  
  orig_main = (orig_main_f_type)main;
  
  orig_lsm_f_type orig_lsm;
  orig_lsm = (orig_lsm_f_type)dlsym(RTLD_NEXT,"__libc_start_main");
  int result = orig_lsm(timed_main,argc,ubp_av,init,fini,rtld_fini,stack_end);
  
  return result;
}
