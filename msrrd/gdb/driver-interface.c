#include "defs.h"
#include "driver-interface.h"
#include "ME_common.h"
#include "ME_RLI_IR.h"
#include "ME_RLI_IR_eval.h"
#include "ME_RLI_IR_API.h"
#include "ME_to_GDB.h"
#include "queue.h"

//GDB includes
//#include "top.h"
//#include "infrun.c"

#ifdef HAVE_POLL
#if defined (HAVE_POLL_H)
#include <poll.h>
#elif defined (HAVE_SYS_POLL_H)
#include <sys/poll.h>
#endif
#endif

#include <sys/types.h>
#include <sys/time.h>
#include "gdb_select.h"
#include "observer.h"

#include <netinet/in.h>

#include <stdbool.h>

#include <assert.h>

#include <jansson.h>

BE_Context the_context;  

void BE_hook_disable(BE_hook * hook) {
  if (!hook) return;
  hook->enabled = false;
}

void BE_hook_enable(BE_hook * hook) {
  if (!hook) return;
  hook->enabled = true;
}

void BE_hook_kill(BE_hook * hook) {
  if (!hook) return;

  BE_hook_disable(hook);
  
  if (hook->event->type == BE_EVENT_B) {
    BE_stop_inferior();
    BE_delete_breakpoint(hook->event->edata.b.bp_id);
    BE_resume_inferior();
  }
}

void BE_hook_array_init(void)
{
  the_context.hook_array.count = 0;
}

int BE_hook_array_add(BE_hook * hook) {
  //TODO check for bounds
  int i = the_context.hook_array.count++;
  the_context.hook_array.hooks[i] = hook;
  return i;
}

BE_hook * BE_hook_array_get(int i) {
  return the_context.hook_array.hooks[i];
}

/*====================================================
  CONTEXT STUFF
  ====================================================*/

void BE_main(void)
{ 
  int c = 0;

  struct BE_Context * bec = BE_context_create();
  BE_log("Measurer started.");
  printd("Measurer started.\n");
 
  BE_initial_gdb();
  
  BE_start_session();
  
  printd("Main loop starting.\n");
 
  while(1) {
    BE_get_request();
    BE_do_continuous();
  }

  return;
}

int ME_sock_server_connect(int port)
{
  int listenfd = 0, connfd = 0;
  struct sockaddr_in serv_addr;
  
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0)
    {
      BE_log("ERROR: Could not create server socket!");
      BE_exit();
    }
  memset(&serv_addr, '0', sizeof(serv_addr));
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 serv_addr.sin_port = htons(port);
 
 if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) {
   BE_log("ERROR: Could not bind server socket!");
   BE_exit();
 } 

 BE_log("Listening for client on *:%d ...",port);
 printd("Listening for client on *:%d...\n",port);
 
 listen(listenfd, 10);

 connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
 
 if (connfd < 0) {
   printf("ERROR: Could not accept client!");
   BE_exit();
 }
 
 fcntl(connfd, F_SETFL, O_NONBLOCK);
 
 close(listenfd);

 return connfd;
}


void BE_start_session(void)
{  
  if (the_context.driverfd!=-1)
  {
    BE_log("Session already started!");
    BE_exit();
  }
  the_context.driverfd = ME_sock_server_connect(BE_port);

  BE_log("Client connected.");
  printd("Client connected.\n");
}

BE_Context * BE_context_create(void)
{
  the_context.attached = false;
  the_context.stopped = false;
  the_context.exited = false;
  the_context.PID = -1;
  the_context.driverfd = -1;

  strcpy(the_context.logpath, "msrrd.log"); 
      
  return &the_context;
  
}

void BE_context_print(void)
{
  BE_log("Measurer Context {PID=%d}",the_context.PID);
}

bool startsWith(const char *pre, const char *str)
{
  size_t lenpre = strlen(pre),
    lenstr = strlen(str);
  return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

int quitting = 0;

void BE_exit(void)
{
  BE_log("Measurer exited.");
  printf("msrrd exited.\n");
  close(the_context.driverfd);
  exit(0);
}

void BE_get_request(void)
{
 
  char request[1024];

  int n = ME_sock_recv(the_context.driverfd, request);

  if (n < 0) {
    if (errno==EAGAIN) return n;
    BE_log("Error: Read error [%d]!",errno);
    BE_exit();
  }
  if (n == 0) {
    BE_log("Client disconnected!");
    BE_exit();
  }
  
  if (n <= 0 || !(*request))
    return 1;
  
  //Parse out JSON
  json_t *root, *params;
  json_error_t error;
  root = json_loads(request, 0, &error);
  params = json_object_get(root,"params");
  const char * RLI_expr = json_string_value(json_array_get(params,0));
  json_t *id_copy = json_copy(json_object_get(root,"id"));

  BE_log("Client sent request \"%s\".",RLI_expr);
  
  ME_RLI_IR_value value_result = BE_rhandler_dispatch(RLI_expr);
    
  json_decref(root);

  //Send response
  root = json_object();
  json_object_set_new(root, "jsonrpc", json_string("2.0"));

  json_object_set_new(root, "result", ME_RLI_IR_value_toJSON(value_result));
  
  json_object_set_new(root, "id", id_copy);

  char * response;
  response = json_dumps( root, 0 );
  
  ME_sock_send(the_context.driverfd, response);
  
  json_decref(root);

  if (quitting) {
    close(the_context.driverfd);
    BE_exit();
  }

}

void BE_do_continuous(void)
{
  
  if (!the_context.attached || the_context.exited) {
    return;
  }

  //Check inferior for stops
  char * filename = NULL;
  int line = -1;
  int bp_id;
  int exec_state = BE_fetch_inferior_event(&filename, &line, &bp_id);
  if (exec_state == 1) {
    the_context.stopped = true;
    BE_log("Stop caught at %s:%d !", filename, line);
  } else if (exec_state == 2) {
      BE_log("Stop exit caught!", filename, line);
      the_context.exited = true;
      return;
  }

  //Check events
  BE_hook_array_handle(exec_state, filename, line, bp_id);

  if (the_context.stopped) {
    BE_resume_inferior();
    the_context.stopped = false;
  }
 
  
}

struct ME_RLI_IR_value BE_rhandler_dispatch(const char * request)
{
  ME_RLI_token * tokens = ME_RLI_tokenize(request);
  if (!tokens) return ME_RLI_IR_value_create_error("Couldn't tokenize request!"); //TODO - should not actually be of type measurement
  
  ME_RLI_IR_expr * expr = ME_RLI_IR_expr_parse(&tokens);
  if (!expr)  return ME_RLI_IR_value_create_error("Couldn't parse request!");
  //ME_RLI_IR_expr_print(expr);
  
  ME_RLI_IR_value result = ME_RLI_IR_expr_eval(expr);
  
  if (ME_DEBUG) { //TODO - move to printd
    printd("Request result = ");
    ME_RLI_IR_value_print(result);
    printd("\n");
  }
  
  //TODO delete tokens, expr
    
  return result;
}

/*============================================================*/
void BE_hook_array_handle(int breaked, char * filename, int line, int bp_id)
{  
  //check event_t
  int i = 0;
  for (i=0; i<the_context.hook_array.count;i++) {
    BE_hook * curr = BE_hook_array_get(i);  
    clock_t t = clock();
    if (curr->enabled) {
      switch (curr->event->type) {
      case BE_EVENT_T:
	if (t >= curr->event->edata.t.start + curr->event->edata.t.delay) {

	  //stop if not stopped
	  if (!the_context.stopped) {
	    BE_stop_inferior();
	    
	    //BE_get_file_and_line(get_selected_frame(NULL), &filename, &line); //put these in the BEC

	    the_context.stopped = true;
	    //continue_command_JG();
	  }    

	  
	  BE_hook_handle(curr);
	  
	  if (curr->event->repeat) {
	    curr->event->edata.t.start = t;
	  } else {
	    curr->enabled = false;
	  }
	  
	}
	break;
      case BE_EVENT_B:
	break;
      }
    }
  }

  //check event_b
  for (i=0; i<the_context.hook_array.count;i++) {
    BE_hook * curr = BE_hook_array_get(i);  
    if (curr->enabled) {
      switch (curr->event->type) {
      case BE_EVENT_T:
	break;
      case BE_EVENT_B:
	if (breaked) {
	  if (bp_id == curr->event->edata.b.bp_id)
	  {
	    BE_hook_handle(curr);
	    if (!curr->event->repeat) {
	      
	      BE_delete_breakpoint(curr->event->edata.b.bp_id);
	      
	      curr->enabled = 0;
	    }
	    
	  }
	}
      }
      
    }
  }  
}

void BE_hook_handle(BE_hook * hook) {
  if (hook->action) {
    ME_RLI_IR_expr_eval(hook->action);
    return;
  }
}


/*====================================================
  LOG STUFF
  ====================================================*/

void BE_log(const char *fmt, ...) {
  va_list arg;
  FILE *log_file = fopen(the_context.logpath, "a");
  if (log_file == NULL) return;  

  time_t t = time(NULL);
  struct tm *ptm = gmtime(&t);
  char buf[256];
  strftime(buf, sizeof buf, "%F %T", ptm);
  fprintf(log_file, "\n%s - %d : ",buf,getpid());
  
  va_start(arg, fmt);
  vfprintf(log_file, fmt, arg);
  va_end(arg);

  fclose(log_file);  
}


/*====================================================
  API STUFF
  ====================================================*/

void ME_API_set_target(int target_PID)
{

  char le[1024];
  snprintf(le, sizeof(le),"ME_API_set_target(%d)",target_PID);
  BE_log(le);
  
  if (the_context.attached) {
    ME_API_detach();
  }
  
  the_context.PID = target_PID;
  
  //Attach to process
  if (BE_attach(target_PID)) {
    BE_log("Failed to attach to process %d!",target_PID);
    return;
  }

  the_context.attached = true;

}

void ME_API_detach(void)
{
  if (the_context.attached) {
    //Detach
    BE_detach();
    
    //Update context
    the_context.attached = false;
    the_context.PID = -1;
  }
}

void ME_API_quit(void)
{
  if (the_context.attached && !the_context.exited) {
    ME_API_detach();
  }
  quitting = 1;
}

void ME_API_print_context(void)
{
  BE_context_print();
}

ME_measurement * ME_API_measure(ME_feature * feature)
{
  ME_measurement * ms = NULL;
  
  if (!the_context.attached) {
    BE_log("Not attached to a process!");
    return NULL;
  } else if (the_context.exited) {
    BE_log("Process has exited!");
    return NULL;
  }

  bool stopped_here = false;
  if (!the_context.stopped) {
    BE_stop_inferior();
    stopped_here = true;
  }
 
  //Measure Callstack
  if (feature->type == ME_FEATURE_CALLSTACK) {
    struct ME_CG * stack;
    struct ME_FT * ft = ME_FT_create();
    //BE_get_call_stack_as_CG(NULL, 0, 0, 1, &stack, ft);
    BE_get_call_stack_wrapper(&stack, ft);
    
    ms = ME_measurement_create(ME_MEASUREMENT_CALLSTACK);
    ms->data.cgft.cg = stack;
    ms->data.cgft.ft = ft;
  } else if (feature->type == ME_FEATURE_VARIABLE) {
    char * value = BE_get_variable_wrapper(feature->fdata.var_name);
    
    ms = ME_measurement_create(ME_MEASUREMENT_STRING);
    ms->data.string_val = value;
  } else if (feature->type == ME_FEATURE_MEMORY) {
    char * value = BE_get_memory(feature->fdata.m.address, feature->fdata.m.format);
    
    ms = ME_measurement_create(ME_MEASUREMENT_STRING);
    ms->data.string_val = value;
  }
    
  if (stopped_here)
    BE_resume_inferior();

  return ms;
}

void ME_API_store(int i, ME_measurement * ms) {
  if (i >= ME_STORE_SIZE) {
    BE_log("Could not store beyond bounds of measurement store!");
  }
  ms->next = the_context.store[i];
  the_context.store[i] = ms;
}

ME_measurement * ME_API_load(int i) {
  if (i >= ME_STORE_SIZE) {
    BE_log("Could not load beyond bounds of measurement store!");
    return NULL;
  }
  return the_context.store[i];
}

BE_event * ME_API_delay(int delay, int repeat) {
  return BE_event_t_create(delay,repeat);
}

BE_event * ME_API_reach(char * filename, int line, int repeat) {
  BE_event * event = BE_event_b_create(get_breakpoint_count()+1,repeat);

  //interrupt and insert breakpoint
  BE_stop_inferior();
  char arg[64];
  sprintf(arg, "%s:%d", filename, line);
  BE_add_breakpoint(arg);
  BE_resume_inferior();
      
  return event;
}

BE_event * ME_API_reach_func(char * func_name, int repeat) {
  BE_event * event = BE_event_b_create(get_breakpoint_count()+1,repeat);

  //interrupt and insert breakpoint
  BE_stop_inferior();
  BE_add_breakpoint(func_name);
  BE_resume_inferior();
      
  return event;
}

BE_event * ME_API_reach_syscall(char * syscall, int repeat) {
  BE_event * event = BE_event_b_create(get_breakpoint_count()+1,repeat);

  BE_stop_inferior();
  BE_add_syscall_catch(syscall);
  BE_resume_inferior();
  
  return event;
}

int ME_API_hook(struct BE_event * event, struct ME_RLI_IR_expr * action) {
  if (!the_context.attached) {
    return -1;
  } else if (the_context.exited) {
    return -1;
  }

  BE_hook * hook = (BE_hook *)malloc(sizeof(BE_hook));
  hook->event = event;
  hook->action = action;
  hook->enabled = true;
  int i = BE_hook_array_add(hook);  
  return i;
}

void ME_API_kill(int i) {
  BE_hook_kill(BE_hook_array_get(i));
} 

void ME_API_enable(int i) {
  BE_hook_enable(BE_hook_array_get(i));
} 

void ME_API_disable(int i) {
  BE_hook_disable(BE_hook_array_get(i));
} 

ME_feature * ME_API_callstack(void) {
  return ME_feature_create_callstack();
}

ME_feature * ME_API_var(char * var_name) {
  return ME_feature_create_variable(var_name);
}

ME_feature * ME_API_mem(char * address, char * format) {
  return ME_feature_create_memory(address, format);
}

void ME_API_gdb(char * command) {
  if (!the_context.attached) {
    BE_log("Not attached to a process!");
    return;
  } else if (the_context.exited) {
    BE_log("Process has exited!");
    return;
  }

  bool stopped_here = false;
  if (!the_context.stopped) {
    BE_stop_inferior();
    stopped_here = true;
  }
 
  BE_execute_command(command);

  if (stopped_here) {
    //Continue Inferior
    BE_resume_inferior();
  }

}
