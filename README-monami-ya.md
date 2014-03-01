
## What's monami-ya.mrb

monami-ya.mrb is the enhanced version of mruby.

# !!Notice!!
    This is a preliminary release for internal team review.
    (Even if the original version of mruby becomes stable.)
    The URLs and addresses described below are not available yet.
    The official release will be announced later.
    Any suggestions for modification are welcome.
    Delays in replies are to be expected. Sorry in advance.

## How to get monami-ya.mrb opensource edition

Distribution files can be found in the following site:

  https://bitbucket.org/mruby/monami-ya.mrb

The trunk of the monami-ya.mrb source tree can be checked out with the
following command:

    $ git clone https://bitbucket.org/mruby/monami-ya.mrb

There are some other branches under development.

```master``` branch is stable relatively.

```develop``` branch is a beeding edge.

## How to Contribute

See the contribution guidelines in CONTRIBUTING.md file.
then send pull request to <http://bitbucket.org/mruby/monami-ya.mrb>.  We consider you have granted
non-exclusive right to your contributed code under MIT license.  If you want to be named
as one of monami-ya.mrb OSS developers, include an update to the AUTHORS file in your pull request.

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
Copyright (c) 2014 mruby developers

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

It's possible that mruby developers changes their license.
So please add "monami-ya.mrb developers" as the copyright name if you want
to keep your code under MIT license.
The core team of monami-ya.mrb promise we don't change their license.
(And this is not "MUST" requirement. You can choice contribute your code
as "mruby developers")

## Note for License

By the reason same as the strategy of the orignal version of mruby,
monami-ya.mrb also decided to display "monami-ya.mrb developers" as the copyright name.
Actual copyright holder will be listed in the AUTHORS-monami-ya file.

## How to Contribute

See the contribution guidelines (CONTRIBUTING.md)
then send a pull request to <https://bitbucket.org/monami_ya_mrb/monami-ya.mrb>.
We consider you have granted non-exclusive right to your contributed code under MIT license.
If you want to be named as one of mruby developers, please include an update to the AUTHORS-monami-ya
file in your pull request.
(Note: AUTHORS file is for the original version of mruby. So we don't edit it.)
