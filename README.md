## What's monami-ya.mrb

monami-ya.mrb is the enhanced version of mruby.

# !!Notice!!
    This is a preliminary release for internal team review.
    The URLs and addresses described below are not available yet.
    The official release will be announced later.
    Any suggestions for modification are welcome.
    Delays in replies are to be expected. Sorry in advance.

[![Build Status](https://travis-ci.org/mruby/mruby.png?branch=master)](https://travis-ci.org/mruby/mruby)

## What's mruby

mruby is the lightweight implementation of the Ruby language complying to (part of)
the [ISO standard](http://www.iso.org/iso/iso_catalogue/catalogue_tc/catalogue_detail.htm?csnumber=59579). 
mruby can be linked and embedded within your application.  We provide the interpreter program "mruby" and 
the interactive mruby shell "mirb" as examples.  You can also compile Ruby programs into compiled byte code
using the mruby compiler "mrbc".  All those tools reside in "bin" directory.  The "mrbc" is also able to
generate compiled byte code in a C source file.  You can check the "mrbtest" program under the "test" directory.

This achievement was sponsored by the Regional Innovation Creation R&D Programs of
the Ministry of Economy, Trade and Industry of Japan.

You can get original version of mruby on http://github.com/mruby/mruby/


## How to get monami-ya.mrb opensource edition

Distribution files can be found in the following site:

  https://bitbucket.org/mruby/monami-ya.mrb

The trunk of the monami-ya.mrb source tree can be checked out with the
following command:

    $ git clone https://bitbucket.org/mruby/monami-ya.mrb

There are some other branches under development.

```master``` branch is stable relatively.

```develop``` branch is a beeding edge.

Some branches are for temporary purpose.  It might remove with no previous notice.


Try the following command and see the list of branches:

    $ git branch -r


## How to compile and install

See the INSTALL file.

## Running Tests

To run the tests, execute the following from the project's root directory.

    $ make test

Or

    $ ruby ./minirake test

## Customization

monami-ya.mrb contains a package manager called *mrbgems*. To create extensions
in C and/or Ruby you should create a *GEM*. You will find a complete
documentation with examples under *examples/mrbgems*.

## License

See each source code.
If there are no special description on each source file, the license is below.

```
Copyright (c) 2012,2013 mruby developers

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in 
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.
```

## Note for License

The original version of mruby) has chosen a MIT License due to its permissive
license allowing developers to target various environments such as embedded systems.
However, the license requires the display of the copyright notice and license
information in manuals for instance. Doing so for big projects can be 
complicated or troublesome.
This is why mruby has decided to display "mruby developers" as the copyright name
to make it simple conventionally.
In the future, mruby might ask you to distribute your new code
(that you will commit,) under the MIT License as a member of
"mruby developers" but contributors will keep their copyright.
(We did not intend for contributors to transfer or waive their copyrights,
 Actual copyright holder name (contributors) will be listed in the AUTHORS file.)

## How to Contribute

See the contribution guidelines in CONTRIBUTING.md file.
then send pull request to <http://bitbucket.org/mruby/monami-ya.mrb>.  We consider you have granted
non-exclusive right to your contributed code under MIT license.  If you want to be named
as one of monami-ya.mrb OSS developers, include an update to the AUTHORS file in your pull request.
