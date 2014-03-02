/*
** state.c - mrb_state open/close functions
**
** See Copyright Notice in mruby.h
*/

#include <string.h>
#include "mruby.h"
#include "mruby/irep.h"
#include "mruby/variable.h"
#include "mruby/debug.h"
#include "mruby/string.h"

#include "tlsf.h"

#include <stdlib.h>
void mrb_init_heap(mrb_state*);
void mrb_init_core(mrb_state*);
void mrb_final_core(mrb_state*);

#ifdef TLSF_HEAP_SIZE
static char memory_pool[TLSF_HEAP_SIZE];
#undef free(p)
#define free(p)			do { free_ex((p), memory_pool); } while(0)
#undef realloc(p, size)
#define realloc(p, size)	(realloc_ex((p), (size), memory_pool))
#endif

static mrb_value
inspect_main(mrb_state *mrb, mrb_value mod)
{
  return mrb_str_new_lit(mrb, "main");
}

mrb_state*
mrb_open_allocf(mrb_allocf f, uintptr_t ud)
{
  static const mrb_state mrb_state_zero = { 0 };
  static const struct mrb_context mrb_context_zero = { 0 };
  mrb_state *mrb;

#ifdef MRB_NAN_BOXING
# ifdef MRB_SUPPORT_STATIC_ASSERT
  MRB_STATIC_ASSERT(sizeof(void*) == 4, "Can use MRB_NAN_BOXING on (sizeof(void*) == 4) environments only.");
# else
  mrb_assert(sizeof(void*) == 4);
# endif
#endif

  mrb = (mrb_state *)(f)(NULL, NULL, sizeof(mrb_state), ud);
  if (mrb == NULL) return NULL;

  *mrb = mrb_state_zero;
  mrb->ud = ud;
  mrb->allocf = f;
  mrb->current_white_part = MRB_GC_WHITE_A;

#ifndef MRB_GC_FIXED_ARENA
  mrb->arena = (struct RBasic**)mrb_malloc(mrb, sizeof(struct RBasic*)*MRB_GC_ARENA_SIZE);
  mrb->arena_capa = MRB_GC_ARENA_SIZE;
#endif

  mrb_init_heap(mrb);
  mrb->c = (struct mrb_context*)mrb_malloc(mrb, sizeof(struct mrb_context));
  *mrb->c = mrb_context_zero;
  mrb->root_c = mrb->c;
  mrb_init_core(mrb);

  return mrb;
}

static void*
allocf(mrb_state *mrb, void *p, size_t size, uintptr_t ud)
{
  if (size == 0) {
    free(p);
    return NULL;
  }
  else {
    return realloc(p, size);
  }
}

struct alloca_header {
  struct alloca_header *next;
  char buf[];
};

void*
mrb_alloca(mrb_state *mrb, size_t size)
{
  struct alloca_header *p;

  p = (struct alloca_header*) mrb_malloc(mrb, sizeof(struct alloca_header)+size);
  if (p == NULL) return NULL;
  p->next = mrb->mems;
  mrb->mems = p;
  return (void*)p->buf;
}

static void
mrb_alloca_free(mrb_state *mrb)
{
  struct alloca_header *p;
  struct alloca_header *tmp;

  if (mrb == NULL) return;
  p = mrb->mems;

  while (p) {
    tmp = p;
    p = p->next;
    mrb_free(mrb, tmp);
  }
}

mrb_state*
mrb_open(void)
{
  mrb_state *mrb;

#ifdef TLSF_HEAP_SIZE
  init_memory_pool(TLSF_HEAP_SIZE, memory_pool);
#endif
  mrb = mrb_open_allocf(allocf, (uintptr_t)NULL);

  return mrb;
}

void mrb_free_symtbl(mrb_state *mrb);
void mrb_free_heap(mrb_state *mrb);

void
mrb_irep_incref(mrb_state *mrb, mrb_irep *irep)
{
  irep->refcnt++;
}

void
mrb_irep_decref(mrb_state *mrb, mrb_irep *irep)
{
  irep->refcnt--;
  if (irep->refcnt == 0) {
    mrb_irep_free(mrb, irep);
  }
}

void
mrb_irep_free(mrb_state *mrb, mrb_irep *irep)
{
  size_t i;

  if (!(irep->flags & MRB_ISEQ_NO_FREE))
    mrb_free(mrb, irep->iseq);
  for (i=0; i<irep->plen; i++) {
    if (mrb_type(irep->pool[i]) == MRB_TT_STRING) {
      if ((mrb_str_ptr(irep->pool[i])->flags & MRB_STR_NOFREE) == 0) {
        mrb_free(mrb, mrb_str_ptr(irep->pool[i])->ptr);
      }
      mrb_free(mrb, mrb_obj_ptr(irep->pool[i]));
    }
#ifdef MRB_WORD_BOXING
    else if (mrb_type(irep->pool[i]) == MRB_TT_FLOAT) {
      mrb_free(mrb, mrb_obj_ptr(irep->pool[i]));
    }
#endif
  }
  mrb_free(mrb, irep->pool);
  mrb_free(mrb, irep->syms);
  for (i=0; i<irep->rlen; i++) {
    mrb_irep_decref(mrb, irep->reps[i]);
  }
  mrb_free(mrb, irep->reps);
  mrb_free(mrb, (void *)irep->filename);
  mrb_free(mrb, irep->lines);
  mrb_debug_info_free(mrb, irep->debug_info);
  mrb_free(mrb, irep);
}

mrb_value
mrb_str_pool(mrb_state *mrb, mrb_value str)
{
  struct RString *s = mrb_str_ptr(str);
  struct RString *ns;
  mrb_int len;

  ns = (struct RString *)mrb_malloc(mrb, sizeof(struct RString));
  ns->tt = MRB_TT_STRING;
  ns->c = mrb->string_class;

  len = s->len;
  ns->len = len;
  if (s->flags & MRB_STR_NOFREE) {
    ns->ptr = s->ptr;
    ns->flags = MRB_STR_NOFREE;
  }
  else {
    ns->flags = 0;
    ns->ptr = (char *)mrb_malloc(mrb, (size_t)len+1);
    if (s->ptr) {
      memcpy(ns->ptr, s->ptr, len);
    }
    ns->ptr[len] = '\0';
  }
  return mrb_obj_value(ns);
}

void
mrb_free_context(mrb_state *mrb, struct mrb_context *c)
{
  if (!c) return;
  mrb_free(mrb, c->stbase);
  mrb_free(mrb, c->cibase);
  mrb_free(mrb, c->rescue);
  mrb_free(mrb, c->ensure);
  mrb_free(mrb, c);
}

void
mrb_close(mrb_state *mrb)
{
  mrb_final_core(mrb);

  /* free */
  mrb_gc_free_gv(mrb);
  mrb_free_context(mrb, mrb->root_c);
  mrb_free_symtbl(mrb);
  mrb_free_heap(mrb);
  mrb_alloca_free(mrb);
#ifndef MRB_GC_FIXED_ARENA
  mrb_free(mrb, mrb->arena);
#endif
  mrb_free(mrb, mrb);
}

mrb_irep*
mrb_add_irep(mrb_state *mrb)
{
  static const mrb_irep mrb_irep_zero = { 0 };
  mrb_irep *irep;
  size_t i;

  irep = (mrb_irep *)mrb_malloc(mrb, sizeof(mrb_irep));
  *irep = mrb_irep_zero;
  irep->refcnt = 1;

  return irep;
}

mrb_value
mrb_top_self(mrb_state *mrb)
{
  if (!mrb->top_self) {
    mrb->top_self = (struct RObject*)mrb_obj_alloc(mrb, MRB_TT_OBJECT, mrb->object_class);
    mrb_define_singleton_method(mrb, mrb->top_self, "inspect", inspect_main, MRB_ARGS_NONE());
    mrb_define_singleton_method(mrb, mrb->top_self, "to_s", inspect_main, MRB_ARGS_NONE());
  }
  return mrb_obj_value(mrb->top_self);
}
