## signals

### Introduction

Signals are used to notify a process about some event. Signals are asynchornous events. The event condition may be for ex: packet received on a network interface, packet sent on a network interface, watchdog timer expiry, floating point exception, invalid memory address read / write (segfault, alignment fault) etc. The linux provides 64 different signals range from `SIGHUP` to `SIGRTMAX`. The normal signals range from `SIGHUP` to `SIGSYS` and the real time signals start from `SIGRTMIN` to `SIGRTMAX`.

The command `kill -l` on the `bash` would give us the following.

|  |  |  |  |  |
|--|--|--|--|--|
| SIGHUP | SIGINT | SIGQUIT | SIGILL | SIGTRAP |
| SIGABRT | SIGBUS | SIGFPE | SIGKILL | SIGUSR1 |
| SIGSEGV | SIGUSR2 | SIGPIPE | SIGALRM | SIGTERM |
| SIGSTKFLT | SIGCHLD | SIGCONT | SIGSTOP | SIGTSTP |
| SIGTTIN | SIGTTOU | SIGURG | SIGXCPU | SIGXFSZ |
| SIGVTARLM | SIGPROF | SIGWINCH | SIGIO | SIGPWR |
| SIGSYS | SIGRTMIN | SIGRTMIN + 1 | SIGRTMIN + 2 | SIGRTMIN + 3|
| SIGRTMIN + 4 | SIGRTMIN + 5 | SIGRMIN + 6 | SIGRTMIN + 7 | SIGRTMIN + 8 |
| SIGRTMIN + 9 | SIGRTMIN + 10 | SIGRTMIN + 11 | SIGRTMIN + 12 | SIGRTMIN + 13 |
| SIGRTMIN + 14 | SIGRTMIN + 15 | SIGRTMAX - 14 | SIGRTMAX - 13 | SIGRTMAX - 12|
| SIGRTMAX - 11 | SIGRTMAX - 10 | SIGRTMAX - 9 | SIGRTMAX - 8 | SIGRTMAX - 6 |
| SIGRTMAX - 6 | SIGRTMAX - 5 | SIGRTMAX - 4 | SIGRTMAX - 3 | SIGRTMAX - 2 |
| SIGRTMAX - 1 | SIGRTMAX |



The `SIGINT` and `SIGQUIT` are familiar to us as from the keyboard as we usually perform the `ctrl + c` (`SIGINT`) or `ctrl + \` (`SIGQUIT`) combination to stop a program.

Signals are also delivered to a process (running or stopped or waiting) with the help of `kill` command. The manual page (`man  kill`) of kill command says that the default and easier version of kill command is the `kill pid`. Where `pid` is the process ID that is found via the `ps` command. The default signal is `SIGTERM (15)`. Alternatively a signal number is specified to the `kill` command such as `kill -2 1291` making a delivery of `SIGINT(2)` signal to the process ID 1291.


The linux also provide a mechanism for sysadmins to control the processes via two powerful and unmaskable signals `SIGKILL` and `SIGSTOP`. They are fatal and the program terminates as soon as it receives them. This allows admins to kill the offending or bad processes from hogging the resources.

The linux also provide us a set of system calls API to use the signals that are delivered to the process. Some of the most important API are `sigaction` and `signal`. Lately `signalfd` is introduced that delivers the signals in a synchronous fashion to safely control the signals that occur at unknown or surprisingly.

handling of the signals:

1. ignore the signal - SIG_IGN (ignore the signal by specifying this)
2. perform the handler function execution - create a callback handler that gets called when signal occurs
3. default action - is default for each process when created - kernel creates a default handler for each process

Signals are asynchornous and must be handled. So the system call interface provides an API to handle the signals. Below described are some of the system calls.

### signal and sigaction

`signal` is a system call, that allows the program to handle the signal when it occurs.

prototype:

```c

sighandler_t signal(int signum, sighandler_t handler);

```

The first argument is the signal number. The second argument is the signal handler callback.

An example of system call is shown below.


```c

void signal_callback(int sig)
{
    printf("signal handler\n);
}

int main()
{
    signal(SIGINT, signal_callback);

    sleep(2);
}

```

above program registers `SIGINT` that's `ctrl + c` combination. A `ctrl + c` key combination is pressed when the program is running, it invokes the `signal_callback`.


to set the signal to default we must use `SIG_DFL` in the callback argument. Such as,

```c

signal(SIGINT, SIG_DFL);

```


#### Sigaction


The `sigaction` system call is more sophisticated system call for signal handling. The prototype of `sigaction` system call is as follows,

```c

int sigaction(int signum, const struct sigaction *act,
                struct sigaction *oldact);

```

The structure `sigaction` is shown as below,

```c

struct sigaction {
    void (*sa_handler)(int);
    void (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer)(void);
}

```

The first argument is the signal number.
The second argument is the structure `sigaction`.

The `sa_flags` contain the following information. It provides a various set of flags but only the following list is useful.

1. `SA_SIGINFO` - the callback `sa_sigaction` is used when flags are set to this.
2. 0 - a default `sa_handler` should be set

#### Key differences

A very good [stackoverflow question](http://stackoverflow.com/questions/231912/what-is-the-difference-between-sigaction-and-signal) here tells us why `sigaction` is better than `signal`. The key differences are:

1. the `signal()` does not necessarily block other signals from arriving while the current signal is being executed. Thus when more than one signal occur at the same time, it becomes more problematic to understand and perform actions. If it is on the same data, this might even get more complex. The `sigaction()` blocks the other signals while the handler is being executed.

2. As soon as the `signal()` handler is executed, the `signal()` sets the default handler to `SIG_DFL` which may be `SIGINT / SIGTERM / SIGQUIT` and the handler must reset the function back to itself so that when the signal occur again, the signal can be handled back. However, with the signal() allowing the handler to be called even though the handler is already executing, this will implicate a serious problem where in which the small window between the register and default would let the program go into `SIG_DFL`.

### sigwaitinfo

### sigtimedwait

### sigwait

The system call `sigwait` is used to wait for signal until one of the signals specified in the signal mask are pending.

```c

int sigwait(const sigset_t *set, int *sig);

```

the second argument `sig` contain the returned signal number.

The `sigwait` returns 0 on success and returns a positive error on failure.

### signalfd

signalfd is a new system call introduced by the linux kernel. The signalfd system call returns a file descriptor associated with the signal. This file descriptor is then used to wait on the select, poll or epoll system calls. Unlike the `signal` or `sigaction` the signals are queued in the socket buffer and can be read on the socket.

The prototype of the `signalfd` is as follows.

`int signalfd(int fd, const sigset_t *mask, int flags);`

To use the `signalfd` one must include `<sys/signalfd.h>`.

If the fd argument is -1, the `signalfd` returns a new file descriptor. If fd argument is not -1, then the fd that is returned from previous calls to the `signalfd` must be given as an argument.

The mask argument is similar to the one that we pass to the `sigprocmask` system call. This allows the `signalfd` to create an `fd` out for the mask. As the mask is created, the signals in the mask should be blocked with the `sigprocmask`. This allows the correct functionality of the `signalfd`.

The `flags` argument is usually set to 0. It is much similar to the O_NONBLOCK flag options of other system calls.

Include `<sys/signalfd.h>` to use the `signalfd` system call.

Below is an example of the `signalfd` system call. Download [here](https://github.com/DevNaga/gists/blob/master/sigfd.c)

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <string.h>

int main(int argc, char **argv)
{
    int sfd;
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGINT);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
        printf("afiled to sigprocmask\n");
        return -1;
    }

    sfd = signalfd(-1, &mask, 0);
    if (sfd < 0) {
        printf("failed to get signalfd\n");
        return -1;
    }

    while (1) {
        struct signalfd_siginfo sf;
        int ret;

        memset(&sf, 0, sizeof(sf));
        ret = read(sfd, &sf, sizeof(sf));
        if (ret != sizeof(sf)) {
            printf("invalid length of siginfo %d received\n", ret);
            return -1;
        }
        
        printf("pid %d signal value: %d signal code: %d\n",
        				sf.ssi_pid,
        				sf.ssi_signo,
        				sf.ssi_code);

        if (sf.ssi_signo == SIGQUIT) {
            printf("received termination signal\n");
        } else if (sf.ssi_signo == SIGINT) {
            printf("received interrupt\n");
        } else {
            printf("invalid signal %d\n", sf.ssi_signo);
        }
    }

    close(sfd);

    return 0;
}
```

The signals are first masked by the `sigaddset` system call and then blocked with `sigprocmask`. The mask is then given to the `signalfd` system call. The signals are then queued to the socket fd returned. This can be waited upon the `read` or `select` system call.

The kernel returns a variable of the form `signalfd_siginfo` upon read.The structure then contains the signal that is occured. Here's some contents in the `signalfd_siginfo`.

```c
struct signalfd_siginfo {
    uint32_t ssi_signo;
    ...
    uint32_t ssi_pid;
    ....
};

```

The `ssi_signo` contains the signal number that is occured. The `ssi_pid` is the process that sent this signal.

**Example: signalfd basic example**

### sigaddset

`sigaddset` adds the signal to a set. The prototype is as follows.

```c

int sigaddset(sigset_t *set, int signo);

```

The `signo` gets added to the `set`. Multiple calls of the `sigaddset` on the same `set` would add the signals to the set. The function is mostly used in generating a signal mask for the `sigprocmask`. See more about `sigprocmask` in the below sections.

usually, the `sigaddset` is called this way:

```c

sigset_t set;

sigaddset(&set, SIGINT); // ignore SIGINT

```

ignores the signal `SIGINT`.

To setup a group of signals, the `sigaddset` can be allowed to call in a loop. For example,

```c

int i = 0;
int signal_list[] = {SIGINT, SIGQUIT, SIGALRM};
sigset_t set;

// setup signal mask for the signals in signal_list
while (i < sizeof(signal_list) / sizeof(signal_list[0])) {
    sigaddset(&set, signal_list[i]);    
}

```



### sigfillset

`sigfillset` initializes the `set` to full, including all the signals. The prototype is as follows.

```c
int sigfillset(sigset_t *set);
```

### sigemptyset

`sigemptyset` clears the signal mask. The initialization of the set of type `sigset_t` is usually done with the `sigemptyset`. Before any calls to `sigaddset` the set of type `sigset_t` must be cleared with `sigemptyset`.

The above examples calls of `sigaddset` must use `sigemptyset`. So the fixed code examples look like the below,

```c

sigset_t set;

sigemptyset(&set); // clear the signal set

sigaddset(&set, SIGINT); // ignore SIGINT

```


```c

int i = 0;
int signal_list[] = {SIGINT, SIGQUIT, SIGALRM};
sigset_t set;

sigemptyset(&set); // clear the signal set

// setup signal mask for the signals in signal_list
while (i < sizeof(signal_list) / sizeof(signal_list[0])) {
    sigaddset(&set, signal_list[i]);    
}

```


### sigismember


`sigismember` validates if the given signal is with in the set. Prototype is as follows,

```c

int sigismember(sigset_t *set, int no);

```

usually, the calling example looks like the following way,

```c

sigismember(&set, SIGALRM);

```

below is one of the examples of using both `sigfillset` and `sigismember`.

```c

/**
 * sigismember and sigfillset example
 *
 * Author: Devendra Naga (devendra.aaru@gmail.com)
 *
 * LICENSE MIT
 */

#include <stdio.h>
#include <signal.h>

int main()
{
    sigset_t set;

    sigfillset(&set);

    if (sigismember(&set, SIGALRM)) {
        printf("sigalrm is a member of the set\n");
    } else {
        printf("sigalrm is not a member of the set\n");
    }
}

```


### sigdelset

`sigdelset` deletes the signal from the given set. The prototype is as follows,

```c

int sigdelset(sigset_t *set, int no);

```


### sigprocmask

The system call `sigprocmask`, used to block certain signals.

The `sigprocmask` prototype is as follows (from the man pages),

```c

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);

```

`how` is defined by one of the following, `SIG_BLOCK` and `SIG_UNBLOCK`.

Below is an example use of `sigprocmask`.

```c

/**
 * example sigprocmask
 *
 * Author: Devendra Naga (devendra.aaru@gmail.com)
 *
 * LICENSE MIT
 */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main()
{
    sigset_t set;
    int ret;

    sigemptyset(&set);

    // add SIGINT
    sigaddset(&set, SIGINT);

    ret = sigprocmask(SIG_BLOCK, &set, NULL);
    if (ret != 0) {
        perror("sigprocmask");
        return -1;
    }

    int count = 0;

    while (1) {
        printf("hello .. %d\n", count ++);
        sleep(1);
        if (count == 5) {
            break;
        }
    }

    ret = sigprocmask(SIG_UNBLOCK, &set, NULL);
    if (ret != 0) {
        perror("sigprocmask");
        return -1;
    }
}

```

In the above example, the signal set of type `sigset_t` is created with `sigemptyset` followed by the `sigaddset`. The signal `SIGINT` being setup in the signal set.

The `sigprocmask` system call is made with `SIG_BLOCK` that effectively blocks the signal `SIGINT` till the loop below executes. The loop is created only for testing purposes to see if the `SIGINT` is actually blocked by holding down `ctrl +c` combination on the keyboard. Till the count of 5, the signal is blocked and a call to the `sigprocmask` with `SIG_UNBLOCK` is made to unblock the signal.

