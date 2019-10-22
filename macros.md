## preprocessor macros

### #define preprocessor

The preprocessor macros or just macros are really useful in the program to give it a real meaningful name.

Say for example:

```c
int main(void)
{
    double a = 3.14159;

    printf("value of pi is %f\n", a);

    return 0;
}
```

is more meaningful when written as

```c
#define PI 3.14159
int main(void)
{
    double a = PI;

    printf("value of pi is %f\n", a);

    return 0;
}
```

The above program simply provides an example of the macro usefulness. You could although still use `M_PI` from the `<math.h>` header file.

At the preprocessor stage the value `PI` is replaced with its actual value i.e. 3.14159.

Although macros mostly are used to define constants that can't be defined / sensibly defined with enums, they could also be used to write some simple and useful statements. Such as the following.

```c
#define DEG_TO_RAD(deg) (deg * M_PI / 180.0)
```

or Say

```c
#define LEFT_SHIFT(__val, __bits) (__val = ((__val) << __bits))
```

### #include preprocessor

The `#include` is a preprocessor directive that does the following..

1. When used with `<>` the preprocessor will look at the standard includes directory `/usr/include/` for example `#include <stdio.h>`.

This makes the preprocessor to look at `/usr/include` for the header file `stdio.h`.

2. When included with `" "` the preprocessor will look at the current directory first and then if the headerfile is not found, it looks at the standard includes directory `/usr/include`.

Ex:
    `#include "stdio.h"`

This makes the preprocessor to look at the current working directory where the program is being compiled and if the header is not found, then at the `/usr/include`.

### conditional compilation macros

Sometimes, we do not want some part of the code to be compiled and part of the final program. Lets take the famous example of describing it with the device driver code.

Lets say that the device driver has functions specific for the X86 (Intel) and ARM architectures. We do not want an ARM function run on the X86 platform and an X86 function run as part of the ARM platform. Thus, we conditionally de-select the functions at the part of compilation. The preprocessor macros `#if` and `#ifdef` are used for this purpose.

#### The #if preprocessor macro

The `#if` directive is used to test the value of an arithmetic expression. Every `#if <expression>` statement must follow `#endif`. The below example shows the usage of the `#if` macro.

```c
#if 1
    printf("Learn C .. \n");
#endif
```

The `#if` macro generally comes to handy at debugging. Lets consider that we wanted to hide the `printf` statement for some reason. We could simply replace the 1 after the `#if` with 0. Such as the below,

```c
#if 0
    printf("Learn C .. \n");
#endif
```

This is almost same as commenting the code (meaning taking it out of the compilation). Lets think about a large code base containing thousands of lines of source. To debug a part of the program or a function, the `#if` would come very handy.

#### The #ifdef preprocessor macro

