# Formatted input and output

The formatted input and output is used to take input and output in a specific format. The header file used for this one is `stdio.h`.

### printf and sub functions

`printf` is a very useful API to print the message in a formatted way on to the console or to `stdout`. The `printf` usage is very simple as below.

```c
printf("value is %d\n", value);
```

`fprintf` is another API that prints on to the console but to the `stderr`. The difference between the `stdout` and `stderr` is that the later is not buffered and directly the messages will be dumped on to the console. The `stdout` instead buffers the input and dumps it later. This can easily be viewed when debugging a big loop.

There is another API that prints to a buffer in the formatted way just as `printf`. This API we call it `sprintf`. The `sprintf` usage is very simple as below.

```c
sprintf(buf, "value is %d\n", value);
```

### scanf and sub functions

`scanf` is a very useful API to read strings in a suitable format from the console or from the `stdin`. The `scanf` usage is very simple as below.

```c
scanf("%s %d", string, integer);
```

There is another function that 
Although `scanf` is really useful in reading the strings in the format the user wants, it sometimes gives us data that we did not expect.

There is a similar API that reads instead from the buffer called `sscanf`. This API we call it `sscanf`. The `sscanf` usage is very simple as below.

```c
sscanf(buf, "%s %d", string, integer);
```

### variadic functions

The variadic functions allows the user to write own `printf` like functions that are customizable for the project purposes. They all include the `stdarg.h`.

The `stdarg.h` uses the following APIs: `va_start`, `va_list`, `va_arg` and `va_end`.

```c
void dev_printf(char *fmt, ...)
{
    va_list ap;
    
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}
```

### variadic macros

`gcc` provides a useful feature called variadic macro that allows us to define our own printf style macros. The usual format is

```c
#define dprintf(...) fprintf(stderr, __VA_ARGS__)
```
