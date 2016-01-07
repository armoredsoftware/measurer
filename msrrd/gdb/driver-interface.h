#ifndef DRIVER_INTERFACE_H
#define DRIVER_INTERFACE_H

#define ME_STORE_SIZE 64

#include <stdbool.h>
#include "ME_RLI_IR.h"
#include "ME_common.h"

extern int BE_port;
extern int BE_child_pid;
extern char * BE_binary;

struct BE_snap_array;
typedef struct BE_snap_array {
  struct BE_snapshot * snap_records[64]; //MAX hook array size???
  int count;
  int next_port;
}
BE_snap_array;

struct BE_hook_array;
typedef struct BE_hook_array {
  struct BE_hook * hooks[64]; //MAX hook array size???
  int count;
}
BE_hook_array;

struct BE_snapshot;
typedef struct BE_snapshot {
  int msrrd_port;
  int app_pid;
  int sockfd;
}
BE_snapshot;
  
struct BE_Context;
typedef struct BE_Context
{
  bool attached;
  bool stopped;
  bool exited;
  int PID;
  int driverfd;
  char logpath[64]; //TODO - static length
  
  struct BE_hook_array hook_array;
  struct BE_snap_array snap_array;

  //measurement store
  struct ME_measurement * store[ME_STORE_SIZE]; 
  
}
BE_Context;

extern BE_Context the_context;

extern void BE_hook_array_init(void);
extern int BE_hook_array_add(struct BE_hook *);
extern struct BE_hook * BE_hook_array_get(int);
extern void BE_hook_array_handle(int, char *, int, int);

extern struct BE_snapshot * BE_snapshot_create(int, int, int);
extern int BE_snap_array_add(struct BE_snapshot *);
extern struct BE_snapshot * BE_snap_array_get(int);

extern void BE_main(void);
extern void BE_start_session(void);
extern void BE_exit(void);

extern void BE_log(const char *, ...);

extern struct BE_Context * BE_context_create(void);
extern void BE_context_print(void);
extern void BE_get_request(void);
extern bool startsWith(const char *, const char *);

extern void BE_update_callgraph(void);
extern void BE_do_continous(void);
extern struct ME_RLI_IR_value BE_rhandler_dispatch(const char *);

extern void BE_hook_handle(struct BE_hook *);
extern void BE_hook_table_handle(struct BE_hook *, int, char *, int);

extern int ME_API_set_target(int);
extern void ME_API_detach(void);
extern void ME_API_quit(void);
extern void ME_API_print_context(void);
extern struct ME_measurement * ME_API_measure_callstack(void);
extern struct ME_measurement * ME_API_measure(struct ME_feature *);
extern void ME_API_store(int, struct ME_measurement *);
extern struct ME_measurement * ME_API_load(int);
extern struct BE_event * ME_API_delay(int, int);
extern struct BE_event * ME_API_reach(char *, int,int);
extern struct BE_event * ME_API_reach_func(char *,int);
extern struct BE_event * ME_API_reach_syscall(char *,int);
extern int ME_API_hook(struct BE_event *, struct ME_RLI_IR_expr *);
extern void ME_API_kill(int);
extern void ME_API_enable(int);
extern void ME_API_disable(int);
extern struct ME_feature * ME_API_callstack(void);
extern struct ME_feature * ME_API_var(char *);
extern struct ME_feature * ME_API_mem(char *,char *);
extern void ME_API_gdb(char *);
extern struct ME_RLI_IR_value ME_API_snap(void);
extern struct ME_RLI_IR_value ME_API_to_snap(int, char *);

#endif
