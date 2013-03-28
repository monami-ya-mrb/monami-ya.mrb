#include "mruby.h"
#include "mruby/panic.h"

/* Each test function should have test_mrb_ prefix. */
/* The name of test class initializer must be 'test_mrb_#{filename}_init'. */

static mrb_bool panic_called_p = FALSE;

static void
expected_panic(mrb_state *mrb)
{
  if (mrb) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "Expected Error.");
  }
  else {
    panic_called_p = TRUE;
  }
}

static void
unexpected_panic(mrb_state *mrb)
{
}

mrb_value
test_1_mrb_panic(mrb_state *mrb, mrb_value self)
{
  /* mrb_state local panic hook */
  mrb_panic_set(mrb, expected_panic);
  mrb_panic_reset(mrb, expected_panic);

  return mrb_true_value();
}

mrb_value
test_2_mrb_panic(mrb_state *mrb, mrb_value self)
{
  /* state local panic hook */
  mrb_panic_set(mrb, expected_panic);
  mrb_panic_reset(mrb, unexpected_panic); /* ArgumentError expected. */

  return mrb_false_value();
}

mrb_value
test_3_mrb_panic(mrb_state *mrb, mrb_value self)
{
  /* global panic hook */
  mrb_panic_set(NULL, expected_panic);
  mrb_panic_reset(NULL, expected_panic);

  return mrb_bool_value(mrb_panic_get_global_hook() == NULL);
}

mrb_value
test_4_mrb_panic(mrb_state *mrb, mrb_value self)
{
  /* state local panic hook */
  mrb_panic_set(NULL, expected_panic);
  mrb_panic_reset(NULL, unexpected_panic);

  return mrb_bool_value(mrb_panic_get_global_hook() == expected_panic);
}

mrb_value
test_5_mrb_panic(mrb_state *mrb, mrb_value self)
{
  /* state local panic hook */
  mrb_panic_set(mrb, expected_panic);

  mrb_panic(mrb); /* RuntimeError expected. */

  return mrb_false_value();
}

mrb_value
test_6_mrb_panic(mrb_state *mrb, mrb_value self)
{
  /* global panic hook */
  panic_called_p = FALSE;
  mrb_panic_set(NULL, expected_panic);

  mrb_panic(NULL); /* RuntimeError expected. */

  return mrb_bool_value(panic_called_p);
}

void
test_mrb_panic_init(mrb_state *mrb)
{
  struct RClass *test;

  test = mrb_define_module(mrb, "CAPITest_mrb_panic");
  mrb_define_module_function(mrb, test, "test1", test_1_mrb_panic, ARGS_NONE());
  mrb_define_module_function(mrb, test, "test2", test_2_mrb_panic, ARGS_NONE());
  mrb_define_module_function(mrb, test, "test3", test_3_mrb_panic, ARGS_NONE());
  mrb_define_module_function(mrb, test, "test4", test_4_mrb_panic, ARGS_NONE());
  mrb_define_module_function(mrb, test, "test5", test_5_mrb_panic, ARGS_NONE());
  mrb_define_module_function(mrb, test, "test6", test_6_mrb_panic, ARGS_NONE());
}
