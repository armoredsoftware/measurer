#include "ME_common.h"

extern int bak_stdin;

extern void BE_stdout_close(void);
extern void BE_stdout_open(void);

extern void BE_initial_gdb(void);
extern void BE_execute_command(const char *);
extern void BE_stop_inferior(void);
extern void BE_add_breakpoint(char *);
extern void BE_add_syscall_catch(const char *);
extern void BE_delete_breakpoint(int);
extern void BE_resume_inferior(void);
extern int BE_attach(int);
extern void BE_detach(void);
extern int BE_fetch_inferior_event(char **, int *, int *);
extern void BE_get_call_stack_wrapper(ME_CG **, ME_FT *); 
extern char *  BE_get_variable_wrapper(const char *);  
extern char *  BE_get_memory_wrapper(char *, char *); 
extern void BE_quit(void);
extern void BE_send_signal(void);
