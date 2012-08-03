/*
** state.c - RiteVM open/close functions
**
** See Copyright Notice in mruby.h
*/

#include "tlsf.h"

#include "mruby.h"
#include "mruby/irep.h"
#include <string.h>

void mrb_init_heap(mrb_state*);
void mrb_init_core(mrb_state*);
void mrb_init_ext(mrb_state*);

#ifndef TLSF_POOL_SIZE
#define TLSF_POOL_SIZE (1024 * 10240)
#endif

static char memory_pool[TLSF_POOL_SIZE];

mrb_state*
mrb_open_allocf(mrb_allocf f)
{
  mrb_state *mrb = (mrb_state *)(f)(NULL, NULL, sizeof(mrb_state));
  if (mrb == NULL) return NULL;

  memset(mrb, 0, sizeof(mrb_state));
  mrb->allocf = f;
  mrb->current_white_part = MRB_GC_WHITE_A;

  mrb_init_heap(mrb);
  mrb_init_core(mrb);
  mrb_init_ext(mrb);
  return mrb;
}

static void*
allocf(mrb_state *mrb, void *p, size_t size)
{
  if (size == 0) {
    free_ex(p, memory_pool);
    return NULL;
  }
  else {
    return realloc_ex(p, size, memory_pool);
  }
}

mrb_state*
mrb_open()
{
  mrb_state *mrb;

  init_memory_pool(TLSF_POOL_SIZE, memory_pool);
  mrb = mrb_open_allocf(allocf);

  return mrb;
}

void
mrb_close(mrb_state *mrb)
{
  int i;

  /* free */
  mrb_free(mrb, mrb->stbase);
  mrb_free(mrb, mrb->cibase);
  for (i=0; i<mrb->irep_len; i++) {
    if (mrb->irep[i]->flags & MRB_IREP_NOFREE) continue;
    if ((mrb->irep[i]->flags & MRB_ISEQ_NOFREE) == 0) {
      mrb_free(mrb, mrb->irep[i]->iseq);
    }
    mrb_free(mrb, mrb->irep[i]->pool);
    mrb_free(mrb, mrb->irep[i]->syms);
    mrb_free(mrb, mrb->irep[i]);
  }
  mrb_free(mrb, mrb->irep);
  mrb_free(mrb, mrb);
}

void
mrb_add_irep(mrb_state *mrb, int idx)
{
  if (!mrb->irep) {
    int max = 256;

    if (idx > max) max = idx+1;
    mrb->irep = (mrb_irep **)mrb_calloc(mrb, max, sizeof(mrb_irep*));
    mrb->irep_capa = max;
  }
  else if (mrb->irep_capa <= idx) {
    while (mrb->irep_capa <= idx) {
      mrb->irep_capa *= 2;
    }
    mrb->irep = (mrb_irep **)mrb_realloc(mrb, mrb->irep, sizeof(mrb_irep*)*mrb->irep_capa);
  }
}

mrb_value
mrb_top_self(mrb_state *mrb)
{
  // for now
  return mrb_nil_value();
}
