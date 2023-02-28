## chmod system call

The `chmod` system call changes the permissions of a file.

the header file `<sys/stat.h>` contains the prototype of the `chmod` system call.

The prototype of `chmod` is as follows.

```c
int chmod(const char *path, mode_t mode);
```

There is one another system call `fchmod`, it does the same job as `chmod` does, but operates instead on a file descriptor.

The prototype of `fchmod` is below.

```c
int fchmod(int fd, mode_t mode);
```

**Example:**

The below program adds the permissions 0777 to the file. The file is taken as input. You can also download it  [here](https://github.com/DevNaga/gists/blob/master/chmod.c).

```c
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
   int ret;

   if (argc != 2) {
       printf("%s <filename>\n", argv[0]);
       return -1;
   }

   ret = chmod(argv[1], S_IRUSR | S_IWUSR | S_IXUSR |
                        S_IRGRP | S_IWGRP | S_IXGRP |
                        S_IROTH | S_IWOTH | S_IXOTH);
   if (ret != 0) {
       printf("failed to chmod [%s] \n", argv[1]);
       return -1;
   }

   return 0;
}
```

The same example with fchmod is given below, Download [here](https://github.com/DevNaga/gists/blob/master/fchmod.c)

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int fd;
    int ret;

    if (argc != 2) {
        fprintf(stderr, "<%s> filename\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "failed to open %s\n", argv[1]);
        return -1;
    }

    ret = fchmod(fd, 0777);
    if (ret < 0) {
        fprintf(stderr, "fchmod failure %s\n", strerror(errno));
    } else {
        fprintf(stderr, "succesfully set permissions of file %s\n", argv[1]);
    }
    close(fd);

    // perform stat() to verify
    struct stat s_;

    ret = stat(argv[1], &s_);
    if (ret < 0) {
        fprintf(stderr, "failed to stat %s\n", argv[1]);
        return -1;
    }

    fprintf(stderr, "file permissions %o\n", s_.st_mode);

    return 0;
}
```

above example, operates on a file descriptor setting the file in read + write + execute mode (this is the mode usually operated by the executable files). After setting permissions, the file permissions are verified with the `stat` system call.

Trying the same program on `/dev/null` gets into permissions denied error. (`EPERM`)

```bash
./a.out /dev/null 
fchmod failure Operation not permitted
file permissions 20666
```

**permission bits**

The below permission bits describe the mode settings when setting them for a file.

| permission value | bit mask |
|------------------|----------|
| S_ISUID | 04000 |
| S_ISGID | 02000 |
| S_ISVTX | 01000 |
| S_IRUSR | 00400 |
| S_IWUSR | 00200 |
| S_IXUSR | 00100 |
| S_IRGRP | 00040 |
| S_IWGRP | 00020 |
| S_IXGRP | 00010 |
| S_IROTH | 00004 |
| S_IWOTH | 00002 |
| S_IXOTH | 00001 |

To give acess to user READ , WRITE and EXEC, then we need `S_IRUSR | S_IWUSR | S_IXUSR`.

Remember that when giving the permissions, understand first if the certain user / group really require the permissions.

1. For example, not every file needs to have execute permissions. So default mode could become 0666 for non executable files.
2. For example, not every file needs to be accessed by the group and others in write mode. So the default mode could further optimized to 0644.
3. For example, not every user need to read certain system level files. So the default mode could even become 0640.
4. Executable files does not have to be group and other accessible when run by certain users. So permissions become 0100.

Reasoning behind this is that, system permissions can be exploited by the unprivileged users such as others (everyone else) and could potentially lead to
faults or privilege escalations or code execute in privilege modes. Avoid this by explicitly defining permission bits on each and every file in the system.

Now, one cannot simply define permission bits on every file, but build systems such as openwrt, yocto or the buildroot can provide such configurability while creating the root file system.

## chown system call

## access system call

The `access` system call prototype is as follows.

```c
int access(const char *pathname, int mode);
```

`access` system call checks whether the calling process can access the `pathname`. The mode is one of the following.

| mode | description|
|------|------------|
| `F_OK` | check if the file exists |
| `R_OK` | check if the file is read accessible |
| `W_OK` | check if the file is write accessible |
| `X_OK` | check if the file is executable accessible |

include the header file `<unistd.h>`. The otherway to check the existence of the file is to use `access` system call instead of using `stat` system call for it.

**Example:**

The below program depicts the usage of the `access` API. You can also download it in [here](https://github.com/DevNaga/gists/blob/master/access.c).

```c
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "%s [filename] [mode]\n", argv[0]);
        return -1;
    }

    int ret;
    int mode;

    if (!strcmp(argv[2], "F_OK")) {
        mode = F_OK;
    } else if (!strcmp(argv[2], "R_OK")) {
        mode = R_OK;
    } else if (!strcmp(argv[2], "W_OK")) {
        mode = W_OK;
    } else if (!strcmp(argv[2], "X_OK")) {
        mode = X_OK;
    } else {
        fprintf(stderr, "invalid option %s entered\n", argv[2]);
        return -1;
    }

    ret = access(argv[1], mode);
    if (ret < 0) {
        fprintf(stderr, "failed to access %s error : %s\n",
                                argv[1], strerror(errno));
        return -1;
    }

    fprintf(stderr, "%s is ok\n", argv[2]);
    return 0;
}
```
