/*
** mruby/version.h - mruby version definition
**
** See Copyright Notice in mruby.h
*/

#ifndef MRUBY_VERSION_H
#define MRUBY_VERSION_H

#define MRUBY_RUBY_VERSION "1.9"
#define MRUBY_RUBY_ENGINE  "mruby (monami-ya.mrb)"

#define MRB_STRINGIZE0(expr) #expr
#define MRB_STRINGIZE(expr) MRB_STRINGIZE0(expr)

#define MRUBY_VERSION "1.1.0"
#define MRUBY_RELEASE_MAJOR 1
#define MRUBY_RELEASE_MINOR 1
#define MRUBY_RELEASE_TEENY 1
#define MRUBY_RELEASE_NO 10101
#define MRUBY_RELEASE_YEAR 2014
#define MRUBY_RELEASE_MONTH 11
#define MRUBY_RELEASE_DAY 19
#define MRUBY_RELEASE_DATE \
  MRB_STRINGIZE(MRUBY_RELEASE_YEAR) "-" \
  MRB_STRINGIZE(MRUBY_RELEASE_MONTH) "-" \
  MRB_STRINGIZE(MRUBY_RELEASE_DAY)

#define MRUBY_BIRTH_YEAR 2010

#define MRUBY_AUTHOR "mruby developers"

#define MONAMI_YA_MRB_RELEASE "1.1.0-SNAPSHOT"
#define MONAMI_YA_MRB_RELEASE_YEAR 2015
#define MONAMI_YA_MRB_AUTHOR "monami-ya.mrb developers"

#define MRUBY_DESCRIPTION      \
  "monami-ya.mrb " MRUBY_VERSION       \
  " (" MRUBY_RELEASE_DATE ") " \

#define MRUBY_COPYRIGHT                \
  "mruby - Copyright (c) "             \
  MRB_STRINGIZE(MRUBY_BIRTH_YEAR)"-"   \
  MRB_STRINGIZE(MRUBY_RELEASE_YEAR)" " \
  MRUBY_AUTHOR                         \
  "\nmonami-ya.mrb - Copyright (C) "           \
  "2012-"                                      \
  MRB_STRINGIZE(MONAMI_YA_MRB_RELEASE_YEAR) " "\
  MONAMI_YA_MRB_AUTHOR

#endif  /* MRUBY_VERSION_H */
