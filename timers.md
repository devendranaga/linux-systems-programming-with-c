# timers

A timer counts down to 0 from a specific value. In operating systems, the timer upon expiry, allows a program to perform specific actions. There are two timers, specifically one shot and periodic.

A one shot timer runs only once. A periodic timer repeats itself upon every expiration. Some programs need the timer to be tick at smaller intervals and with lesser resolutions. the `time_t` variable can be used for this purpose. The `time_t` is of resolution in seconds.

## alarm

`alarm()` arranges for a `SIGALRM` signal to be delivered to the calling process. The `alarm` can be thought of as one shot timer.

Below example provides a oneshot timer implementation with the `alarm`. The weblink to the program is [here](https://github.com/DevNaga/gists/blob/master/alarm_book.c)

```c
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

void sigalrm_handler(int sig)
{
    printf("Alarm signal\n");
}

int main(int argc, char **argv)
{
    signal(SIGALRM, sigalrm_handler);
    alarm(2);
    while(1);
    return 0;
}
```

compile and run the program as follows.

```bash
root@b516cef12271:~/books# gcc alarm.c
root@b516cef12271:~/books# ./a.out
Alarm signal
```

Removing the `while (1);` would make the program stop instead of waiting in the alarm. This means that the alarm function registers a timer in the kernel and returns. The kernel, upon a timer expiry, triggers a SIGALRM signal that, when handled by a program, the signal handler is invoked.

The waiting is done in the `while` statement so as to allow the kernel to trigger the timer for this running process.

## setitimer

The `setitimer` API provides either a one shot or an interval timer. When the timer fires, the OS activates the SIGALRM signal for each expiry. Before the `setitimer` we register a signal handler for the SIGALRM. The `setitimer` thus invokes the signal handler indirectly upon each expiry.

the `setitimer` API prototype is as follows.

```c
int setitimer(int which,
               const struct itimerval *cur,
               struct itimerval *old);
```

the `which` argument takes one of the three aguments which are `ITIMER_REAL`, `ITIMER_VIRTUAL` and `ITIMER_PROF`. Generally, for a system timer we use `ITIMER_REAL`.

The second argument is the structure pointer of type `struct itimerval`. The structure contains the following.

```c
struct itimerval {
    struct timeval it_interval;
    struct timeval it_value;
};

struct timeval {
    time_t tv_sec;
    suseconds_t tv_usec;
};

```

The structure contains an it\_value member describing the initial value for the timer and an it\_interval member that is used as a repeatable value. The timer is initialized with the it\_value and when the timer expires, the it\_interval is loaded as a next expiry value and is repeated again and again.

for ex:

```c
struct itimerval it = {
    .it_interval.tv_sec = 1;
    .it_interval.tv_usec = 0;
    .it_value.tv_sec = 2;
    .it_value.tv_usec = 0;
};

```

The it\_value is initialized to 2 seconds and so after the 2 secs the timer expires and calls the signal handler that is registered. upon the expiry, the it\_interval is loaded into the timer as the new expiry time that is 1 sec. At every expiry the 1 sec timeout value is loaded back as a next triggering timeout.

The `setitimer` behaves as a one shot timer when the it\_interval argument is 0.

Below is the example of the `setitimer`.

```c
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

void signal_handler(int sig)
{
    struct timeval tv;

    gettimeofday(&tv, 0);
    printf("timer interrupt %ld.%ld\n", tv.tv_sec, tv.tv_usec / 1000);
}

int main(int argc, char **argv)
{
    int ret;
    int timerval;
    int repeat = 0;

    struct sigaction new_sig;

    if (argc != 3) {
        fprintf(stderr, "%s <msec timeout value> <repeat enable (1 / 0)>\n", argv[0]);
        return -1;
    }

    timerval = atoi(argv[1]);
    repeat = atoi(argv[2]);

    memset(&new_sig, 0, sizeof(new_sig));

    new_sig.sa_handler = signal_handler;

    ret = sigaction(SIGALRM, &new_sig, NULL);
    if (ret != 0) {
        fprintf(stderr, "failed to setup SIGACTION for SIGALRM\n");
        return -1;
    }

    if (timerval != 0) {
        struct itimerval iv;

        memset(&iv, 0, sizeof(iv));

        iv.it_value.tv_sec = 0;
        iv.it_value.tv_usec = timerval * 1000;
        if (repeat) {
            iv.it_interval.tv_sec = 0;
            iv.it_interval.tv_usec = timerval * 1000;
        }

        ret = setitimer(ITIMER_REAL, &iv, NULL);
        if (ret != 0) {
            perror("setitimer");
            fprintf(stderr, "failed to setitimer\n");
            return -1;
        }
    }

    while (1);

    return 0;
}
```

The program first registers the SIGALRM signal via `sigaction` and registers the `setitimer` with the given input values. The periodicity of the timeout is controlled via the repeat argument.

## timer\_create

## timerfd

**1. timerfd_create**

`timerfd_create` notifies the timer events via the file descriptors. The returned file descriptor is watchable via `select` or `epoll` calls.

The prototype is as follows.

```c
int timerfd_create(int clockid, int flags);
```

The `clockid` refers to the clock that can be used to mark the timer progress. It is one of the `CLOCK_REALTIME` or `CLOCK_MONOTONIC`. Monotonic clock is a non settable clock that progresses constantly over the time. Meaning that the clock will stay constant although there are changes in the system time.

The `flags` argument is usually kept 0 by default.

**2. timerfd_settime**

The `timerfd_settime` arms or disrams the timer value referred by the file descriptor.

The `timerfd_settime` prototype is as follows.

```c
int timerfd_settime(int fd, int flags,
                    const struct itimerspec *new_value,
                    struct itimerspc *old_value);
```


The `itimerspec` looks as below.

```c
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

struct itimerspec {
    struct timerspec it_interval;
    struct timerspec it_value;
}
```

The `it_value` is taken as initial value of the timer. As soon as the timer is fired, the `it_interval` is stored into the timer as the new value. If `it_interval` is set to 0, then the timer becomes a oneshot timer. 

The below program explains the `timerfd_` calls.

```c

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <sys/select.h>

int main(int argc, char **argv)
{
    int time_intvl;
    int ret;
    int fd;

    if (argc != 2) {
        printf("%s [timer interval in msec]\n", argv[0]);
        return -1;
    }

    time_intvl = atoi(argv[1]);

    fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (fd < 0) {
        printf("failed to timerfd_create\n");
        return -1;
    }

    struct itimerspec it = {
            .it_value = {
                .tv_sec = 0,
                .tv_nsec = 1000 * 1000ULL * time_intvl,
            },
            .it_interval = {
                .tv_sec = 0,
                .tv_nsec = 1000 * 1000ULL * time_intvl,
            },
    };

    ret = timerfd_settime(fd, 0, &it, 0);
    if (ret < 0) {
        printf("failed to timerfd_settime\n");
        return -1;
    }

    printf("fd %d\n", fd);
    struct timeval tv;
    fd_set rdfd;

    while (1) {
        FD_ZERO(&rdfd);
        FD_SET(fd, &rdfd);

        ret = select(fd + 1, &rdfd, NULL, NULL, NULL);
        if (ret > 0) {
            if (FD_ISSET(fd, &rdfd)) {
                uint64_t expiration;

                ret = read(fd, &expiration, sizeof(expiration));
                if (ret > 0) {
                    gettimeofday(&tv, 0);
                    printf("interval timer %ld.%ld, expirations %ju\n", tv.tv_sec, tv.tv_usec, expiration);
                }
            }
        }
    }

    return 0;
}

```

The expiration indicated by an event from wait mechanism i.e. either `select` or `epoll` and the value is read from the `read `system call. The 8 byte value shall be read describing the number of expirations that have occured.
