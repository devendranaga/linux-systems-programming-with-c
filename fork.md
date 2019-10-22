## fork

The system call fork is used to create processes. The most important realworld use of fork is in the daemon processes where in which the daemon has to get attached to the pid 1 (the init process).

The process that calls fork and creates another process is called a parent process and the process that is created by the fork system call is called child process.

Fork returns twice unlike any other function in C. The fork call is executed by the kernel and it returns once in the parent process and once in the child process.

Fork returns the pid of the child process in the parent, and returns 0 in the child. If creation of a process was unsuccessful then it returns -1.

The below program gives an example of `fork` system call. Download [here](https://github.com/DevNaga/gists/blob/master/fork.c)

```c
#include <stdio.h>
#include <unistd.h>

int main()
{
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        printf("failed to fork..\n");
    } else if (pid == 0) {
        printf("child process ..\n");
    } else if (pid > 0) {
        printf("parent process..\n");
    }

    sleep(2);
    return 0;
}
```

**Example: demonstration of fork system call**

notice that when you run the above program several times, the print statements "parent process" and "child process" never come exactly one after the another for ex: parent first and then the child. This is because of the scheduling job that is done internally in the kernel.

After a child process is created the parent can continue its execution of the remaining code or can exit by calling the `exit` system call. When the parent exits before the child could run, the child becomes an orphan. This process is then parented by the init process (pid 1). This method is one of the step in creating the system daemon.

In an other case where in the child gets stopped or exited before the parent. In this case, the parent must cleanup the resources that are allocated to the child by calling one of the functions `wait`, `waitpid`. In some cases, when the parent process does not perform the task of cleanup, the child process will then go into a state called zombie state. In the zombie state, the child process although is not running, its memory is never get cleaned up by the parent process. Notice the "Z" symbol in the `ps -aux` command.

The system call `getpid` returns the process id of the process. When applied to the above example code, prints out the process ids of both parent and child.

Below is an example of `getpid`. Download [here](https://github.com/DevNaga/gists/blob/master/getpid.c)

```c
#include <stdio.h>
#include <unistd.h>

int main()
{
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        printf("failed to fork..\n");
    } else if (pid == 0) {
        printf("child process .. %d\n", getpid());
    } else if (pid > 0) {
        printf("parent process.. %d\n", getpid());
    }

    sleep(2);
    return 0;
}

```

fork system call is mostly used in the creation of system daemons.

## wait

The system call `wait` lets the parent process wait for the child process to finish.

The prototype of the `wait` systel call is below:

```c
pid_t wait(int *status);
```

The `status` is the status of the child process that is received when child process finishes its execution. The wait returns the pid of the child process. There are more than one status codes ORed together with in the `status`.

1. The `WIFEXITED` macro describe the exit status of the child. Use `WIFEXITED` macro with the `status` variable and then exit status is found with `WEXITSTATUS` macro.


Below is the example of the `wait` system call. Download [here](https://github.com/DevNaga/gists/blob/master/fork_wait.c)

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        printf("failed to fork..\n");
    } else if (pid == 0) {
        printf("child process ..\n");
        int i;

        double f = 1.0;
        for (i = 0; i < 10000000; i ++) {
            f *= i;
            f /= i;
        }
        //uncomment the below line and observe the behavior of the value at WEXITSTATUS in the parent section
        //exit(1);
    } else if (pid > 0) {
        printf("parent process..\n");
        printf("wait for child to complete the task..\n");

        int status = 0;

        pid_t pid_ = wait(&status);
        if (pid_ == pid) { // child pid
            if (WIFEXITED(status)) {
                printf("child terminated normally..\n");
                printf("exit %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("signal on child\n");
                printf("term signal %d on child \n", WTERMSIG(status));
                printf("coredump %d\n", WCOREDUMP(status));
            } else if (WIFSTOPPED(status)) {
                printf("child stopped by delivery of signal\n");
                printf("stop signal %d\n", WSTOPSIG(status));
            }
        }
    }

    return 0;
}
```

in the above program, the parent process is in the parent portion of the code, the code we handle using the `pid > 0` check. Within which the exit statuses are printed at each execution stage. The program simply adds a busy maths loop to delay the execution of the child process.

The exit status from the child is 0 on a normal condition. When the code below the child having the `exit` library function called with exit status of 1, the parent process receives the exit status as the `wait` system call output.

The status is then collected by checking `WIFEXITED` macro and with the `WEXITSTATUS` macro.

the status field is checked upon with the following macros.

| macro | meaning |
|-------|---------|
| `WIFEXITED` | returns true if the child terminated normally |
| `WEXITSTATUS` | returns the exit status of the child process, use it only when `WIFEXTITED` set to to true |
| `WIFSIGNALED` | returns true if child process is terminated by a signal |
| `WTERMSIG` | return signal number. use it if `WIFSIGNALED` returns true |
| `WCOREDUMP` | returns true if child produced core dump. use it when `WIFSIGNALEd` returns true |
| `WIFSTOPPED` | returns true if child process stopped by delivery of the signal |
| `WSTOPSIG` | return signal number.. use it when `WIFSTOPPED` returns true |

## waitpid

the `waitpid` system call has the below definition.

```c
pid_t waitpid(pid_t pid, int *status, int options);

```

the `pid` argument is of type pid of the child process. if set to -1, the `waitpid` will wait indefinitely for any child process.

The status of the child  process is copied into the `status`. Status is similar to the `wait` status values.

the options contain the folowing values. OR combination of one or more of the below values

| value | meaning |
|-------|---------|
| `WNOHANG` | return immediately if no child has exited |
| `WUNTRACED` | return if a child has stopped |
| `WCONTIUED` | return if a stopped child has continued after delivery of SIGCONT |


## waiting for all the child processes

waiting for multiple child processes can be done using the `wait` system call.

Below is the example. Download [here](https://github.com/DevNaga/gists/blob/master/allchild.c)

```c
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    pid_t pid;
    int ret;
    int i;

    for (i = 0; i < 10; i ++) {
        pid = fork();
        if (pid != 0) {
        } else {
            printf("starting %d\n", getpid());
            _exit(1);
        }
    }

    for (i = 0; i < 10; i ++) {
            pid_t child = wait(NULL);
            printf("reap %d\n", child);
    }

    return 0;
}

```

typically, in larger programs, the child processes does not expect to die soon often. sometimes, it may happen to be that the child process is died because of unexpected reasons, and thus may require a restart. Before restarting, the child process must be cleaned by using the `wait` or `waitpid` system call.


There is another way to do the same waiting. by creating pipes.

below is the procedure:

```
for each child a  pipe is created, the parent closes the write end of the pipe and the child closes the read end of the pipe.

parent then sets the read end of the file descriptor to the `fd_set` for each children.

parent waits in `select` system call to monitor the `fd_set`.

if any children die, reported as a close of socket that is write end on the child end, since parent is on the `read` end, the `select` will wake up causing the parent to call `waitpid`.

```

Below is the source code for the above algorithm. Download [here](https://github.com/DevNaga/gists/blob/master/selfpipe.c)

```c
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/select.h>

int maxfd_all(int pipefd[10][2], int len)
{
    int maxfd = 0;
    int i;

    for (i = 0; i < len; i ++) {
        if (maxfd < pipefd[i][0])
            maxfd = pipefd[i][0];
    }

    return maxfd;
}

int main()
{
    pid_t pid;
    int i;
    int ret;
    int pipefd[10][2];
    fd_set allfd;
    fd_set rdset;
    int maxfd = 0;

    FD_ZERO(&rdset);

    for (i = 0; i < 10; i ++) {
        ret = pipe(pipefd[i]);

        printf("Read end %d max fd %d\n", pipefd[i][0], maxfd);
        FD_SET(pipefd[i][0], &rdset);
        if (maxfd < pipefd[i][0])
            maxfd = pipefd[i][0];

        pid = fork();
        if (pid != 0) {
            close(pipefd[i][1]);
        } else {
            close(pipefd[i][0]);
            for (i = 0; i < 100000000; i ++) ; 
            sleep(1);
            _exit(1);
        }
    }

    while (1) {
        allfd = rdset;

        maxfd = maxfd_all(pipefd, 10);

        if (maxfd <= 0) {
            break;
        }

        ret = select(maxfd + 1, &allfd, 0, 0, NULL);
        if (ret < 0) {
            perror("select");
            return -1;
        } else {
            for (i = 0; i < 10; i ++) {
                if (FD_ISSET(pipefd[i][0], &rdset)) {
                    printf("reap child.. %d \n", pipefd[i][0]);
                    waitpid(-1, NULL, WNOHANG);
                    //close(pipefd[i][0]);
                    FD_CLR(pipefd[i][0], &rdset);
                    pipefd[i][0] = -1;
                }
            }
        }
    }

    return 0;
}

```


the above code, creates 10 child processes. Each by calling `fork` system call. Each child closes the read end and parent closes the write end. The read end of the parent process is set to the `fd_set`. The system call `select` is then used to wait for any data on the read end of the pipes. If a child process die, the pipe close triggers a read of 0 bytes, causing `select` to return. The `waitpid` is then called to reap the child process. Each child that is dead, is then cleared from the `fd_set`.

[[self pipe trick](http://cr.yp.to/docs/selfpipe.html)]

