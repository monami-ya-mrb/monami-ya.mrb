#include "mruby.h"
#include "mruby/string.h"

/* Each test function should have test_mrb_ prefix. */
/* The name of test class initializer must be 'test_mrb_#{filename}_init'. */

mrb_value
test_mrb_str_to_cstr(mrb_state *mrb, mrb_value self)
{
  mrb_value str;

  mrb_get_args(mrb, "o", &str);

  (void *)mrb_str_to_cstr(mrb, str);
}

void
test_mrb_str_to_cstr_init(mrb_state *mrb)
{
  struct RClass *str_test;

  str_test = mrb_define_module(mrb, "CAPITest_mrb_str_to_cstr");
  mrb_define_module_function(mrb, str_test, "mrb_str_to_cstr", test_mrb_str_to_cstr, ARGS_REQ(1));
}
