#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

//#define _GNU_SOURCE
#include <dlfcn.h>
typedef int (*orig_lsm_f_type)(int (*main) (int, char * *, char * *), int argc, char * * ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (* stack_end));

int start_stop;
void signal_callback_handler(int signum)
{
  printf("Caught signal %d\n",signum);
  int pid = fork();
  if (!pid) {
    for(start_stop=1;start_stop;){}
    exit(0);
  }
  //exit(signum);
}

int __libc_start_main(int (*main) (int, char * *, char * *), int argc, char * * ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (* stack_end)) {
  signal(SIGUSR1, signal_callback_handler);
  
  orig_lsm_f_type orig_lsm;
  orig_lsm = (orig_lsm_f_type)dlsym(RTLD_NEXT,"__libc_start_main");
  return orig_lsm(main,argc,ubp_av,init,fini,rtld_fini,stack_end);

  return 0;
}
