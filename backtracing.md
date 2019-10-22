## backtracing

At some situations such as crash, it is at most useful to trace the function calls. One possible way to find this is to perform `gdb` on the program and typing `bt` when the crash occured. The other possible way to simply print the dump using some of the C APIs such as `backtrace` and `backtrace_symbols`.

The function prototypes are as follows.

```c
int backtrace(void **buffer, int size);
char *backtrace_symbols(void *const *buffer, int size);
```

The `backtrace` function returns a backtrace of the calling program, into the array pointed to by buffer. provide `size` large enough to accomodate all the addresses.

`backtrace` API collects all the fubnction call addresses into the buffer.

The `backtrace_symbols` function translates the addresses into the strings. 

The header file `<execinfo.h>` contains the prototypes for these API.

The example of such is follows. Download [here](https://github.com/DevNaga/gists/blob/master/backtrace.c)

```c
#include <stdio.h>
#include <execinfo.h>

void function2()
{
	void *buf[300];
	char **strings;
	int i, len;

	len = backtrace(buf, 300);
	printf("returns %d\n", len);

	strings = backtrace_symbols(buf, len);
	if (strings) {
		for (i = 0; i < len; i ++) {
			printf("%s\n", strings[i]);
		}
	}
}

void function1()
{
	function2();
}

int main(void)
{
	function1();
	return 0;
}
```

compile the program as follows.

```shell
[root@localhost manuscript]# gcc -rdynamic backtrace.c  -g
```

Without the `-g` or `-rdynamic` the backtrace that is producted may not contain the needed symbols, and some of the symbols might be lost.

run the program as follows thus producing the following output.

```shell
[root@localhost manuscript]# ./a.out
returns 5
./a.out(function2+0x1f) [0x4008f5]
./a.out(function1+0xe) [0x40096f]
./a.out(main+0xe) [0x40097f]
/lib64/libc.so.6(__libc_start_main+0xf0) [0x7f04ca774fe0]
./a.out() [0x400809]
[root@localhost manuscript]#
```

Some examples of the `backtrace` use it to produce a crashtrace when the crash occur. For this, the program registers the `SIGSEGV` (segfault signal) via `signal` or `sigprocmask` call. The handler gets called at the event of the crash. The below program provides a case on such scenario.

The program registers the segfault handler with the `signal` and dereferences a null character pointer, thus resulting in a crash. The handler immediately gets called and provides us the trace of the calls made to come to the crash path. The first function in the calls will be the signal handler.

**NOTE**: When registering a signal handler for the segfault (i.e, `SIGSEGV`) please make sure to abort the program, otherwise the signal handler will be restarted continuously. To test that out, remove the abort function call in the signal handler below.

```c
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief - signal handler for SIGSEGV
 */
void segfault_handler(int si)
{
        void *crash_trace[100];
        char **stack_trace;
        size_t size;
        int i;

        size = backtrace(crash_trace, 100);

        if (size <= 0) {
                fprintf(stderr, "No symbols found \n");
                goto end;
        }

        stack_trace = backtrace_symbols(crash_trace, size);
        if (!stack_trace) {
                fprintf(stderr, "No symbols found \n");
                goto end;
        }

        fprintf(stderr, "Trace\n");
        fprintf(stderr, "----------------XXXXXX--------------\n");
        for (i = 0; i < size; i ++) {
                printf("[%s]\n", stack_trace[i]);
        }
        fprintf(stderr, "----------------XXXXXX--------------\n");
end:
        abort();
}

void function3()
{
        int *data = NULL;

        printf("Data %s\n", *data);
}

void function2()
{
        function3();
}

void function1()
{
        function2();
}

int main()
{
        signal(SIGSEGV, segfault_handler);
        function1();
}

```

Sometimes, it is necessary to dump the trace to a file by the program. This occurs when the program is running as a daemon or running in the background. The glibc provides us another function called `backtrace_symbols_fd`. This can also be useful when sending the trace over to a network socket or to a local pipe to monitor the crash and perform necessary action such as recording.

The `backtrace_symbols_fd` prints the trace into a file. Here is an example:

```c
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int fd;

void segfault_handler(int si)
{
        void *crash_trace[100];
        size_t size;

        size = backtrace(crash_trace, 100);

        if (size <= 0) {
                fprintf(stderr, "No symbols found \n");
                goto end;
        }

        backtrace_symbols_fd(crash_trace, size, fd);
end:
        abort();
}

void function3()
{
        int *data = NULL;

        printf("Data %s\n", *data);
}

void function2()
{
        function3();
}

void function1()
{
        function2();
}

int main(int argc, char **argv)
{
		if (argc != 2) {
				fprintf(stderr, "%s <trace file>\n", argv[0]);
				return -1;
		}

		fd = open(argv[1], O_CREAT | O_RDWR, S_IRWXU);
		if (fd < 0) {
				fprintf(stderr, "failed to open %s\n", argv[1]);
				return -1;
		}

        signal(SIGSEGV, segfault_handler);
        function1();
}

```



## Core dump

Sometimes, the programs crash. At some points of time, it is hard to debug what's wrong with the program. The Linux OS supports a coredump feature to overcome this situation. When the program terminates in linux, the current state of the program along with every other opened file, state information and registers etc will be dumped into the file called `core`. The core file contains an image of the process's memory at the time of termination. The coredump file can be used in `gdb` to debug the problem.

Let's see below how we can enable the coredump settings one by one.


1. The **CONFIG_COREDUMP** kernel configuration parameter need to be setup while compiling the kernel.
2. once kernel is built with **CONFIG_COREDUMP**, the system is restarted into the built kernel.
2. The coredump file name is to be configured in **/proc/sys/kernel/core_pattern**.


The below table describe a format of the coredump that could be configured.


| format | description                              |
| ------ | ---------------------------------------- |
| %%     | a single % character                     |
| %c     | core file size soft resource limit of crashing process |
| %d     | dump mode                                |
| %e     | executable filename                      |
| %E     | pathname of the executable               |
| %g     | real GID of the dumped process           |
| %h     | hostname                                 |
| %i     | TID of the thread that triggered the core-dump |
| %p     | PID of the dumped process                |
| %s     | number of signal causing dump            |
| %t     | time of dump expressed since the epoch   |


In the above table, if you see, the most important ones are `hostname`, `executable filename`, `number of signals` and the `time of dump`.

1. The `hostname` would tell us on which computer the crash was on, this is very useful if the crash is on a large network of systems running virtualised machines etc.. (such as docker).
2. The `executable filename` will pin-point to the process or the software that we should debug
3. the `number of signals` will tell us that if this is a cause of asnychrnous signals / unknown signal triggers
4. the `time of dump` would give us a chance in time that the crash occured at this point in time and that we could compare this time with the log or usually a syslog to see what log messages are spewed in it at the time of this crash.

The coredump can be configured with `sysctl` as well.. as root

```bash
sysctl -w kernel.core_pattern=/tmp/core_%h_%e_%s_%t
```

this will create a corefile under /tmp/ with name `core_${host}_${exename}_${number_of_signal}_${timeofdump}`.

Corefiles are usually huge, and they should be in a mount point where there is sufficient memory (they are big of order of 40 MB on an embedded system).  If the mount point, does not contain sufficient memory, the directory of the corefiles should be managed and used or older files must be deleted in order to capture new coredump.

Corefiles are usually analysed by using the GCC. The following is the list of steps that are used to analyze the core file to a program that links to the shared libraries.

```bash
gdb <binary_name>

# in gdb
set solib-searchpath path/to/shared_libraries

# in gdb
core-file <core_file_name>

# in gdb
bt

# in gdb
bt full # dumps the full contents of the stack and the history of the program at this point

```

A brief description on the `set solib-searchpath path/to/shared_libraries` command in the gdb:

1. The **path/to/shared_libraries** are the target shared libraries.
2. in case if your compiler is native and running on a native system, then you can simply point that to /usr/lib/ or /lib/.
3. in case if your compiler is a cross compiler such as for target ARM or MIPS .. you should point the path to the toolchain of those cross compiled libs that the program is linked against.


The above program dumps the trace to the crash and points to the line number of a c program. The program must be compiled with **-g** option while using the `gcc` to get a proper trace value.

usually `bt` is prefered over `bt full` because `bt` generally gives an idea without going in detail about any problem.
Cases, where a full debug and history are required, `bt full` is generally used.


