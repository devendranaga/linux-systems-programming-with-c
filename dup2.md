## dup and dup2

The `dup` and `dup2` system calls duplicates the file descriptors.

The `dup` system call creates a copy of the file descriptor.

the `dup` system call prototype is as follows:

```c
int dup(int oldfd);
```


The `dup2` system call on the other hand makes a new copy of the old file descriptor. If the new file descriptor is open it closes and performs a `dup` system call.

The `dup2` system call prototype is as follows:

```c
int dup2(int olfd, int newfd);
```

`dup2` closes the file descriptor `newfd` and dups the `oldfd` into `newfd`. The `newfd` is then returned. 

If the `oldfd` and `newfd` are same, then nothing is closed and the `newfd` is returned.



The following example gives a brief description of the `dup` system call.

```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	char *msg = "printing \n";
	int fd1, fd2;

	fd1 = dup(1);
	fd2 = dup(fd1);

	printf("%d %d\n", fd1, fd2);

	write(fd1, msg, strlen(msg) + 1);
	return 0;
}
```
