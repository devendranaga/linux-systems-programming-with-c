## select and epoll

### 1. select system call

`select` system call is used to wait on multiple file descriptors at the same time. The file descriptors can be a file, socket, pipe, message queue etc.

`FD_SET` is used to set the corresponding file descriptor in the given `fd_set`.

`FD_ISSET` is used to test if the corresponding file descriptor is set in the given `fd_set`.

`FD_ZERO` resets a given `fd_set`.

`FD_CLR` clears an fd in the `fd_set`.

`select` can also be used to perform millisecond timeout. This can also be used to selective wait for an event or a delay.

the `select` from the manual page looks like below:

```c
int select(int nfd, FD_SET *rdfd, FD_SET *wrfd, FD_SET *exfd, struct timeval *timeout);
```

The `select` returns greater than 0 and sets the file descriptors that are ready in the 3 file descriptor sets. and returns `0` if there is a timeout.

A TCP server using the select loop is demonstrated below:

The select accepts 3 sets of file descriptor sets. Read fdset, Write fdset, Except fdset. Of all we only use read fdset. We do not really need write fdset and except fdset in most of the cases.

The select when returned \(and with no error\), the above fdsets should be tested with the FD\_ISSET with the list of FDs that we are interested in.

The below two programs can be downloaded here: [Server](https://github.com/DevNaga/gists/blob/master/tcp_select_server.c) and [Client](https://github.com/DevNaga/gists/blob/master/tcp_client.c)

```c
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define TCP_SERVER_PORT 21111

int main(int argc, char **argv)
{
    int ret;
    fd_set rdset;
    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
        .sin_port = htons(TCP_SERVER_PORT),
    };

    int max_fd = 0;
    int set_reuse = 1;
    int serv_sock;

    int client_list[100];
    int i;

    for (i = 0; i < sizeof(client_list) / sizeof(client_list[0]); i ++) {
        client_list[i] = -1;
    }

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock < 0) {
        return -1;
    }

    ret = setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &set_reuse, sizeof(set_reuse));
    if (ret < 0) {
        return -1;
    }

    ret = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
    if (ret < 0) {
        return -1;
    }

    ret = listen(serv_sock, 100);
    if (ret < 0) {
        return -1;
    }

    if (max_fd < serv_sock)
        max_fd = serv_sock;

    FD_ZERO(&rdset);

    FD_SET(serv_sock, &rdset);

    while (1) {
        int clifd;
        fd_set allset = rdset;

        ret = select(max_fd + 1, &allset, 0, 0, NULL);
        if (ret > 0) {
            if (FD_ISSET(serv_sock, &allset)) {
                clifd = accept(serv_sock, NULL, NULL);
                if (clifd < 0) {
                    continue;
                }

                for (i = 0; i < sizeof(client_list) / sizeof(client_list[0]); i ++) {
                    if (client_list[i] == -1) {
                        client_list[i] = clifd;
                        FD_SET(clifd, &rdset);
                        if (max_fd < clifd)
                            max_fd = clifd;
                        printf("new fd %d\n", clifd);
                        break;
                    }
                }
            } else {
                for (i = 0; i < sizeof(client_list) / sizeof(client_list[0]); i ++) {
                    if ((client_list[i] != -1) &&
                        (FD_ISSET(client_list[i], &allset))) {
                        char buf[100];

                        printf("client %d\n", client_list[i]);
                        ret = recv(client_list[i], buf, sizeof(buf), 0);
                        if (ret <= 0) {
                            printf("closing %d\n", client_list[i]);
                            FD_CLR(client_list[i], &rdset);
                            close(client_list[i]);
                            client_list[i] = -1;
                            continue;
                        }

                        printf("read %s\n", buf);
                    }
                }
            }
        }
    }

    return 0;
}

```

The tcp sample client is defined below:

```c
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TCP_SERVER_PORT 21111

int main()
{
    int cli_sock;
    int ret;

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
        .sin_port = htons(TCP_SERVER_PORT),
    };

    cli_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (cli_sock < 0) {
        return -1;
    }

    ret = connect(cli_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
    if (ret < 0) {
        return -1;
    }

    char msg[] = "sending data to the server";

    send(cli_sock, msg, strlen(msg) + 1, 0);

    close(cli_sock);

    return 0;
}

```

The above two programs are only demonstratable programs. They have many errors. Finding and spotting the errors is going to be your task here. And the solution to it makes us to become a better programmer.

The timeout argument used for the timer events or used as a timer callback.

```c
struct timeval timeout = {
    .tv_sec = 0,
    .tv_usec = 250 * 1000,
};

select(1, NULL, NULL, NULL, &timeout);
```

The above code waits for the timeout of 250 milliseconds and the `select` call returns 0. The `select` may not wait for the exact timeout and for this, more accurate timing APIs must be used. such as the `timer_create`, `setitimer`, or `timerfd_create` set of system calls. We will read more on the timers in the **Time and Timers** chapter.

We can use this mechanism to program a timer along with the sockets. We are going to demostrate this feature in the event library section of this book.

The `select` system call cannot serve maximum connections more than `FD_SETSIZE`. On some systems it is 2048. This limits the number of connections when the server program use this call. Thus the `select` call is not a perferred approach when using with a large set of connections that are possibly for the outside of the box.

### 2. poll system call

### 3. epoll system call

`epoll` is another API that allows you to selectively wait on a large set of file descriptors. Epoll solves the very less number of client connections with `select` .

The `epoll` is similar to `poll` system call. The system call is used to monitor multiple file descriptors to see if I/O is possible on them.

The `epoll` API can be used either as an edge triggered or level triggered interface and scales well as the file descriptors increase.

The `epoll` API is a set of system calls.

| API | description |
|-----|-------------|
| `epoll_create1` | create an epoll context and return an fd |
| `epoll_ctl` | register the interested file descriptors |
| `epoll_wait` | wait for the I/O events |


`epoll_create1` creates an epoll context. The context returned is the file descriptor. The file descriptor is then used for the next set of
API. On failure the `epoll_create1` return -1.

The prototype of the `epoll_create1` is as below.

```c
int epoll_create1(int flags);
```

The `flags` argument is by default set to 0.

The `epoll_ctl` is a control interface that is used to add, modify or delete a file descriptor. The prototype is as follows.

```c
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
```

The `epfd` is the file descriptor returned from the `epoll_create1` system call.

The `op` argument is used to perform the add, modify or delete operations. It can be one of the following.

1. EPOLL_CTL_ADD
2. EPOLL_CTL_DEL
3. EPOLL_CTL_MOD

The `event` object is used to store the context pointer of the caller.

the `struct epoll_event` is as follows.

```c
typedef union epoll_data {
    void *ptr;
    int fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;

struct epoll_event {
    uint32_t events;  // epoll events
    epoll_data_t data; // user data
}
```

The `events` member can be one of the following.

`EPOLLIN`: The file descriptor is available for reading.
`EPOLLOUT`: The file descriptor is available for writing.


The `epoll_wait` system call waits for events on the returned epoll fd.

The `epoll_wait` prototype is as follows.

```c
int epoll_wait(int epfd, struct epoll_event *events,
               int maxevents, int timeout);
```

The `timeout` argument specifies the timeout in milliseconds to wait.

On success the number of file descriptors ready for the requested I/O are returned, or 0 if none of them are ready during the timeout. On error -1 is returned.

The example of the `epoll` command is shown below. You can also download it [here](https://github.com/DevNaga/gists/blob/master/epoll_test.c)

```c
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

int main(int argc, char **argv[])
{
    int ret;
    int stdin_fd = 0;
    int ep_fd;

    ep_fd = epoll_create1(EPOLL_CLOEXEC);
    if (ep_fd < 0)
        return -1;

    struct epoll_event ep_events;

    memset(&ep_events, 0, sizeof(struct epoll_event));

    ep_events.events = EPOLLIN;
    ep_events.data.fd = stdin_fd;

    if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, stdin_fd, &ep_events) < 0)
        return -1;

    for (;;) {
        int fds;
        struct epoll_event evt;

        fds = epoll_wait(ep_fd, &evt, 1, -1);
        printf("fds %d\n", fds);

        if (evt.data.fd == stdin_fd) {
            printf("read from stdin\n");
            char buf[100];

            memset(buf, 0, sizeof(buf));
            read(stdin_fd, buf, sizeof(buf));
            printf("input %s\n", buf);
        }
    }

    return 0;
}
```
