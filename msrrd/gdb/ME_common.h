#ifndef ME_COMMON_H
#define ME_COMMON_H

#ifndef JASON_CLIENT
#include "defs.h"
#endif


//#define DEBUG 1
extern int ME_DEBUG;

#define printd(...) { if (ME_DEBUG){ fprintf(stdout, "%s:%d:%s(): ", __FILE__, __LINE__, __func__); fprintf(stdout, __VA_ARGS__); } }
//#else
//#define printd
//#endif


#include <stdbool.h>
#include <time.h>
#include <jansson.h>

extern char** str_split(char*, const char);
extern void free_str_split(char**);

extern int ME_sock_server_connect (int);
extern int ME_sock_recv(int,char *);
extern void ME_sock_send(int, char *);
extern void ME_sock_recv_dynamic(int, int *, char **);
extern void ME_sock_send_dynamic(int, int, char *);

struct ME_CG;
struct ME_FT;

struct ME_CG_AND_FT;

typedef struct ME_CG
{
  struct ME_CG * child;
  struct ME_CG * sibling;
  int symbol;
}
  ME_CG;


typedef struct ME_FT
{
  struct ME_FT * next;
  char * name;
}
  ME_FT;


extern struct ME_CG * ME_CG_create(int);
extern void ME_CG_add_child(struct ME_CG *, struct ME_CG *);
extern struct ME_CG * ME_CG_copy(struct ME_CG *);
extern void ME_CG_delete(struct ME_CG *);
extern void ME_CG_merge_stack(struct ME_CG *, struct ME_CG *);
extern void ME_CG_print_s_h(struct ME_CG *);
extern void ME_CG_print_s(struct ME_CG *);
extern void ME_CG_print_h(struct ME_CG *, struct ME_FT *);
extern void ME_CG_print(struct ME_CG *, struct ME_FT *);
extern int ME_CG_count(struct ME_CG *);
extern int ME_CG_encode_h(struct ME_CG *, int, int *);
extern void ME_CG_encode(struct ME_CG *, int *, char **);
extern void ME_CG_print_encoded(int, char *);
extern struct ME_CG * ME_CG_decode_h(int *, int);
extern void ME_CG_decode(char *, struct ME_CG **);

extern struct ME_FT * ME_FT_create_entry(const char *);
extern struct ME_FT * ME_FT_create(void);
extern void ME_FT_delete(struct ME_FT *);
extern int ME_FT_add(struct ME_FT *, const char *);
extern void ME_FT_print(struct ME_FT *);
extern int ME_FT_get_index(struct ME_FT *, char *);
extern char * ME_FT_get(struct ME_FT *, int);
extern void ME_FT_encode(struct ME_FT *, int *, char **);
extern void ME_FT_decode(char *, struct ME_FT **);
extern void ME_FT_print_encoded(char *);

enum ME_measurement_type;
struct ME_measurement;

typedef enum ME_measurement_type {
  ME_MEASUREMENT_CALLSTACK, ME_MEASUREMENT_STRING
} ME_measurement_type;

typedef struct ME_CG_AND_FT {
  struct ME_CG * cg;
  struct ME_FT * ft;
} ME_CG_AND_FT;

typedef union ME_measurement_data {
  struct ME_CG_AND_FT cgft;
  char * string_val;

} ME_measurement_data;

typedef struct ME_measurement
{
  //reference to command???
  //when
  int measured; //measurement taken?
  ME_measurement_type type;
  ME_measurement_data data;

  struct ME_measurement * next;
}
  ME_measurement;

extern struct ME_measurement * ME_measurement_create(enum ME_measurement_type);
extern void ME_measurement_delete(struct ME_measurement *);
extern void ME_measurement_print(struct ME_measurement *);
extern json_t * ME_measurement_toJSON(struct ME_measurement *);
extern struct ME_measurement * ME_measurement_fromJSON(json_t *);
extern void ME_measurement_send(int, struct ME_measurement *);
extern void ME_measurement_send_temp(int, struct ME_measurement *);
extern struct ME_measurement * ME_measurement_receive(int);

enum ME_feature_type;
struct ME_feature;

typedef enum {ME_FEATURE_CALLSTACK, ME_FEATURE_VARIABLE, ME_FEATURE_MEMORY} ME_feature_type;

typedef struct ME_feature {
  ME_feature_type type;
  union fdata {
    char var_name[64]; //TODO max length of var_name
    struct ME_feature_memory {
      char address[64];
      char format[64];
    } m;
  } fdata;
} ME_feature;

enum BE_event_type;
struct BE_event_t;
struct BE_event_b;
struct BE_event;

typedef enum {BE_EVENT_T, BE_EVENT_B} BE_event_type;

typedef struct BE_event_t {
  int delay; //int time when
  clock_t start; //int time start
} BE_event_t;

typedef struct BE_event_b {
  int bp_id;
} BE_event_b;

typedef struct BE_event {
  BE_event_type type;
  bool repeat;
  union edata {
    struct BE_event_t t;
    struct BE_event_b b;
  } edata;
} BE_event;

struct BE_hook;

typedef struct BE_hook
{
  BE_event * event;
  struct ME_RLI_IR_expr * action;
  bool enabled;
} BE_hook;

extern struct ME_feature * ME_feature_create_callstack(void);
extern struct ME_feature * ME_feature_create_variable(char *);
extern struct ME_feature * ME_feature_create_memory(char *, char *);
extern void ME_feature_print(struct ME_feature *);

extern struct BE_event * BE_event_t_create(int, int);
extern struct BE_event * BE_event_b_create(int, int);
extern void BE_event_print(struct BE_event *);
extern void BE_hook_print(struct BE_hook *);
extern void BE_hook_disable(struct BE_hook *);
extern void BE_hook_enable(struct BE_hook *);
extern void BE_hook_kill(struct BE_hook *);
extern void BE_hook_array_init(void);
extern int BE_hook_array_add(struct BE_hook *);
extern struct BE_hook * BE_hook_array_get(int);

#endif
