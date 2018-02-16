C Translator
============

Your compiler should support a translation mode, which translates
a sub-set of pre-processed C90 into Python.

Program build and execution
---------------------------

Your program should be built using the command:

    make bin/c_compiler

The translator function is invoked using the flag `--translate`, with
the source file and output file specified on the command line:

    bin/c_compiler --translate [source-file.c] -o [dest-file.py]
    
You can assume that the command-line arguments will always be in this
order, and that there will be no spaces in source or destination paths.

Input format
------------

The input file will be pre-processed [ANSI C](https://en.wikipedia.org/wiki/ANSI_C),
also called C90 or C89. It's what's generally thought of as "classic" or "normal" C,
but not the _really_ old one without function prototypes (you may never have come
across that). C90 is still often used in embedded systems, and pretty much the
entire linux kernel is in C90.

We've mainly taught you C++, but you're probably aware of C as a subset of C++ without
classes, which is a good mental model. Your programs (lexer, parser and compiler) will
never be given code that has different parsing or execution semantics under C and C++ (so for
example I won't give you code that uses "class" as an identifier).

The source code will not contain any compiler-specific or platform-specific
extensions. If you pre-process a typical program (see later), you'll see many
things such as `__attribute__` or `__declspec` coming from the system headers. You will
not need to deal with any of these.

The test inputs will be a set of files of increasing complexity and
variety. The initial files will contain a small number of basic tokens,
then the lexicon will be increased, and finally Pre-Processor directives
(see next section) will be included. All files will be [well-formed](https://en.wikipedia.org/wiki/Well-formedness).

Output format
-------------

The output format will be a Python3equivalent to the input C.

The full set of patterns required is described [here](c_translator_templates.md).

As an example, we would translate the input program:

```
int f(int x)
{
  return x*x;
}
```
into the python:
def f(x):
    return x*x
```

Or this program:
```
int f(int x)
{
  if(x>1){
    return x*f(x-1);
  }else{
    return x;
  }
}
```
turns into this:
```
def f(x):
   if x>1:
       return x*f(x-1)
   else:
       return x
```

Formative submission
--------------------

For the formative submission on 20th of Feb, the goal is to get
as much of the parser and AST working as you can, as then it is
possible to critique your AST and overall approach. If you have
some code-gen working, then so much the better, but parsing and
then representing programs as a data-structure is the main objective.

Pre-Processor
-------------

TLDR : if you don't want to support pre-processor information, then
if you encounter a `#` character that is _not_ in a string literal,
consume all characters to the end of the line.

If needed, the input will already have been pre-processed, which means
that all instances `#define` and `#include` will already have been processed. However,
you may still see some input lines from the pre-processor. Create a simple
file called `tmp.c`:

    echo -e "#include <stdio.h>\nint main(){\n  printf(\"Wibble\"); \n}\n" > tmp.c
    less tmp.c

We can now pre-process it with `cpp`, the [GNU C Pre-Processor](https://gcc.gnu.org/onlinedocs/cpp/):

    cpp tmp.c

You should should see a load of stuff from the standard includes, then
your tiny program at the bottom. You can get a better look with `less`:

    cpp tmp.c | less

At the top of the file you'll see something like `# 1 tmp.c`, then
references to various files like `/usr/include/stdio.h` and `/usr/include/sys/config.h`.
If you scroll down, eventually you'll see the first non-empty line that
_doesn't_ start with `#` - in my case it is `typedef signed char __int8_t;`,
but it could be something else. This represents the first true token,
and will be the first line that should cause a token to be output.

The format of the `#` lines is explained in the [Pre-Processor Manual](https://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html),
though you can get a good sense of what is going on by comparing
the original headers and the output of `cpp`. `less` has a mode
the displays line numbers (the `-N` option), so do:

    cpp tmp.c | less -N

in one terminal, and in another terminal do:

    less -N /usr/include/stdio.h

It gives quite a good insight into how the transformation
happens, as you can match up the lines in the original
versus the lines in the `#` output, and see where comments
and `#define`s have been removed.

Note: there is no requirement that the input has been pre-processed,
so there may be no `#` lines at all.
