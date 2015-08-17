#include "defs.h"
#include "ME_to_GDB.h"
#include "ME_common.h"
#include "top.h"
#include "infrun.h"
#include "breakpoint.h"
#include "inferior.h"
#include "event-loop.h"

void BE_initial_gdb(void) {
  execute_command("set pagination off",0);
}


void BE_execute_command(const char * command) {
  char * temp = strdup(command);
  execute_command(temp,0);
  free(temp);
}

void BE_stop_inferior(void) {
  execute_command("interrupt",0);
  wait_for_inferior();
  normal_stop();
}

void BE_add_breakpoint(char * arg) {;
  break_command(arg,0);  
}

void BE_add_syscall_catch(const char * syscall) {
  char command[64];
  sprintf(command, "catch syscall %s", syscall);
  execute_command(command,0);
}

void BE_delete_breakpoint(int bp_id) {
  char arg[15]; //TODO - maximum length?
  sprintf(arg, "%d", bp_id);
  delete_command(arg,0);
}

void BE_resume_inferior(void) {
  continue_command_JG();
}

int BE_attach(int target_PID) {
  char PID_str[64]; //maximum PID length???
  sprintf(PID_str, "%d", target_PID);

  volatile struct gdb_exception ex;
  TRY_CATCH (ex, RETURN_MASK_ERROR ) {
    attach_command(PID_str,1);
  }
  if (ex.reason < 0) {
    return 1;
  }

  gdb_do_one_event (); 

  continue_command_JG();
  
  return 0;
}

void BE_detach(void) {
  execute_command("detach",0);
}

int BE_fetch_inferior_event(char ** filename, int * line, int * bp_id) {
  return fetch_inferior_event_JG(filename, line, bp_id);
}

void BE_get_call_stack_wrapper(ME_CG ** stack, ME_FT * ft) {
  BE_get_call_stack_as_CG(NULL,0,0,1,stack,ft);
}

char *  BE_get_variable_wrapper(const char * var_name) {
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
   return value;

}
