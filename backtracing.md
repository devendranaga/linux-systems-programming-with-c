# backtracing

At some situations such as crash, it is at most useful to trace the function calls. One possible way to find this is to perform `gdb` on the program and typing `bt` when the crash occured. The other possible way to simply print the dump using some of the C APIs such as `backtrace` and `backtrace_symbols`.

The function prototypes are as follows.

`int backtrace(void **buffer, int size);`

`char *backtrace_symbols(void *const *buffer, int size);`

The `backtrace` function returns a backtrace of the calling program, into the array pointed to by buffer. provide `size` large enough to accomodate all the addresses.

`backtrace` API collects all the fubnction call addresses into the buffer.

The `backtrace_symbols` function translates the addresses into the strings. 

The header file `<execinfo.h>` contains the prototypes for these API.

The example of such is follows.

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

`[root@localhost manuscript]# gcc -rdynamic backtrace.c  -g`

Without the -g or -rdynamic the backtrace that is producted may not contain the needed symbols.

run the program as follows thus producing the following output.

```
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

