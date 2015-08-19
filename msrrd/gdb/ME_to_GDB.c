#include "defs.h"
#include "ME_to_GDB.h"
#include "ME_common.h"
#include "top.h"
#include "infrun.h"
#include "breakpoint.h"
#include "inferior.h"
#include "event-loop.h"

int bak_stdout, bak_stderr;
int closed=0;
void BE_stdout_close() {
  if (ME_DEBUG) return;
  if (closed) {
    BE_stdout_open();
    printf("Error: tried to close already closed output!\n");
    BE_exit();
  }
  int new;
  fflush(stdout);
  fflush(stderr);
  bak_stdout = dup(1);
  bak_stderr = dup(fileno(stderr));
  new = open("/dev/null", O_WRONLY);
  dup2(new, 1);
  dup2(new, fileno(stderr));
  close(new);
  closed = 1;
}

void BE_stdout_open() {
  if (ME_DEBUG) return;
  if (!closed) {
    printf("Error: tried to open already opened output!\n");
    BE_exit();
  }
  fflush(stdout);
  fflush(stderr);
  dup2(bak_stdout, 1);
  dup2(bak_stderr, fileno(stderr));
  close(bak_stdout);
  close(bak_stderr);
  closed=0;
}


void BE_initial_gdb(void) {
  BE_stdout_close();
  execute_command("set pagination off",0);
  BE_stdout_open();
}

void BE_execute_command(const char * command) {
  BE_stdout_close();
  char * temp = strdup(command);
  execute_command(temp,0);
  free(temp);
  BE_stdout_open();
}

void BE_stop_inferior(void) {
  BE_stdout_close();
  execute_command("interrupt",0);
  wait_for_inferior();
  normal_stop();
  BE_stdout_open();
}

void BE_add_breakpoint(char * arg) {;
  BE_stdout_close();
  break_command(arg,0);  
  BE_stdout_open();
}

void BE_add_syscall_catch(const char * syscall) {
  BE_stdout_close();
  char command[64];
  sprintf(command, "catch syscall %s", syscall);
  execute_command(command,0);
  BE_stdout_open();
}

void BE_delete_breakpoint(int bp_id) {
  BE_stdout_close();
  char arg[15]; //TODO - maximum length?
  sprintf(arg, "%d", bp_id);
  delete_command(arg,0);
  BE_stdout_open();
}

void BE_resume_inferior(void) {
  BE_stdout_close();
  continue_command_JG();
  BE_stdout_open();
}

int BE_attach(int target_PID) {
  BE_stdout_close();
  
  char PID_str[64]; //maximum PID length???
  sprintf(PID_str, "%d", target_PID);

  volatile struct gdb_exception ex;
  TRY_CATCH (ex, RETURN_MASK_ERROR ) {
    attach_command(PID_str,1);
  }
  if (ex.reason < 0) {
    BE_stdout_open();
    return 1;
  }

  gdb_do_one_event (); 

  continue_command_JG();

  BE_stdout_open();
  return 0;
}

void BE_detach(void) {
  BE_stdout_close();
  execute_command("detach",0);
  BE_stdout_open();
}

int BE_fetch_inferior_event(char ** filename, int * line, int * bp_id) {
  BE_stdout_close();
  int ret =  fetch_inferior_event_JG(filename, line, bp_id);
  BE_stdout_open();
  return ret;
}

void BE_get_call_stack_wrapper(ME_CG ** stack, ME_FT * ft) {
  BE_stdout_close();
  BE_get_call_stack_as_CG(NULL,0,0,1,stack,ft);
  BE_stdout_open();
}

char *  BE_get_variable_wrapper(const char * var_name) {
  BE_stdout_close();
   char * value = NULL;

   while (value==NULL) {
    value = BE_get_variable(var_name, 0);
    if (value==NULL) {

      volatile struct gdb_exception ex;
      TRY_CATCH (ex, RETURN_MASK_ERROR ) {
	execute_command("up",0);
      }
      if (ex.reason < 0) {
	value = (char*)malloc(sizeof(char) * 64); //max length?...
      }
    }
   }
  BE_stdout_open();
  return value;
}
