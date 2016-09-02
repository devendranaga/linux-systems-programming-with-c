# System calls and library functions

The differences are:

1. System call is a function call that involves kernel to execute part of the code and return the value back to the userspace. Library function does not simply perform that operation. It simply performs a job that does not involve going into the kernel and executing on behalf of the user.
2. Example: `system` is a system call that executes a command. The call involve, going into the kernel space, creating the process, executing etc. The `strcpy` is a function call that simply copies source string into the destination. It does not involve system call to go into kernel space and copy the string \(because that is un-needed\).

When system calls fail, they also set the `errno` variables accordingly to best describe the problem and why the failure has happened.

`errno` is a global variable and to be used carefully protected with in the threads. the `errno.h` should be included to use this variable. The `asm-generic/errno.h` contains all the error numbers. \(Although one can only include the `errno.h` and not the `asm-generic/errno.h` as the later is more platform specific AFAIK\).

`perror` is a useful function that describes the error in the form of a string and outputs to `stderr`

A small example demostrating the `perror` is shown below. \(You can also view \/ Download [here](https://github.com/DevNaga/gists/blob/master/errno_strings.c)\)

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

We compile it with `gcc -Wall errno_strings.c`. It will generate an `a.out` file for us.

Then we run our binary with the correct option as below:

```bash
    ./a.out errno_string.c

    fopen: Success
    opened file errno_strings.c [0xeb1018]
```

The `perror` gives us that the file has been opened successfully. The filepointer is then printed on to the screen.

Then we run our binary with the incorrect option as below:

```bash
    ./a.out errno_string.c.1

    failed to open errno_string.c.1
    fopen: No such file or directory
```

Linux provides another API to print the error message based on the error number variable `errno`.

`error` is another API provided by the Glibc. It is declared as follows.

```c
void error(int status, int errno, const char *format, ...);
```

The `status` variable is usually set to 0. The `errno` variable is the `errno`.
The format is any message that the program wants to print.

The below example provides an idea of the `error` function.

```c
#include <stdio.h>
#include <error.h>
#include <errno.h>

int main(void)
{
    int fd = -1;
    
    close(fd);
    error(0, errno, "failed to closed fd\n");
    
    return 0;
}
```

```c
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
    char *file = NULL;
    int fd;

    fd = open(file, O_RDWR);
    if (fd < 0) {
        printf("failed to open file: error: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}
```

### System\(\) system call

The `system()` system call is used to execute a shell command in a program. Such as the following code

```
system("ls");
```

Will simply execute the `ls` command.

During the execution of the function, the `SIGCHLD` will be blocked and `SIGINT` and `SIGQUIT` will be ignored. We will go in detail about these signals in the later chapter **signals**.

The `system` function returns -1 on error and returns the value returned by command. The return code is actually the value left shifted by 7. We should be using the `WEXITSTATUS(status)`.

However the `system` system call has some serious vulnerabilities and should not be used. [Here](https://www.securecoding.cert.org/confluence/pages/viewpage.action?pageId=2130132) is the link.

### Exec family of calls

The exec family of functions create a new process image from the given binary file or a script and replaces the contents of the original program with the contents of the given binary file or a script. These functions only return in case of a failure such as when the path to the binary \/ script file is not found.

the following are the exec functions.

```c
int execl(const char *path, const char *arg, ...);
int execlp(const char *path, const char *arg, ...);
int execle(const char *path, const char *arg, ...);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);

```

### system variables

`sysconf` is an API to get the current value of a configurable system limit or option variable.

The API prototype is the following

```c
long sysconf(int name);
```

symbolic constants for each of the variables is found at include file`<unistd.h>`. The `name` argument specifies the system variable to be queried.

`sysconf()` example on the max opened files:

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

1. to get the number of clock ticks here is the following API: `_SC_CLK_TCK`.

There are two other APIs to get or set the resource limits on the process.

**1. getrlimit**
**2. setrlimit**

The APIs are defined in `<sys/resource.h>`. The prototypes are as follows.

```c
int getrlimit(int resource, struct rlimit *rlim);
int setrlimit(int resource, const struct rlimit *rlim);
```

The `struct rlimit` is defined as below.

```c
struct rlimit {
    rlim_t rlim_cur; // soft limit
    rlim_t rlim_max; // hard limit
}
```

The two APIs return 0 on success and -1 on failure. The corresponding error descriptive functions will be used to describe the return.

The limits are 15 provided by the kernel at asm-generic/resource.h.

|Resource name | description |
|--------------|-------------|
| `RLIMIT_CPU` | maximum CPU time |
| `RLIMIT_FSIZE` | maximum file size in bytes |
| `RLIMIT_DATA` | maximum data size |
| `RLIMIT_STACK` | maximum stack size in bytes |
| `RLIMIT_CORE` | maximum core file size in bytes |
| `RLIMIT_RSS` | maximum RSS size |
| `RLIMIT_NPROC` | maximum number of processes that user may be running |
| `RLIMIT_NOFILE` | maximum number of open files |
| `RLIMIT_MEMLOCK` | maximum number of bytes a process can lock into memory |
| `RLIMIT_AS`  | maximum address space size |
| `RLIMIT_LOCKS` | maximum file locks held |
| `RLIMIT_SIGPENDING` | maximum number of pending signals that to be delivered to the process |
| `RLIMIT_MSGQUEUE` | maximum bytes in posix message queue |
| `RLIMIT_NICE` | maximum nice priority allowed |
| `RLIMIT_RTPRIO` | maximum realtime priority allowed |
| `RLIMIT_RTTIME` | timeout of RT tasks in use |


The below example illustrates the `rlimit API` uses. Both the `getrlimit` and `setrlimit` are described in the below example.

```c
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

void get_max_addr_space()
{
    struct rlimit rlim;
    int ret;

    ret = getrlimit(RLIMIT_AS, &rlim);
    if (ret < 0) {
        fprintf(stderr, "failed to getrlimit\n");
        return;
    }

    printf("soft : %lu hard : %lu\n",
                            rlim.rlim_cur, rlim.rlim_max);
}

void get_max_file_size()
{
    struct rlimit rlim;
    int ret;

    ret = getrlimit(RLIMIT_FSIZE, &rlim);
    if (ret < 0) {
        fprintf(stderr, "failed to getrlimit\n");
        return;
    }

    printf("soft: %lu hard: %lu\n",
                            rlim.rlim_cur, rlim.rlim_max);
}

void get_max_stack_size()
{
    struct rlimit rlim;
    int ret;

    ret = getrlimit(RLIMIT_STACK, &rlim);
    if (ret < 0) {
        fprintf(stderr, "failed to getrlimit\n");
        return;
    }

    printf("soft: %lu hard: %lu\n",
                            rlim.rlim_cur, rlim.rlim_max);
 }
 
void get_max_cpu_time()
{
    struct rlimit rlim;
    int ret;

    ret = getrlimit(RLIMIT_CPU, &rlim);
    if (ret < 0) {
        fprintf(stderr, "failed to getrlimit\n");
        return;
    }

    printf("soft: %lu hard: %lu\n",
                            rlim.rlim_cur, rlim.rlim_max);
}

void set_max_stack_size(int n, char **argv)
{
    struct rlimit rlim;
    int ret;

    ret = getrlimit(RLIMIT_STACK, &rlim);
    if (ret < 0) {
        fprintf(stderr, "failed to getrlimit\n");
        return;
    }

    rlim.rlim_cur = strtol(argv[0], NULL, 10);

    ret = setrlimit(RLIMIT_STACK, &rlim);
    if (ret < 0) {
        fprintf(stderr, "failed to setrlimit\n");
        return;
    }
}

struct rlimit_list {
    char *string;
    void (*get_callback)(void);
    void (*set_callback)(int n, char **rem_args);
} rlimit_list[] = {
    {"max_addr_space", get_max_addr_space, NULL},
    {"max_file_size", get_max_file_size, NULL},
    {"max_stack_size", get_max_stack_size, set_max_stack_size},
    {"max_cpu_time", get_max_cpu_time, NULL},
};

int main(int argc, char **argv)
{
    int i;

    if (!strcmp(argv[1], "get")) {
        for (i = 0; i < sizeof(rlimit_list) / sizeof(rlimit_list[0]); i ++) {
            if (!strcmp(rlimit_list[i].string, argv[2])) {
                rlimit_list[i].get_callback();
            }
        }
    } else if (!strcmp(argv[1], "set")) {
        for (i = 0; i < sizeof(rlimit_list) / sizeof(rlimit_list[0]); i ++) {
            if (!strcmp(rlimit_list[i].string, argv[2])) {
                if (rlimit_list[i].set_callback)
                    rlimit_list[i].set_callback(argc - 3, &argv[3]);
            }
        }
    }

    return 0;
}
```

