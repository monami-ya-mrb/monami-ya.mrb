/*
** state.c - mrb_state open/close functions
**
** See Copyright Notice in mruby.h
*/

#include <stdlib.h>
#include <string.h>
#include "mruby.h"
#include "mruby/irep.h"
#include "mruby/variable.h"
#include "mruby/debug.h"
#include "mruby/string.h"
#include "mruby/panic.h"
#include "mruby/sandbox.h"

extern struct mrb_sandbox_inib mrb_sandbox_inib_array[];

void mrb_init_heap(mrb_state*);
void mrb_init_core(mrb_state*);
void mrb_init_mrbgems(mrb_state*);
void mrb_final_mrbgems(mrb_state*);

#ifdef MRB_USE_TLSF
#include "tlsf.h"

static tlsf_t tlsf_root_handle = 0;

void
mrb_tlsf_initialize(void *start, size_t bytes)
{
  if (tlsf_root_handle == 0) {
    tlsf_root_handle = tlsf_create_with_pool(start, bytes);
  }
}

/*
 * In this version, it can set only once.
 */
void
mrb_tlsf_set_pool(mrb_state *mrb, size_t bytes)
{
  if (tlsf_root_handle == 0) {
    mrb_panic(NULL);
  }
  if (mrb != NULL) {
    void *ptr;
    ptr = tlsf_malloc(tlsf_root_handle, bytes);
    if (!ptr) {
      mrb_panic(mrb);
    }
    mrb->tlsf_handle = tlsf_create_with_pool(ptr, bytes);
  }
}
#endif

static mrb_value
inspect_main(mrb_state *mrb, mrb_value mod)
{
  return mrb_str_new_lit(mrb, "main");
}

mrb_state*
mrb_open_sandbox_core(mrb_allocf f, void *ud, unsigned int sandbox_id)
{
  static const mrb_state mrb_state_zero = { 0 };
  static const struct mrb_context mrb_context_zero = { 0 };
  mrb_state *mrb;

#ifdef MRB_NAN_BOXING
  mrb_static_assert(sizeof(void*) == 4, "when using NaN boxing sizeof pointer must be 4 byte");
#endif

  mrb = (mrb_state *)(f)(NULL, NULL, sizeof(mrb_state), ud);
  if (mrb == NULL) return NULL;

  *mrb = mrb_state_zero;
  mrb->sandbox_id = sandbox_id;
  mrb->ud = ud;
  mrb->allocf = f;
  mrb->current_white_part = MRB_GC_WHITE_A;
#ifdef USE_MRB_TLSF
  mrb->tlsf_handle = 0;
#endif
  mrb->stack_limit = MRB_STACK_MAX;

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

mrb_state*
mrb_open_core(mrb_allocf f, void *ud)
{
  return mrb_open_sandbox_core(f, ud, 0);
}

void*
mrb_default_allocf(mrb_state *mrb, void *p, size_t size, void *ud)
{
#ifdef MRB_USE_TLSF
  tlsf_t tlsf;
  if (mrb == NULL) {
#if ! defined(MRB_DISABLE_HOSTED)
    if (tlsf_root_handle == 0) {
      void *global_memory;
      const size_t size = 24 * 1024 * 1024;
      global_memory = malloc(size);
      mrb_tlsf_initialize(global_memory, size);
    }
#endif
    tlsf = tlsf_root_handle;
  } else {
    if (mrb->tlsf_handle == 0) {
      mrb_tlsf_set_pool(mrb, 3 * 1024 * 1024);
    }
    tlsf = mrb->tlsf_handle;
  }

  if (!tlsf) {
    mrb_panic(mrb);
  }

  if (size == 0) {
    tlsf_free(tlsf, p);
    return NULL;
  }
  else {
    return tlsf_realloc(tlsf, p, size);
  }
#else
  if (size == 0) {
    free(p);
    return NULL;
  }
  else {
    return realloc(p, size);
  }
#endif
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
mrb_open_sandbox(unsigned int sandbox_id)
{
  mrb_state *mrb = mrb_open_sandbox_allocf(mrb_default_allocf, NULL, sandbox_id);

  return mrb;
}

mrb_state*
mrb_open_sandbox_allocf(mrb_allocf f, void *ud, unsigned int sandbox_id)
{
  mrb_state *mrb = mrb_open_sandbox_core(f, ud, sandbox_id);

#ifndef DISABLE_GEMS
  if (mrb->sandbox_id) {
    mrb_sandbox_inib_array[mrb->sandbox_id - 1].init(mrb);
  } else {
    mrb_init_mrbgems(mrb);
  }
  mrb_gc_arena_restore(mrb, 0);
#endif

  return mrb;
}

mrb_state*
mrb_open(void)
{
  mrb_state *mrb = mrb_open_sandbox_allocf(mrb_default_allocf, NULL, 0);

  return mrb;
}

mrb_state*
mrb_open_allocf(mrb_allocf f, void *ud)
{
  mrb_state *mrb = mrb_open_sandbox_allocf(f, ud, 0);

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
      mrb_gc_free_str(mrb, RSTRING(irep->pool[i]));
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
  mrb_free(mrb, irep->lv);
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
  char *ptr;
  mrb_int len;

  ns = (struct RString *)mrb_malloc(mrb, sizeof(struct RString));
  ns->tt = MRB_TT_STRING;
  ns->c = mrb->string_class;

  if (s->flags & MRB_STR_NOFREE) {
    ns->flags = MRB_STR_NOFREE;
    ns->as.heap.ptr = s->as.heap.ptr;
    ns->as.heap.len = s->as.heap.len;
    ns->as.heap.aux.capa = 0;
  }
  else {
    ns->flags = 0;
    if (s->flags & MRB_STR_EMBED) {
      ptr = s->as.ary;
      len = (mrb_int)((s->flags & MRB_STR_EMBED_LEN_MASK) >> MRB_STR_EMBED_LEN_SHIFT);
    }
    else {
      ptr = s->as.heap.ptr;
      len = s->as.heap.len;
    }

    if (len < RSTRING_EMBED_LEN_MAX) {
      ns->flags |= MRB_STR_EMBED;
      ns->flags &= ~MRB_STR_EMBED_LEN_MASK;
      ns->flags |= (size_t)len << MRB_STR_EMBED_LEN_SHIFT;
      if (ptr) {
        memcpy(ns->as.ary, ptr, len);
      }
      ns->as.ary[len] = '\0';
    }
    else {
      ns->as.heap.ptr = (char *)mrb_malloc(mrb, (size_t)len+1);
      ns->as.heap.len = len;
      ns->as.heap.aux.capa = len;
      if (ptr) {
        memcpy(ns->as.heap.ptr, ptr, len);
      }
      ns->as.heap.ptr[len] = '\0';
    }
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
#ifndef DISABLE_GEMS
  if (mrb->sandbox_id) {
    mrb_sandbox_inib_array[mrb->sandbox_id - 1].final(mrb);
  } else {
    mrb_final_mrbgems(mrb);
  }
  mrb_gc_arena_restore(mrb, 0);
#endif

  /* free */
  mrb_gc_free_gv(mrb);
  mrb_free_context(mrb, mrb->root_c);
  mrb_free_symtbl(mrb);
  mrb_free_heap(mrb);
  mrb_alloca_free(mrb);
#ifndef MRB_GC_FIXED_ARENA
  mrb_free(mrb, mrb->arena);
#endif
#ifdef USE_MRB_TLSF
  if (mrb->tlsf_handle) {
    tlsf_free(tlsf_root_handle, mrb->tlsf_handle);
  }
#endif
  mrb_free(mrb, mrb);
}

mrb_irep*
mrb_add_irep(mrb_state *mrb)
{
  static const mrb_irep mrb_irep_zero = { 0 };
  mrb_irep *irep;

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
