# processes


Process is a binary program that is currently running in the linux system. It can be viewed by using the `ps` command.

A binary program can be created just as we create `a.out` files using the gcc compiler. The program when run, becomes the process.

Each process by default, gets three file descriptors that are attached to it. They are `stdin`, the standard input pipe, `stdout`, the standard output pipe,  `stderr`, the standard error pipe.

Linux is a multi-process operating system. Each process gets an illusion that it has the access to the full hardware for itself. Each process is interleaved in execution using the concept called scheduling. The scheduler, gets woke up by the underlying hardware timer, runs and finds out a thread with the next higher priority and wakes the process. The woken up process will then execute on the CPU.

In linux `stdin` is described as file descriptor 0, `stdout` is described as file descriptor 1, and `stderr` is described as file descriptor 2. Upon the each successful pipe / msgqueue / socket call, the file descriptors will get incremented in number usually (i.e from 3, 4 and so on) most of the time. A file descriptor that is opened must always be closed with a corresponding `close` system call or the similar one. This is a good programming practice.

Each process is identified by a pid (called process identifier). Process ids are 16 bit numbers (Signed numbers). The program can obtain its pid using `getpid()` call. The parent process id is obtained by using `getppid()`. 

**NOTE**: In linux the process 1 is always the init process.

The below program gets the pid and parent pid.

```
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("process id %d\n", getpid());
    printf("parent process id %d\n", getppid());
    
    return 0;
}

```

The parent process of the program that we have currently executed is always the shell that we ran the binary.

The below example is for the `ps` command.

```
  PID TTY           TIME CMD
22963 ttys000    0:00.29 -bash
22860 ttys001    0:00.01 -bash
22863 ttys002    0:00.02 -bash
```

pid 0 is a swapper process and pid 1 is an init process. The init process is the one that is called by the kernel to initialise the userspace.

A process has the following states:

**Running** -  The process is either running or is ready to run.

**Waiting** - The process is waiting for an event or a resource. Such as socket wait for the recv etc.

**Stopped** - The process has been stopped with a signal such as Ctrl + Z combination or under a debugger.


When a process is created, the kernel allocates enough resources and stores the information about the process. When a process is stopped, the information and all the allocated memory and resources will then automatically be freed up by the kernel.

