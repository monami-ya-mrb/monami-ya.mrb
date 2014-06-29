/*
** mrb_state.c - mrb_state utility functions
**
** Copyright (C) 2014 Monami-ya LLC, Japan.
** MIT Livense
*/

#include "mruby.h"

static mrb_value
state_stack_limit_get(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(mrb->stack_limit);
}

static mrb_value
state_stack_limit_set(mrb_state *mrb, mrb_value obj)
{
  mrb_int stack_limit;
  ptrdiff_t size = mrb->c->stend - mrb->c->stbase;

  mrb_get_args(mrb, "i", &stack_limit);
  if (stack_limit < size) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "Smaller than the current stack size.");
  }
  mrb->stack_limit = stack_limit;

  return mrb_nil_value();
}

void
mrb_init_state(mrb_state *mrb)
{
  struct RClass *clazz;

  clazz = mrb_define_module(mrb, "MrbState");

  mrb_define_class_method(mrb, clazz, "stack_limit", state_stack_limit_get, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, clazz, "stack_limit=", state_stack_limit_set, MRB_ARGS_REQ(1));
}
