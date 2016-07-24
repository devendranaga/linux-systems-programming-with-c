# function pointers

Functions pointer is a pointer to a function such as for example a pointer to a string. The usefulness lie behind the use of generic name to call any function with just referencing the pointer. This also means that a software layering is possible with the use of the function pointers and the data structures.

Function pointers are the core to an object oriented approach in the C programming language. This method of using the function pointers and the data structures pave the way of object oriented programming approach.

Function pointers have a wide range of uses. Such as they can be used as event notification calls, ex: a data is ready on the socket, file descriptor, a timer is expired, an interrupt has been received etc. these we are going to discuss and dive deeply in the following chapters.

for example, the below program calls a function pointer based on the user input.

```c
#include <stdio.h>

char dest[100];
int len;

int copy_strings(int argc, char **argv)
{
    // fail if we do not have enough arguments
    if (argc != 1) {
        return -1
    }

    strcpy(dest, argv[0]);

    printf("copied to dest: %s\n", dest);
    return 0;
}

int len_strings(int argc, char **argv)
{
    if (argc != 1) {
        return -1;
    }

    len = strlen(argv[0]);

    printf("length of the string: %d\n", len);
    return 0;
}

static struct cmd_args {
    char *arg_name;
    int (*func)(int argc, char **argv);
} arg_list[] = {
    {"copy", copy_strings},
    {"len", len_strings},
};

int main(int argc, char **argv)
{
    int ret;
    int i;

    for (i = 0; i < sizeof(arg_list) / sizeof(arg_list[0]); i++) {
        if (!strcmp(argv[1], arg_list[i].arg_name)) {
            arg_list[i].func(argc - 2, &argv[i + 2]);
        }
    }

    return 0;
}
```

The function pointers above are kept into the data structure, so that the caller can write the code in generic fashion. The generic code then does not need an expansion if there are more command line arguments other than just `copy` and `len`. It simply needs expanding the structure and the generic code is already handling the calling of functions based on the command line arguments.

The more advanced uses of function pointers is that they can be used as notification mechanisms when an event happens.

Such as incoming data over a network connection, connection loss e.t.c. are some of the events. The pseudo code for such types of events is as follows.

```c

void event_cb(void *event_data, void *user_data)
{
// read the event
}

int main()
{
    myapp_data *my_data;
    ...
    // alloc my_data
    ...
    // setup callback for the networking data event
    register_event_networking_data(...event_cb, my_data);
}
```



The `user_data` is provided to the application back at the callback to facilitate the use of its contextual information that is stored before.

 

Problems:

1. write a usage\(\) API that gets called, if the arguments are mismatched from the command line. Also print the usage\(\) if there are not enough arguments.
2. handle the return code from the function pointer call, and print if there is an error returned by the functions.

