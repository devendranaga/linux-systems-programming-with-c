# Uname

The `uname` system call provides system information.

The prototype from the manual page is follows.

```c
#include <sys/utsname.h>

int uname(struct utsname *buf);
```

It returns `utsname` structure filling the content.

The `utsname` structure looks as follows.


```c
struct utsname {
    char sysname[];    /* Operating system name (e.g., "Linux") */
    char nodename[];   /* Name within "some implementation-defined
                                     network" */
    char release[];    /* Operating system release
                                     (e.g., "2.6.28") */
    char version[];    /* Operating system version */
    char machine[];    /* Hardware identifier */
    #ifdef _GNU_SOURCE
        char domainname[]; /* NIS or YP domain name */
    #endif
};
```

Below is one example,

```c
#include <stdio.h>
#include <sys/utsname.h>

int main()
{
    struct utsname buf;
    int ret;

    ret = uname(&buf);
    if (ret == 0) {
        printf("system name : %s\n", buf.sysname);
        printf("node name: %s\n", buf.nodename);
        printf("release: %s\n", buf.release);
        printf("verison: %s\n", buf.version);
        printf("machine: %s\n", buf.machine);
    }
}
```

