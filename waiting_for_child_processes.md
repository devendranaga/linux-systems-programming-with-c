## waiting for child processes

When a child process stops the parent process must `reap` it to prevent it from becoming a zombie. The zombie meaning that the process is not taken out from process table but they dont execute and do not utilize the memory or CPU.

When a parent process stops before the child stops, then the child process is called an `orphan` process. Often some process adapts the child process and becomes its parent. This process often is the `init` process.

When a parent waits for the child process by some means and reaps it, the zombie processes will not happen.

Waiting for a child process can be performed in the following ways.

1. calling `wait` and finding the child's pid as its return value.
2. calling `waitpid` on a specific child.

### wait

The `wait` system call suspends the execution of the parent process until one of its child processes terminates.  On success returns the process ID of the child and -1 on failure.

The prototype is as follows.

```c
pid_t wait(int *status);
```

### waitpid

The `waitpid` system call suspends the execution of the parent process until a child process specified by the `pid` argument has changed state.

The prototype is as follows.

```c
pid_t waitpid(pid_t pid, int *status, int options);
```

By default, the `waitpid` waits only for the terminated children. With the `options` argument the behavior is changed.

|pid value | meaning |
|--------|---------|
| -1 | wait for any child process |
| > 0 | wait for the child with the given process ID |

Most of the time the `options` argument is either 0 or `WNOHANG`. With `WNOHANG` the `waitpid` returns immediately if there is no child exits.

The `status` value is kept into the `waitpid` if the passed argument of `status` is non `NULL`. The value can be interpreted with the following macros.

|exit status | description |
|------------|-------------|
|`WIFEXITED(status)` | returns true if the child is exited normally |
|`WEXITSTATUS(status)` | returns the exit status of the child. call this only if `WIFEXITED(status)` returns true |
