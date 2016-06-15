# System calls and library functions


The differences are:

1. System call is a function call that involves kernel to execute part of the code and return the value back to the userspace. Library function does not simply perform that operation. It simply performs a job that does not involve going into the kernel and executing on behalf of the user.
2. Example: ```system``` is a system call that executes a command. The call involve, going into the kernel space, creating the process, executing etc. The ```strcpy``` is a function call that simply copies source string into the destination. It does not involve system call to go into kernel space and copy the string (because that is un-needed).


When system calls fail, they also set the ```errno``` variables accordingly to best describe the problem and why the failure has happened.

```errno``` is a global variable and to be used carefully protected with in the threads. the ```errno.h``` should be included to use this variable. The ```asm-generic/errno.h``` contains all the error numbers. (Although one can only include the ```errno.h``` and not the ```asm-generic/errno.h``` as the later is more platform specific AFAIK).

```perror``` is a useful function that describes the error in the form of a string and outputs to ```stderr```

A small example demostrating the ```perror``` is shown below. (You can also view / Download [here](https://github.com/DevNaga/gists/blob/master/errno_strings.c))

```c
#include <stdio.h>
#include <errno.h> //for errno and perror
#include <string.h> // for strerror

int main(int argc, char **argv)
{
    FILE *fp;

    fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "failed to open %s\n", argv[1]);
        perror("fopen:");
        return -1;
    }

    perror("fopen:");
    printf("opened file %s [%p]\n", argv[1], fp);

    fclose(fp);

    return 0;
}
```
**
Example: perror example**

We compile it with ```gcc -Wall errno_strings.c```. It will generate an ```a.out``` file for us.

Then we run our binary with the correct option as below:

        ./a.out errno_string.c
        
        fopen: Success
        opened file errno_strings.c [0xeb1018]
        

The ```perror``` gives us that the file has been opened successfully. The filepointer is then printed on to the screen.

Then we run our binary with the incorrect option as below:

        ./a.out errno_string.c.1
        
        failed to open errno_string.c.1
        fopen: No such file or directory

###System() system call

The `system()` system call is used to execute a shell command in a program. Such as the following code


    system("ls");

Will simply execute the `ls` command.

During the execution of the function, the `SIGCHLD` will be blocked and `SIGINT` and `SIGQUIT` will be ignored. We will go in detail about these signals in the later chapter **signals**.

The `system` function returns -1 on error and returns the value returned by command. The return code is actually the value left shifted by 7. We should be using the `WEXITSTATUS(status)`.

### Exec family of calls

The exec family of functions create a new process image from the given binary file or a script and replaces the contents of the original program with the contents of the given binary file or a script. These functions only return in case of a failure such as when the path to the binary / script file is not found.

### system variables

`sysconf` is an API to get the current value of a configurable system limit or option variable.

The API prototype is the following

    long sysconf(int name);
    
symbolic constants for each of the variables is found at include file `<unistd.h>`. The `name` argument specifies the system variable to be queried.

sysconf() example on the max opened files:

```c
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    int maxfd;
    
    maxfd = sysconf(_SC_OPEN_MAX);
    printf("maxfd %d\n", maxfd);
    
    return 0;
}

```

