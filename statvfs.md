## statvfs and fstatvfs

`statvfs` and `fstatvfs` system calls provide the information of a mounted file system.

`statvfs` prototype is as follows.

```c
int statvfs(const char *path, struct statvfs *vfs);
```

`fstatvfs` prototype is as follows.

```c
int fstatvfs(int fd, struct statvfs *vfs);
```

the `fstatvfs` is same as `statvfs` but operates on the file descriptor pointed to by the file.

The `statvfs` and `fstatvfs` system calls return the file system information of a given `path` into the `vfs` structure. The `vfs` must be a valid pointer. The path can be a file under a directory or a directory or a mount point itself.

Below is an example of `statvfs`. Download [here](https://github.com/DevNaga/gists/blob/master/statvfs.c)

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
    struct statvfs vfs_;
    int ret;

    if (argc != 2) {
        fprintf(stderr, "<%s> filename\n", argv[0]);
        return -1;
    }

    ret = statvfs(argv[1], &vfs_);
    if (ret != 0) {
        fprintf(stderr, "failed to statvfs [%s] error : %s\n",
                            argv[1], strerror(errno));
        return -1;
    }

    fprintf(stderr, "<statvfs> :\n");
    fprintf(stderr, "\t block size %lu\n", vfs_.f_bsize);
    fprintf(stderr, "\t fragment size %lu\n", vfs_.f_frsize);
    fprintf(stderr, "\t blocks %lu\n", vfs_.f_blocks);
    fprintf(stderr, "\t number of free blocks %lu\n", vfs_.f_bfree);
    fprintf(stderr, "\t number of free blocks for unprivileged user %lu\n", vfs_.f_bavail);
    fprintf(stderr, "\t number of inodes %lu\n", vfs_.f_files);
    fprintf(stderr, "\t number of free inodes %lu\n", vfs_.f_ffree);
    fprintf(stderr, "\t number of free inodes for unprivileged user %lu\n", vfs_.f_favail);
    fprintf(stderr, "\t file system id %lu\n", vfs_.f_fsid);
    fprintf(stderr, "\t mount flags %lx\n", vfs_.f_flag);
    fprintf(stderr, "\t filename max len %lu\n", vfs_.f_namemax);

    return 0;
}

```

```bash
./a.out /home
<statvfs> :
	 block size 4096
	 fragment size 4096
	 blocks 231975294
	 number of free blocks 130668897
	 number of free blocks for unprivileged user 118879457
	 number of inodes 58933248
	 number of free inodes 53036742
	 number of free inodes for unprivileged user 53036742
	 file system id 7389426947721209490
	 mount flags 1000
	 filename max len 255

```

Below is an example of `fstatvfs`. Download [here](https://github.com/DevNaga/gists/blob/master/fstatvfs.c)

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
    struct statvfs vfs_;
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

    ret = fstatvfs(fd, &vfs_);
    if (ret != 0) {
        fprintf(stderr, "failed to fstatvfs [%s] error : %s\n",
                            argv[1], strerror(errno));
        return -1;
    }

    fprintf(stderr, "<fstatvfs> :\n");
    fprintf(stderr, "\t block size %lu\n", vfs_.f_bsize);
    fprintf(stderr, "\t fragment size %lu\n", vfs_.f_frsize);
    fprintf(stderr, "\t blocks %lu\n", vfs_.f_blocks);
    fprintf(stderr, "\t number of free blocks %lu\n", vfs_.f_bfree);
    fprintf(stderr, "\t number of free blocks for unprivileged user %lu\n", vfs_.f_bavail);
    fprintf(stderr, "\t number of inodes %lu\n", vfs_.f_files);
    fprintf(stderr, "\t number of free inodes %lu\n", vfs_.f_ffree);
    fprintf(stderr, "\t number of free inodes for unprivileged user %lu\n", vfs_.f_favail);
    fprintf(stderr, "\t file system id %lu\n", vfs_.f_fsid);
    fprintf(stderr, "\t mount flags %lx\n", vfs_.f_flag);
    fprintf(stderr, "\t filename max len %lu\n", vfs_.f_namemax);

    close(fd);

    return 0;
}

```

the file descriptor however, can only be given a file but not a directory as given in `statvfs`. However, the information from `fstatvfs` is same as that of `statvfs`.

`statvfs` can be used to find out the disk usage. Below is an example, Download [here](https://github.com/DevNaga/gists/blob/master/statvfs_disk.c)


```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv)
{
    struct statvfs vfs_;
    int ret;

    if (argc != 2) {
        fprintf(stderr, "<%s> filename\n", argv[0]);
        return -1;
    }

    ret = statvfs(argv[1], &vfs_);
    if (ret != 0) {
        fprintf(stderr, "failed to statvfs [%s] error : %s\n",
                            argv[1], strerror(errno));
        return -1;
    }

    double total_disk = (vfs_.f_blocks * vfs_.f_frsize * 1.0) / (1024 * 1024 * 1024);
    double available = (vfs_.f_bfree * vfs_.f_frsize * 1.0) / (1024 * 1024 * 1024);

    fprintf(stderr, "total %f GB available %f GB used %f GB\n", total_disk, available, total_disk - available);
    fprintf(stderr, "used disk size [%s] %f\n", argv[1], ((total_disk - available) / total_disk) * 100);
    fprintf(stderr, "free disk size [%s] %f\n", argv[1], ((available) / total_disk) * 100);
    return 0;
}

```

The `total_disk` is the number of blocks and the fragment size of the block and divide it with GB unit. The `available` or the `free_disk` is the free blocks multiplied by the size of the fragments and divide with GB unit to get the units in terms of GigaBytes.

The used is the `total - free` disk space. The usage `%` is calculate by dividing the result with `total` and multiply by 100.

