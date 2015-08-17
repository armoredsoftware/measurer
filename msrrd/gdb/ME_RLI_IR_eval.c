#include "ME_RLI_IR_API.h"
#include "ME_RLI_IR.h"
#include "ME_RLI_IR_eval.h"

ME_RLI_IR_value ME_RLI_IR_expr_eval(ME_RLI_IR_expr * expr) {
  if (expr->type == ME_RLI_IR_EXPR_VALUE) {
    return (*expr->data.value);
  }
  else if (expr->type == ME_RLI_IR_EXPR_FUNC) {
    return ME_RLI_IR_func_eval(expr->data.func);
  }
  else {
    printf("Expected a value or a function!\n");
    exit(-1);
  }
}


ME_RLI_IR_value ME_RLI_IR_func_eval(ME_RLI_IR_func * func) {
  int args_count, i;

  //evaluate arguments
  args_count = ME_RLI_IR_func_arg_count(func);
  ME_RLI_IR_value arg_vals[args_count];
  i = 0;
  ME_RLI_IR_arg * arg_curr = func->args;
  for (i = 0; i<args_count; i++) {
    arg_vals[i] = ME_RLI_IR_expr_eval(arg_curr->expr);

    //If error, return
    if (arg_vals[i].type == ME_RLI_IR_VALUE_ERROR) {
      return arg_vals[i];
    }

    arg_curr = arg_curr->next;
  }
  //resolve function name and evaluate
  ME_RLI_API_func f = ME_RLI_API_func_look_up(func->func_name->value);
  if (!f) return ME_RLI_IR_value_create_error("No such function!");
  ME_RLI_IR_value result;
  result = (*f)(arg_vals,args_count);
  return result;
}
