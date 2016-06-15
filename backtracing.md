# backtracing

At some situations such as crash, it is at most useful to trace the function calls. One possible way to find this is to perform `gdb` on the program and typing `bt` when the crash occured. The other possible way to simply print the dump using some of the C APIs such as `backtrace` and `backtrace_symobls`.

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
