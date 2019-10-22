## fcntl system call

`fcntl` system calls allow to control file descriptor options.

Here are some of the options.

| option | description |
|--------|-------------|
| `F_DUPFD` `F_DUPFD_CLOEXEC` | duplicate file descriptor |
| `F_GETFD` `F_SETFD` | file descriptor flags |
| `F_GETFL` `F_SETFL` | file status flags |
| `F_SETLK` `F_SETLKW` | advisory locking |
 

**Example in C:**

```c
    int fd;
    int flags = 0;
    int ret;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags != 0) {
        return -1;
    }

    flags |= O_NONBLOCK;

    ret = fcntl(fd, F_SETFL, flags);
    if (ret != 0) {
        return -1;
    }
```

The `F_DUPFD` option is similar to the `dup` system call. This can be done alternatively with `fcntl`.

below example demonstrates the `F_DUPFD` feature of `fcntl` system call. Download [here](https://github.com/DevNaga/gists/blob/master/fcntl.c)

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
    int stdout_fd;

    if (argc != 2) {
        fprintf(stderr, "<%s> text\n", argv[0]);
        return -1;
    }

    stdout_fd = fcntl(1, F_DUPFD, 3);
    if (stdout_fd < 0) {
        fprintf(stderr, "failed to fcntl dupfd %s\n", strerror(errno));
        return -1;
    }

    write(stdout_fd, argv[1], strlen(argv[1]) + 1);

    return 0;
}

```
