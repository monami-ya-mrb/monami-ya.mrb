/*
** disas.c - RITE bytecode disassembler.
**
** Copyright (c) mruby developers 2010-2012
** Copyright (c) Monami-ya LLC, Japan.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** [ MIT license: http://www.opensource.org/licenses/mit-license.php ]
**/

#include "mruby.h"
#include "mruby/irep.h"
#include "ritevm/opcode.h"
#include <stdio.h>

void
mmrb_disasm_once(mrb_state *mrb, mrb_irep *irep, mrb_code c)
{
  int i = 0;
  switch (GET_OPCODE(c)) {
  case OP_NOP:
    printf("OP_NOP\n");
    break;
  case OP_MOVE:
    printf("OP_MOVE\tR%d\tR%d\n", GETARG_A(c), GETARG_B(c));
    break;
  case OP_LOADL:
    printf("OP_LOADL\tR%d\tL(%d)\n", GETARG_A(c), GETARG_Bx(c));
    break;
  case OP_LOADI:
    printf("OP_LOADI\tR%d\t%d\n", GETARG_A(c), GETARG_sBx(c));
    break;
  case OP_LOADSYM:
    printf("OP_LOADSYM\tR%d\t:%s\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_Bx(c)]));
    break;
  case OP_LOADNIL:
    printf("OP_LOADNIL\tR%d\n", GETARG_A(c));
    break;
  case OP_LOADSELF:
    printf("OP_LOADSELF\tR%d\n", GETARG_A(c));
    break;
  case OP_LOADT:
    printf("OP_LOADT\tR%d\n", GETARG_A(c));
    break;
  case OP_LOADF:
    printf("OP_LOADF\tR%d\n", GETARG_A(c));
    break;
  case OP_GETGLOBAL:
    printf("OP_GETGLOBAL\tR%d\t:%s\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_Bx(c)]));
    break;
  case OP_SETGLOBAL:
    printf("OP_SETGLOBAL\t:%s\tR%d\n",
	   mrb_sym2name(mrb, irep->syms[GETARG_Bx(c)]),
	   GETARG_A(c));
    break;
  case OP_GETCONST:
    printf("OP_GETCONST\tR%d\t:%s\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_Bx(c)]));
    break;
  case OP_SETCONST:
    printf("OP_SETCONST\t:%s\tR%d\n",
	   mrb_sym2name(mrb, irep->syms[GETARG_Bx(c)]),
	   GETARG_A(c));
    break;
  case OP_GETMCNST:
    printf("OP_GETMCNST\tR%d\tR%d::%s\n", GETARG_A(c), GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_Bx(c)]));
    break;
  case OP_SETMCNST:
    printf("OP_SETMCNST\tR%d::%s\tR%d\n", GETARG_A(c)+1,
	   mrb_sym2name(mrb, irep->syms[GETARG_Bx(c)]),
	   GETARG_A(c));
    break;
  case OP_GETIV:
    printf("OP_GETIV\tR%d\t%s\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_Bx(c)]));
    break;
  case OP_SETIV:
    printf("OP_SETIV\t%s\tR%d\n",
	   mrb_sym2name(mrb, irep->syms[GETARG_Bx(c)]),
	   GETARG_A(c));
    break;
  case OP_GETUPVAR:
    printf("OP_GETUPVAR\tR%d\t%d\t%d\n",
	   GETARG_A(c), GETARG_B(c), GETARG_C(c));
    break;
  case OP_SETUPVAR:
    printf("OP_SETUPVAR\tR%d\t%d\t%d\n",
	   GETARG_A(c), GETARG_B(c), GETARG_C(c));
    break;
  case OP_GETCV:
    printf("OP_GETCV\tR%d\t%s\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_Bx(c)]));
    break;
  case OP_SETCV:
    printf("OP_SETCV\t%s\tR%d\n",
	   mrb_sym2name(mrb, irep->syms[GETARG_Bx(c)]),
	   GETARG_A(c));
    break;
  case OP_JMP:
    printf("OP_JMP\t\t%03d\n", i+GETARG_sBx(c));
    break;
  case OP_JMPIF:
    printf("OP_JMPIF\tR%d\t%03d\n", GETARG_A(c), i+GETARG_sBx(c));
    break;
  case OP_JMPNOT:
    printf("OP_JMPNOT\tR%d\t%03d\n", GETARG_A(c), i+GETARG_sBx(c));
    break;
  case OP_SEND:
    printf("OP_SEND\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;
  case OP_SENDB:
    printf("OP_SENDB\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;
  case OP_FSEND:
    printf("OP_FSEND\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;
  case OP_CALL:
    printf("OP_CALL\tR%d\n", GETARG_A(c));
    break;
  case OP_TAILCALL:
    printf("OP_TAILCALL\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;
  case OP_SUPER:
    printf("OP_SUPER\tR%d\t%d\n", GETARG_A(c),
	   GETARG_C(c));
    break;
  case OP_ARGARY:
    printf("OP_ARGARY\tR%d\t%d:%d:%d:%d\n", GETARG_A(c),
	   (GETARG_Bx(c)>>10)&0x3f,
	   (GETARG_Bx(c)>>9)&0x1,
	   (GETARG_Bx(c)>>4)&0x1f,
	   (GETARG_Bx(c)>>0)&0xf);
    break;

  case OP_ENTER:
    printf("OP_ENTER\t%d:%d:%d:%d:%d:%d:%d\n",
	   (GETARG_Ax(c)>>18)&0x1f,
	   (GETARG_Ax(c)>>13)&0x1f,
	   (GETARG_Ax(c)>>12)&0x1,
	   (GETARG_Ax(c)>>7)&0x1f,
	   (GETARG_Ax(c)>>2)&0x1f,
	   (GETARG_Ax(c)>>1)&0x1,
	   GETARG_Ax(c) & 0x1);
    break;
  case OP_RETURN:
    printf("OP_RETURN\tR%d", GETARG_A(c));
    switch (GETARG_B(c)) {
    case OP_R_NORMAL:
      printf("\n"); break;
    case OP_R_RETURN:
      printf("\treturn\n"); break;
    case OP_R_BREAK:
      printf("\tbreak\n"); break;
    default:
      printf("\tbroken\n"); break;
      break;
    }
    break;
  case OP_BLKPUSH:
    printf("OP_BLKPUSH\tR%d\t%d:%d:%d:%d\n", GETARG_A(c),
	   (GETARG_Bx(c)>>10)&0x3f,
	   (GETARG_Bx(c)>>9)&0x1,
	   (GETARG_Bx(c)>>4)&0x1f,
	   (GETARG_Bx(c)>>0)&0xf);
    break;

  case OP_LAMBDA:
    printf("OP_LAMBDA\tR%d\tI(%+d)\t%d\n", GETARG_A(c), GETARG_b(c), GETARG_c(c));
    break;
  case OP_RANGE:
    printf("OP_RANGE\tR%d\tR%d\t%d\n", GETARG_A(c), GETARG_B(c), GETARG_C(c));
    break;
  case OP_METHOD:
    printf("OP_METHOD\tR%d\t:%s\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]));
    break;

  case OP_ADD:
    printf("OP_ADD\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;
  case OP_ADDI:
    printf("OP_ADDI\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;
  case OP_SUB:
    printf("OP_SUB\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;
  case OP_SUBI:
    printf("OP_SUBI\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;
  case OP_MUL:
    printf("OP_MUL\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;
  case OP_DIV:
    printf("OP_DIV\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;
  case OP_LT:
    printf("OP_LT\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;
  case OP_LE:
    printf("OP_LE\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;
  case OP_GT:
    printf("OP_GT\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;
  case OP_GE:
    printf("OP_GE\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;
  case OP_EQ:
    printf("OP_EQ\tR%d\t:%s\t%d\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]),
	   GETARG_C(c));
    break;

  case OP_STOP:
    printf("OP_STOP\n");
    break;

  case OP_ARRAY:
    printf("OP_ARRAY\tR%d\tR%d\t%d\n", GETARG_A(c), GETARG_B(c), GETARG_C(c));
    break;
  case OP_ARYCAT:
    printf("OP_ARYCAT\tR%d\tR%d\n", GETARG_A(c), GETARG_B(c));
    break;
  case OP_ARYPUSH:
    printf("OP_ARYPUSH\tR%d\tR%d\n", GETARG_A(c), GETARG_B(c));
    break;
  case OP_AREF:
    printf("OP_AREF\tR%d\tR%d\t%d\n", GETARG_A(c), GETARG_B(c), GETARG_C(c));
    break;
  case OP_APOST:
    printf("OP_APOST\tR%d\t%d\t%d\n", GETARG_A(c), GETARG_B(c), GETARG_C(c));
    break;
  case OP_STRING:
    {
      /*      mrb_value s = irep->pool[GETARG_Bx(c)];
	
      s = mrb_str_dump(mrb, s);
      printf("OP_STRING\tR%d\t%s\n", GETARG_A(c), RSTRING_PTR(s));*/
      printf("OP_STRING\n");
    }
    break;
  case OP_STRCAT:
    printf("OP_STRCAT\tR%d\tR%d\n", GETARG_A(c), GETARG_B(c));
    break;
  case OP_HASH:
    printf("OP_HASH\tR%d\tR%d\t%d\n", GETARG_A(c), GETARG_B(c), GETARG_C(c));
    break;

  case OP_OCLASS:
    printf("OP_OCLASS\tR%d\n", GETARG_A(c));
    break;
  case OP_CLASS:
    printf("OP_CLASS\tR%d\t:%s\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]));
    break;
  case OP_MODULE:
    printf("OP_MODULE\tR%d\t:%s\n", GETARG_A(c),
	   mrb_sym2name(mrb, irep->syms[GETARG_B(c)]));
    break;
  case OP_EXEC:
    printf("OP_EXEC\tR%d\tI(%d)\n", GETARG_A(c), GETARG_Bx(c));
    break;
  case OP_SCLASS:
    printf("OP_SCLASS\tR%d\tR%d\n", GETARG_A(c), GETARG_B(c));
    break;
  case OP_TCLASS:
    printf("OP_TCLASS\tR%d\n", GETARG_A(c));
    break;
  case OP_ERR:
    printf("OP_ERR\tL(%d)\n", GETARG_Bx(c));
    break;
  case OP_EPUSH:
    printf("OP_EPUSH\t:I(%d)\n", GETARG_Bx(c));
    break;
  case OP_ONERR:
    printf("OP_ONERR\t%03d\n", i+GETARG_sBx(c));
    break;
  case OP_RESCUE:
    printf("OP_RESCUE\tR%d\n", GETARG_A(c));
    break;
  case OP_RAISE:
    printf("OP_RAISE\tR%d\n", GETARG_A(c));
    break;
  case OP_POPERR:
    printf("OP_POPERR\t%d\n", GETARG_A(c));
    break;
  case OP_EPOP:
    printf("OP_EPOP\t%d\n", GETARG_A(c));
    break;

  default:
    printf("OP_unknown %d\t%d\t%d\t%d\n", GET_OPCODE(c),
	   GETARG_A(c), GETARG_B(c), GETARG_C(c));
    break;
  }
}

mrb_irep *
mmrb_search_irep(mrb_state *mrb, mrb_code *pc)
{
  int i = 0;
  for (i = 0; i < mrb->irep_len; i++) {
    mrb_irep *irep = mrb->irep[i];
    if (irep->iseq <= pc && pc <= irep->iseq + irep->ilen) {
      return irep;
    }
  }

  return NULL;
}

void
mmrb_disasm_irep(mrb_state *mrb, mrb_irep *irep)
{
  int i = 0;
  for (i = 0; i < irep->ilen; i++) {
    printf("%4x ", i);
    mmrb_disasm_once(mrb, irep, irep->iseq[i]);
  }
}


void
mrb_mmrb_debug_ext_gem_init(void) { }

void
mrb_mmrb_debug_ext_gem_final(void) { }

