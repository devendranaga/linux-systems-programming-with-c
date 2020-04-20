### ioctl system call

manual page of linux `ioctl_list` lists down a various set of linux supported ioctls.

Networking ioctls section describe and use a lot of `ioctl` calls to perform certain functions.

The `ioctl` system call prototype is as follow,

```c

#include <sys/ioctl.h>

int ioctl(int fd, unsigned long request, ...);

```

as shown above, `sys/ioctl.h` is the header file that has the prototype.

The man page of `ioctl_list` lists a set of ioctl numbers.

