## Basics and Starting up

Lets start with a sample command line parser program with the coding. The below example deal with both C and Python as well. I generally use **Python**  here to 'declutter' the things and to add more clarity.


### Command line parser


Let us start with a small program that performs an action based on the command line arguments passed to it.


In C:

```c
#include <stdio.h>
#include <stdlib.h>

int add(int a, int b)
{
    return a + b;
}

int sub(int a, int b)
{
    return a - b;
}

void program_usage(char *prog_name)
{
    printf("%s <add/sub> num1 num2\n", prog_name);
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        program_usage(argv[0]);
        return -1; // always return failure if you unsure of what to do
    }

    if (!strcmp(argv[1], "add")) {
        printf("add result %d\n", add(atoi(argv[2]), atoi(argv[3])));
    } else if (!strcmp(argv[1], "sub")) {
        printf("sub result %d\n", sub(atoi(argv[2]), atoi(argv[3])));
    } else {
        program_usage(argv[0]);
        return -1;
    }

    return 0;
}
```

**Example: command line sample program**


We use `gcc` (any version > 5.0 is fine) for compiling our programs presented in this book.

when compiled with `gcc -Wall cmdline.c -o cmdline` option, this will generate the `cmdline` binary file.

This file is an executable file. This file can be run by typing the file name as below in the shell.

```
# ./cmdline
```

The `./` tells the shell to execute the program that is present in this directory.

More about the program running in the upcoming chapters. For now, we now know how to compile and run a program.

The program accepts either of two strings namely, add and sub. If either of them are present, it then executes a function that performs the action (Add two integers or Subtract two integers).

The `program_usage`, `sub` and `add` are called the functions. Functions allow the code to be more structured and sensible.

so typically the command line argument would become,

```bash
# ./cmdline add 28  24
```

or

```bash
# ./cmdline sub 28 24
```

**NOTE**: You do not really need to be root to run this program.



A function would typically look as below..

```c
return_type function_name(variable_type var_name, variable_type var_name, ..);
```

Each function has `return_type` and accepts set of variables as its input called `arguments`. The return_type can be a variable type such as `int` or `void`. The function, when it has the return type as `void` meaning that the function returns nothing.

The same program is written in Python below:

```python
#!/usr/bin/python

import sys

def add(a, b):
    return int(a) + int(b)

def sub(a, b):
    return int(a) - int(b)

def program_usage():
    print sys.argv[0] + " add/sub num1 num2"

if len(sys.argv) != 4:
    program_usage()
    exit(1)

if sys.argv[1] == "add":
    print "add result: " + str(add(sys.argv[2], sys.argv[3]))
elif sys.argv[1] == "sub":
    print "sub result: " + str(sub(sys.argv[2], sys.argv[3]))
else:
    program_usage()
    exit(1)
```

### Compilation stages in a C program

Here are the things that happen when you compile a **C** program.

There are 4 stages in a **C** program compilation.

1. Preprocessor
2. Compiler
3. Assembler
4. Linker


#### Preprocessor:

The preprocessor is a program that converts the C code into a high level code by removing comments, replacing the header files with the actual content of the header files, replacing the macros into their corresponding code etc. Its job is to parse the C program and convert into something that's understandable by the compiler. Find more on the preprocessor in the macros chapter.

This preprocessor stage can be done with the `gcc` using the `-E` option.

```bash
gcc -E basics.c
```

when given with -o option it copies the output to the file after the -o.

```bash
gcc -E basics.c -o basics.i
```

The `basics.i` will contain the high level preprocessor output.

#### Compiler:

The compiler is another program that converts the source code into assembly code that is understandable by the assembler.

This can be done with the gcc using the **-S** option. We are passing the **basics.i** file after the preprocessor stage to the compiler. Alternatively you can pass **basics.c** and see what happens.

```bash
gcc -S basics.i
```

or another way to generate the assembly file is to do `gcc -S basics.c`.

for example the following hello program.

```c
#include <stdio.h>

int main()
{
    printf("Hello..\n");
}

```

the output of `hello.s` is,


```asm
    .file   "hello.c"
    .section    .rodata
.LC0:
    .string "Hello.."
    .text
    .globl  main
    .type   main, @function
main:
.LFB0:
    .cfi_startproc
    pushq   %rbp
    .cfi_def_cfa_offset 16
    .cfi_offset 6, -16
    movq    %rsp, %rbp
    .cfi_def_cfa_register 6
    movl    $.LC0, %edi
    call    puts
    movl    $0, %eax
    popq    %rbp
    .cfi_def_cfa 7, 8
    ret
    .cfi_endproc
.LFE0:
    .size   main, .-main
    .ident  "GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609"
    .section    .note.GNU-stack,"",@progbits

```


#### Assembler:

The assembler program converts the assembly code passed by the compiler into an object code.

This can be done with the gcc using the -c option. We are passing the basics.s file that is produced at the compilation stage. Alternatively you can pass basics.c and see what happens.

```bash
gcc -c basics.s
```

This generates a file called basics.o that is the object file.

running `file` command on the object file results in the following output on a 64 bit machine.

```bash
ELF 64-bit LSB relocatable, x86-64, version-1 (SYSV), not stripped
```

#### Linker:

The linker resolves the references to the functions that are defined else where (such as in libraries) by adding the function's addresses so that the loader program can locate and load the functions if necessary. This part is called the linking process. The executable is created after the linking process. We can do all this with the gcc -o flag. As always, try it out with `basics.c` and see what happens ! :-)

```bash
gcc -o basics basics.o
```

The final executable's name is `basics`.

We will go deep into each compilation stage in the following chapters where ever necessary.

