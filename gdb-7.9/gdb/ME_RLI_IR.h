#ifndef ME_RLI_IR_H
#define ME_RLI_IR_H

#include "ME_common.h"
#include <stdbool.h>

#define MAX_TOKEN_LENGTH 64
#define MAX_STRING_LENGTH 64
#define MAX_SYM_LENGTH 64 //token length and sym length identical??
struct ME_RLI_token;

enum ME_RLI_IR_expr_type;

enum ME_RLI_IR_value_type;

struct ME_RLI_IR_value;
struct ME_RLI_IR_arg;
struct ME_RLI_IR_func;
struct ME_RLI_expr;
struct ME_RLI_IR_sym;

typedef struct ME_RLI_token {
  char value[MAX_TOKEN_LENGTH];
  struct ME_RLI_token * next;
} ME_RLI_token;

typedef struct ME_RLI_IR_arg {
  struct ME_RLI_IR_expr * expr;
  struct ME_RLI_IR_arg * next;
} ME_RLI_IR_arg;

typedef struct ME_RLI_IR_func {
  struct ME_RLI_IR_sym * func_name;
  struct ME_RLI_IR_arg * args;
} ME_RLI_IR_func;

extern struct ME_RLI_token * ME_RLI_token_create(char *);
extern void ME_RLI_token_print(struct ME_RLI_token *);
extern bool _white_space(char);
extern struct ME_RLI_token * ME_RLI_tokenize(char *);

extern struct ME_RLI_IR_value ME_RLI_IR_value_create_int(int);
extern struct ME_RLI_IR_value ME_RLI_IR_value_get_int(struct ME_RLI_IR_value, int *);
extern struct ME_RLI_IR_value ME_RLI_IR_value_create_string(char *);
extern struct ME_RLI_IR_value ME_RLI_IR_value_get_string(struct ME_RLI_IR_value, char **);
extern struct ME_RLI_IR_value ME_RLI_IR_value_create_error(char *);
extern struct ME_RLI_IR_value ME_RLI_IR_value_create_measurement(struct ME_measurement *);
extern struct ME_RLI_IR_value ME_RLI_IR_value_get_measurement(struct ME_RLI_IR_value, struct ME_measurement **);
extern struct ME_RLI_IR_value ME_RLI_IR_value_create_event(struct BE_event *);
extern struct ME_RLI_IR_value ME_RLI_IR_value_get_event(struct ME_RLI_IR_value, struct BE_event **);
extern struct ME_RLI_IR_value ME_RLI_IR_value_create_feature(struct ME_feature *);
extern struct ME_RLI_IR_value ME_RLI_IR_value_get_feature(struct ME_RLI_IR_value, struct ME_feature **);
extern struct ME_RLI_IR_value ME_RLI_IR_value_create_void();
extern struct ME_RLI_IR_value ME_RLI_IR_value_create_lexpr(struct ME_RLI_IR_expr *);
extern struct ME_RLI_IR_value ME_RLI_IR_value_get_lexpr(struct ME_RLI_IR_value, struct ME_RLI_IR_expr **);
extern void ME_RLI_IR_value_print(struct ME_RLI_IR_value);
extern struct ME_RLI_IR_value * ME_RLI_IR_value_parse(struct ME_RLI_token **);

extern struct ME_RLI_IR_expr * ME_RLI_IR_expr_create_value(struct ME_RLI_IR_value *);
extern struct ME_RLI_IR_expr * ME_RLI_IR_expr_create_func(struct ME_RLI_IR_func *);
extern void ME_RLI_IR_expr_print(struct ME_RLI_IR_expr *);
extern struct ME_RLI_IR_expr * ME_RLI_IR_expr_parse(struct ME_RLI_token **);
extern struct ME_RLI_IR_value ME_RLI_IR_expr_eval(struct ME_RLI_IR_expr *);

extern struct ME_RLI_IR_func * ME_RLI_IR_func_create(struct ME_RLI_IR_sym *);
extern int ME_RLI_IR_func_arg_count(struct ME_RLI_IR_func *);
extern void ME_RLI_IR_func_add_arg(struct ME_RLI_IR_func *, struct ME_RLI_IR_expr *);
extern void ME_RLI_IR_func_print(struct ME_RLI_IR_func *);
extern struct ME_RLI_IR_func * ME_RLI_IR_func_parse(struct ME_RLI_token **);
extern struct ME_RLI_IR_value ME_RLI_IR_func_eval(struct ME_RLI_IR_func *);

extern struct ME_RLI_IR_sym * ME_RLI_IR_sym_create(char *);
extern void ME_RLI_IR_sym_print(struct ME_RLI_IR_sym *);
extern struct ME_RLI_IR_sym * ME_RLI_IR_sym_parse(struct ME_RLI_token **);

#endif
