# Basics and Starting up


For any systems level programs, tools are at most important for validation, simulation and statistics.

The tools may range from a simple program that gets transmitted and received packet counts to a tool that performs dynamic changing of configuration items in the system daemons without the need of a restart.

## Command line parser


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
    return a + b;
}

void program_usage(char *prog_name)
{
    printf("%s <add/sub> num1 num2\n");
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        program_usage(argv[0]);
        return -1;
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
**Example: command line sample program
**

We use Gcc (any version > 5.0 is fine).

when compiled with ```gcc -Wall cmdline.c -o cmdline``` option, this will generate the ```cmdline```binary file.


The program accepts either of two strings namely, add and sub. If either of them are present, it then executes a function that performs the action (Add two integers or Subtract two integers).

The `program_usage`, `sub` and `add` are called the functions. Functions allow the code to be more structured and sensible.

A function would typically look as below..

```c
return_type function_name(variable_type var_name, variable_type var_name, ..);
```

Each function has `return_type` and accepts set of variables as its input called `arguments`. The return_type can be a variable type such as `int` or `void`. The function, when it has the return type as `void` meaning that the function returns nothing.

The same program is written in Python below:

```
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
