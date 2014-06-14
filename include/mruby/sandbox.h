#ifndef MRUBY_SANDBOX_H__
#define MRUBY_SANDBOX_H__

struct mrb_sandbox_inib {
  void (*init)(mrb_state *);
  void (*final)(mrb_state *);
};

#endif /* MRUBY_SANDBOX_H__ */
