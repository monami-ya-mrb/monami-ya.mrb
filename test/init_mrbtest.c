#include "mruby.h"
#include "mruby/irep.h"
#include "mruby/panic.h"

extern const uint8_t mrbtest_irep[];

void mrbgemtest_init(mrb_state* mrb);
void mrb_capitest_init(mrb_state* mrb);

void
mrb_init_mrbtest(mrb_state *mrb)
{
  mrb_capitest_init(mrb);
  mrb_load_irep(mrb, mrbtest_irep);
#ifndef DISABLE_GEMS
  mrbgemtest_init(mrb);
#endif
  if (mrb->exc) {
    mrb_p(mrb, mrb_obj_value(mrb->exc));
    mrb_panic(mrb);
  }
}

