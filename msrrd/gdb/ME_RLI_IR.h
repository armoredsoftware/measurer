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

typedef enum ME_RLI_IR_value_type {
  ME_RLI_IR_VALUE_INT, ME_RLI_IR_VALUE_STRING, ME_RLI_IR_VALUE_VOID, ME_RLI_IR_VALUE_LEXPR,
  ME_RLI_IR_VALUE_MEASUREMENT, ME_RLI_IR_VALUE_EVENT, ME_RLI_IR_VALUE_FEATURE, ME_RLI_IR_VALUE_ERROR
}
  ME_RLI_IR_value_type;

typedef struct ME_RLI_IR_value {
  ME_RLI_IR_value_type type;
  union vdata {
    int int_val;
    char string_val[MAX_STRING_LENGTH];
    struct ME_RLI_IR_expr * lexpr;
    struct ME_measurement * ms;
    struct BE_event * event;
    struct ME_feature * feature;
    char error_desc[MAX_STRING_LENGTH];
  } vdata;
}
  ME_RLI_IR_value;


typedef struct ME_RLI_token {
  char value[MAX_TOKEN_LENGTH];
  struct ME_RLI_token * next;
} ME_RLI_token;

typedef struct ME_RLI_IR_arg {
  struct ME_RLI_IR_expr * expr;
  struct ME_RLI_IR_arg * next;
} ME_RLI_IR_arg;

typedef struct ME_RLI_IR_sym {
  char value[MAX_SYM_LENGTH];
}
  ME_RLI_IR_sym;

typedef struct ME_RLI_IR_func {
  struct ME_RLI_IR_sym * func_name;
  struct ME_RLI_IR_arg * args;
} ME_RLI_IR_func;


typedef enum ME_RLI_IR_expr_type {
  ME_RLI_IR_EXPR_VALUE, ME_RLI_IR_EXPR_FUNC
}
  ME_RLI_IR_expr_type;

typedef struct ME_RLI_IR_expr {
  ME_RLI_IR_expr_type type;
  union expr_data {
    struct ME_RLI_IR_value * value;
    struct ME_RLI_IR_func * func;
  } data;
}
  ME_RLI_IR_expr;

extern struct ME_RLI_token * ME_RLI_token_create(char *);
extern void ME_RLI_token_print(struct ME_RLI_token *);
extern struct ME_RLI_token * ME_RLI_tokenize(char *);
extern bool _white_space(char);
//extern struct ME_RLI_token * ME_RLI_tokenize(char *);

extern struct ME_RLI_IR_value ME_RLI_IR_value_create_int(int);
extern struct ME_RLI_IR_value ME_RLI_IR_value_get_int(struct ME_RLI_IR_value, int *);
extern struct ME_RLI_IR_value ME_RLI_IR_value_create_string(const char *);
extern struct ME_RLI_IR_value ME_RLI_IR_value_get_string(struct ME_RLI_IR_value, char **);
extern struct ME_RLI_IR_value ME_RLI_IR_value_create_error(const char *);
extern struct ME_RLI_IR_value ME_RLI_IR_value_create_measurement(struct ME_measurement *);
extern struct ME_RLI_IR_value ME_RLI_IR_value_get_measurement(struct ME_RLI_IR_value, struct ME_measurement **);
extern struct ME_RLI_IR_value ME_RLI_IR_value_create_event(struct BE_event *);
extern struct ME_RLI_IR_value ME_RLI_IR_value_get_event(struct ME_RLI_IR_value, struct BE_event **);
extern struct ME_RLI_IR_value ME_RLI_IR_value_create_feature(struct ME_feature *);
extern struct ME_RLI_IR_value ME_RLI_IR_value_get_feature(struct ME_RLI_IR_value, struct ME_feature **);
extern struct ME_RLI_IR_value ME_RLI_IR_value_create_void(void);
extern struct ME_RLI_IR_value ME_RLI_IR_value_create_lexpr(struct ME_RLI_IR_expr *);
extern struct ME_RLI_IR_value ME_RLI_IR_value_get_lexpr(struct ME_RLI_IR_value, struct ME_RLI_IR_expr **);
extern void ME_RLI_IR_value_print(struct ME_RLI_IR_value);
extern struct ME_RLI_IR_value ME_RLI_IR_value_fromJSON(struct json_t *);
extern struct ME_RLI_IR_value * ME_RLI_IR_value_parse(struct ME_RLI_token **);

extern struct ME_RLI_IR_expr * ME_RLI_IR_expr_create_value(struct ME_RLI_IR_value *);
extern struct ME_RLI_IR_expr * ME_RLI_IR_expr_create_func(struct ME_RLI_IR_func *);
extern void ME_RLI_IR_expr_print(struct ME_RLI_IR_expr *);
extern struct ME_RLI_IR_expr * ME_RLI_IR_expr_parse(struct ME_RLI_token **);

extern struct ME_RLI_IR_func * ME_RLI_IR_func_create(struct ME_RLI_IR_sym *);
extern int ME_RLI_IR_func_arg_count(struct ME_RLI_IR_func *);
extern void ME_RLI_IR_func_add_arg(struct ME_RLI_IR_func *, struct ME_RLI_IR_expr *);
extern void ME_RLI_IR_func_print(struct ME_RLI_IR_func *);
extern struct ME_RLI_IR_func * ME_RLI_IR_func_parse(struct ME_RLI_token **);

extern struct ME_RLI_IR_sym * ME_RLI_IR_sym_create(char *);
extern void ME_RLI_IR_sym_print(struct ME_RLI_IR_sym *);
extern struct ME_RLI_IR_sym * ME_RLI_IR_sym_parse(struct ME_RLI_token **);

#endif
