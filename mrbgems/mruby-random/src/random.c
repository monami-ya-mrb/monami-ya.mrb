/*
** random.c - Random module
**
** See Copyright Notice in mruby.h
*/

#include "mruby.h"
#include "mruby/variable.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/array.h"
#include "mt19937ar.h"

#include <time.h>

static char const GLOBAL_RAND_SEED_KEY[] = "$mrb_g_rand_seed";
static char const MT_STATE_KEY[] = "$mrb_i_mt_state";

static const struct mrb_data_type mt_state_type = {
  MT_STATE_KEY, mrb_free,
};

static mrb_value mrb_random_rand(mrb_state *mrb, mrb_value self);
static mrb_value mrb_random_srand(mrb_state *mrb, mrb_value self);

static void 
mt_srand(mt_state *t, unsigned long seed)
{
  mrb_random_init_genrand(t, seed);
}  

static unsigned long 
mt_rand(mt_state *t)
{
  return mrb_random_genrand_int32(t);
}  

static double 
mt_rand_real(mt_state *t)
{
  return mrb_random_genrand_real1(t);
}  

static mrb_value 
mrb_random_mt_srand(mrb_state *mrb, mt_state *t, mrb_value seed)
{ 
  if (mrb_nil_p(seed)) {
    seed = mrb_fixnum_value(time(NULL) + mt_rand(t));
    if (mrb_fixnum(seed) < 0) {
      seed = mrb_fixnum_value( 0 - mrb_fixnum(seed));
    }
  }

  mt_srand(t, (unsigned) mrb_fixnum(seed));

  return seed;
}

static mrb_value 
mrb_random_mt_rand(mrb_state *mrb, mt_state *t, mrb_value max)
{ 
  mrb_value value;

  if (mrb_fixnum(max) == 0) {
    value = mrb_float_value(mrb, mt_rand_real(t));
  }
  else {
    value = mrb_fixnum_value(mt_rand(t) % mrb_fixnum(max));
  }

  return value;
}

static mrb_value 
get_opt(mrb_state* mrb)
{
  mrb_value arg;

  arg = mrb_fixnum_value(0);
  mrb_get_args(mrb, "|o", &arg);

  if (!mrb_nil_p(arg)) {
    if (!mrb_fixnum_p(arg)) {
      mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument type");
    }
    arg = mrb_check_convert_type(mrb, arg, MRB_TT_FIXNUM, "Fixnum", "to_int");
    if (mrb_fixnum(arg) < 0) {
      arg = mrb_fixnum_value(0 - mrb_fixnum(arg));
    }
  }
  return arg;
}

static mrb_value 
mrb_random_g_rand(mrb_state *mrb, mrb_value self)
{
  mrb_value random = mrb_const_get(mrb,
          mrb_obj_value(mrb_class_get(mrb, "Random")),
          mrb_intern_lit(mrb, "DEFAULT"));
  return mrb_random_rand(mrb, random);
}

static mrb_value 
mrb_random_g_srand(mrb_state *mrb, mrb_value self)
{
  mrb_value random = mrb_const_get(mrb,
          mrb_obj_value(mrb_class_get(mrb, "Random")),
          mrb_intern_lit(mrb, "DEFAULT"));
  return mrb_random_srand(mrb, random);
}

static mrb_value 
mrb_random_init(mrb_state *mrb, mrb_value self)
{
  mrb_value seed;
  mt_state *t;
  
  DATA_TYPE(self) = &mt_state_type;
  DATA_PTR(self) = NULL;
  
  /* avoid memory leaks */
  t = (mt_state*)DATA_PTR(self);
  if (t) {
    mrb_free(mrb, t);
  }  

  t = (mt_state *)mrb_malloc(mrb, sizeof(mt_state));
  t->mti = N + 1;

  seed = get_opt(mrb);
  seed = mrb_random_mt_srand(mrb, t, seed);
  if (mrb_nil_p(seed)) {
    t->has_seed = FALSE;
  }
  else {
    mrb_assert(mrb_fixnum_p(seed));
    t->has_seed = TRUE;
    t->seed = mrb_fixnum(seed);
  }
  
  DATA_PTR(self) = t;
  
  return self;
}

static void 
mrb_random_rand_seed(mrb_state *mrb, mt_state *t)
{
  if (!t->has_seed) {
    mrb_random_mt_srand(mrb, t, mrb_nil_value());
  }
}

static mrb_value 
mrb_random_rand(mrb_state *mrb, mrb_value self)
{
  mrb_value max;
  mt_state *t = DATA_PTR(self);

  max = get_opt(mrb);
  mrb_random_rand_seed(mrb, t);
  return mrb_random_mt_rand(mrb, t, max);
}

static mrb_value 
mrb_random_srand(mrb_state *mrb, mrb_value self)
{
  mrb_value seed;
  mrb_value old_seed;
  mt_state *t = DATA_PTR(self);

  seed = get_opt(mrb);
  seed = mrb_random_mt_srand(mrb, t, seed);
  old_seed = t->has_seed? mrb_fixnum_value(t->seed) : mrb_nil_value();
  if (mrb_nil_p(seed)) {
    t->has_seed = FALSE;
  }
  else {
    mrb_assert(mrb_fixnum_p(seed));
    t->has_seed = TRUE;
    t->seed = mrb_fixnum(seed);
  }

  return old_seed;
}

/*
 *  call-seq:
 *     ary.shuffle!   ->   ary
 *
 *  Shuffles elements in self in place.
 */

static mrb_value
mrb_ary_shuffle_bang(mrb_state *mrb, mrb_value ary)
{
  mrb_int i;
  mt_state *random = NULL;
  
  if (RARRAY_LEN(ary) > 1) {
    mrb_get_args(mrb, "|d", &random, &mt_state_type);

    if (random == NULL) {
      mrb_value random_val = mrb_const_get(mrb,
              mrb_obj_value(mrb_class_get(mrb, "Random")),
              mrb_intern_lit(mrb, "DEFAULT"));
      random = (mt_state *)DATA_PTR(random_val);
    }
    mrb_random_rand_seed(mrb, random);
  
    mrb_ary_modify(mrb, mrb_ary_ptr(ary));
    
    for (i = RARRAY_LEN(ary) - 1; i > 0; i--)  {
      mrb_int j;
      mrb_value tmp;
      
      j = mrb_fixnum(mrb_random_mt_rand(mrb, random, mrb_fixnum_value(RARRAY_LEN(ary))));
      
      tmp = RARRAY_PTR(ary)[i];
      RARRAY_PTR(ary)[i] = RARRAY_PTR(ary)[j];
      RARRAY_PTR(ary)[j] = tmp;
    }    
  }
  
  return ary;
}

/*
 *  call-seq:
 *     ary.shuffle   ->   new_ary
 *
 *  Returns a new array with elements of self shuffled.
 */

static mrb_value
mrb_ary_shuffle(mrb_state *mrb, mrb_value ary)
{
  mrb_value new_ary = mrb_ary_new_from_values(mrb, RARRAY_LEN(ary), RARRAY_PTR(ary));
  mrb_ary_shuffle_bang(mrb, new_ary);
  
  return new_ary;
}

void mrb_mruby_random_gem_init(mrb_state *mrb)
{
  struct RClass *random;
  struct RClass *array = mrb->array_class;

  mrb_define_method(mrb, mrb->kernel_module, "rand", mrb_random_g_rand, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, mrb->kernel_module, "srand", mrb_random_g_srand, MRB_ARGS_OPT(1));

  random = mrb_define_class(mrb, "Random", mrb->object_class);
  MRB_SET_INSTANCE_TT(random, MRB_TT_DATA);
  mrb_define_class_method(mrb, random, "rand", mrb_random_g_rand, MRB_ARGS_OPT(1));
  mrb_define_class_method(mrb, random, "srand", mrb_random_g_srand, MRB_ARGS_OPT(1));

  mrb_define_method(mrb, random, "initialize", mrb_random_init, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, random, "rand", mrb_random_rand, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, random, "srand", mrb_random_srand, MRB_ARGS_OPT(1));
  
  mrb_define_method(mrb, array, "shuffle", mrb_ary_shuffle, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, array, "shuffle!", mrb_ary_shuffle_bang, MRB_ARGS_OPT(1));

  mrb_const_set(mrb, mrb_obj_value(random), mrb_intern_lit(mrb, "DEFAULT"),
          mrb_obj_new(mrb, random, 0, NULL));
}

void mrb_mruby_random_gem_final(mrb_state *mrb)
{
}
