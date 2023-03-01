## Error codes

Linux system calls returns errors in the form of code stored in `errno` variable. This variable is referenced using the header file `<errno.h>`. The variable is global and be used in safe if the program is using threads.

The error codes are crucial for programs that access critical resources or very important tasks. Depending on the error code returned by the kernel, the userspace can accommodate for the failures.



Here are some of the descriptions of the error codes. As usual, we use the `perror` or `strerror` to describe the error in a string format.

| error code | description |
| --- | --- |
| EPERM | Permission denied. Special privileges are needed. |
| ENOENT | The file does not exist. |
| EIO | Input \/ Output error occured. |
| EINTR | Interrupted system call. Signal occured before the system call was allowed to finish. |
| ENOEXEC | Invalid executable file format. This is detected by the exec family of functions. |
| ENOMEM | No memory is available. Out of memory situation can get this error. |
| EBUSY | System resource that can't be shared and already in use. |
| ENOTDIR | Not a directory. File is given as an argument instead of the directory. |
| ENODEV | Given device is not found under \/dev\/ etc. |
| EEXIST | File exists. A file tried to open in write mode and is already there. |
| EFAULT | Segmentation fault. Invalid pointer access is detected. |
| EACCESS | Permission denied. The file permissions on the file does not allow the user to access. |
| EMFILE | Too many files are opened and can't open anymore. |
| EISDIR | File is a directory. |
| EFBIG | File too big. |
| EADDRINUSE | socket address is already in use. |
| ENETDOWN | a socket operation failed because the network was down. |
| ENETUNREACH | a socket operation failed because the network is not reachable. |
| EADDRNOTAVAIL | socket address that is requested is not available. |
| EOPNOTSUPP | operation not supported. mostly happens with server and client socket type and family mismatches. |
| EINPROGRESS | Connect syscall is in in progress to connect with a server. |
| EAGAIN | Resource temporarily blocked. Try again the call may succeed. |
| EPIPE | Broken pipe. to ignore it, signal\(SIGPIPE, SIG\_IGN\). Usually a broken pipe means that the other end of the connnection is closed. |
| EMLINK | too many links |
| ESPIPE | Invalid seek operation. |
| ENOTTY | Inappropriate ioctl operation for the tty. |
| EBADF | File descriptor is bad. The file descriptor is used on the read, write, select, epoll calls is invalid. |


Below is a useful error lookup table to map the errors against the strings (Which is what **strerror** usually do)

```c
static struct str_errno_lookup {
    int error;
    char *string;
} table[] = {
    {EPERM, "Permission denied"},
    {ENOENT, "No entry"},
    {EIO, "I/O Error"},
    {EINTR, "Interrupted system call"},
    {ENOEXEC, "Invalid exec format"},
    {ENOMEM, "Out of memory"},
    {EBUSY, "System resource busy"},
    {ENOTDIR, "Not a directory"},
    {ENODEV, "No such device"},
    {EEXIST, "File exist"},
    {EFAULT, "Segmentation Violation"},
    {EACCESS, "Access / Permission denied"},
    {EMFILE, "Too many opened files"},
    {EISDIR, "File is a directory"},
    {EFBIG, "File too big"},
    {EADDRINUSE, "Address already in use"},
    {ENETDOWN, "Network down"},
    {ENETUNREACH, "Network not reachable"},
    {EADDRNOTAVAIL, "Address not available"},
    {EOPNOTSUPP, "Operation not supported"},
    {EINPROGRESS, "Connect system call in progress"},
    {EAGAIN, "Resource temporarily unavailable. Restart the system call"},
    {EPIPE, "Broken pipe"},
    {EMLINK, "Too many symbolic links"},
    {ESPIPE, "Invalid seek operation"},
    {ENOTTY, "Inappropriate ioctl operation for the tty"},
    {EBADF, "Bad file descriptor"}
}

/**
 * @brief - get string based on the error number
 */
char *get_str_lookup(int errno)
{
    int i;

    for (i = 0; i < sizeof(table)/ sizeof(table[0]); i ++) {
        if (table[i].error == errno) {
            return table[i].string;
        }
    }

    return NULL;
}
```

**Examples:**

Below are some of the useful examples based off of the above described error codes.

**1. bad file descriptor**: [link](https://github.com/DevNaga/gists/blob/master/ebadf.c)

```c
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char **argv)
{
    char hello[] = "hello";
    int fd = -1;

    // writing on a file descriptor thats invalid
    write(fd, hello, sizeof(hello));
    printf("error %d : %s\n", errno, strerror(errno));

    return 0;
}

```



The output looks like the following

```shell

devnaga@Hanzo:~/gists$ ./a.out
error 9 : Bad file descriptor
devnaga@Hanzo:~/gists$

```







**2. permission denied as well address already in use**: [link](https://github.com/DevNaga/gists/blob/master/ealreadyinuse.c)

```c
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

int main()
{
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }

    int ret;
    struct sockaddr_in serv = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(22),
    };

    ret = bind(sock, (struct sockaddr *)&serv, sizeof(serv));
    if (ret < 0) {
        printf("error: %d %s\n", errno, strerror(errno));
        return -1;
    }

    return 0;
}


```



Running the above program without **sudo**:

```shell

devnaga@Hanzo:~/gists$ ./a.out
error: 13 Permission denied
devnaga@Hanzo:~/gists$

```



Running the above program with **sudo**:

```shell

devnaga@Hanzo:~/gists$ sudo ./a.out
[sudo] password for devnaga:
error: 98 Address already in use
devnaga@Hanzo:~/gists$

```

**3. Below is an example that demos the `EADDRINUSE`.**

Download [here](https://github.com/DevNaga/gists/blob/master/eaddrinuse.c)


```c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int fd;
    int ret;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        return -1;
    }

    struct sockaddr_in addr;

    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4444);

    ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        fprintf(stderr, "failed to bind : %s\n", strerror(errno));
        close(fd);
        return -1;
    }

    while (1) {
        char buf[100];

        ret = recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
        if (ret < 0) {
            break;
        }
    }

    close(fd);

}

```

compile and run the above program on one console. Run it again on the other console, observe the output of the failure in `bind` system call.

