#include <stdio.h>

//#define _GNU_SOURCE
#include <dlfcn.h>
typedef int (*orig_lsm_f_type)(int (*main) (int, char * *, char * *), int argc, char * * ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (* stack_end));
int start_stop;
int __libc_start_main(int (*main) (int, char * *, char * *), int argc, char * * ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (* stack_end)) {
  for(start_stop=1;start_stop;){}
  
  orig_lsm_f_type orig_lsm;
  orig_lsm = (orig_lsm_f_type)dlsym(RTLD_NEXT,"__libc_start_main");
  return orig_lsm(main,argc,ubp_av,init,fini,rtld_fini,stack_end);

  return 0;
}
