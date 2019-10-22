
## /proc file system

Linux kernel exposes the runtime information of the kernel and each process into the `/proc` file system as a set of files. `/proc` file system contain various information about the current process and the kernel.

each process information is stored under `/proc/<process-id>` folder. where `process-id` is the id of the process shown in `ps` command or via the call to `getpid` syscall.

the file `/proc/<process-id>/status` contain various information about the process. For example, for a `login` process, here's the following information:

```bash
Name:   login
State:  S (sleeping)
Tgid:   23
Pid:    23
PPid:   4
TracerPid:      0
Uid:    0       0       0       0
Gid:    1000    1000    1000    1000
FDSize: 3
Groups:
VmPeak: 0 kB
VmSize: 14532 kB
VmLck:  0 kB
VmHWM:  0 kB
VmRSS:  1868 kB
VmData: 0 kB
VmStk:  0 kB
VmExe:  44 kB
VmLib:  0 kB
VmPTE:  0 kB
Threads:        1
SigQ:   0/0
SigPnd: 0000000000000000
ShdPnd: 0000000000000000
SigBlk: 0000000000000000
SigIgn: 0000000000000000
SigCgt: 0000000000000000
CapInh: 0000000000000000
CapPrm: 0000001fffffffff
CapEff: 0000001fffffffff
CapBnd: 0000001fffffffff
Cpus_allowed:   00000001
Cpus_allowed_list:      0
Mems_allowed:   1
Mems_allowed_list:      0
voluntary_ctxt_switches:        150
nonvoluntary_ctxt_switches:     545

```

Here's the brief description about the information contained in the `status` file.

1. The `Name` specify the name of the process.
2. The `state` represent the state of the process (Run, sleep etc).
3. `pid` is the process id and `ppid` is the parent process id.
4. `VmSize` is the sum of all mapped memory in `/proc/pid/maps`.
5. `VmRSS` is the attached physical memory reserved for this.



getting process name from the pid is achieved by using the `/proc/<process-id>/status` file. This file is present for each process.

Below is an example of getting the process name from the process id. Download [here](https://github.com/DevNaga/gists/blob/master/pname_from_pid.c)

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    char name[200];
    char buf[2048];
    char val[100];
    FILE *fp;
    int ret;

    if (argc != 2) {
        fprintf(stderr, "<%s> pid\n", argv[0]);
        return -1;
    }

    snprintf(name, sizeof(name), "/proc/%s/status", argv[1]);

    fp = fopen(name, "r");
    if (!fp) {
        return -1;
    }

    while (fgets(buf, sizeof(buf), fp)) {
        int i = 0;
        int j = 0;

        if (strstr(buf, "Name")) {
            i = strlen("Name:");

            while ((buf[i] != '\0') && (buf[i] == ' ')) {
                i ++;
            }

            while (buf[i] != '\n') {
                val[j] = buf[i];
                j ++;
                i ++;
            };

            val[j] = '\0';

        }
    }

    printf("name: %s\n", val);

    return 0;
}

```

The folder `/proc/<pid>/fd` contains the list of currently opened files by a specific process.

The below program lists the currently opened files by a process.

```c

/**
 * Find current files in use for a specific process
 *
 * @Author: Devendra Naga (devendra.aaru@gmail.com)
 *
 * License MIT
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

int get_open_files(pid_t pid)
{
    DIR *d;
    struct dirent *dirp;
    char dirpath[100];

    // get the full directory path for the /proc/<pid>/fd/
    snprintf(dirpath, sizeof(dirpath), "/proc/%d/fd", pid);

    // open /proc/pid/fd
    d = opendir(dirpath);
    if (!d) {
        return -1;
    }

    // read contents of the directory and skip directories . and ..
    while ((dirp = readdir(d)) != NULL) {
        if (!strcmp(dirp->d_name, ".") ||
                !strcmp(dirp->d_name, "..")) {
            continue;
        }

        char file[512];
        char realpath[512];
        ssize_t res;

        memset(file, 0, sizeof(file));
        memset(realpath, 0, sizeof(realpath));

        // get the real file name that's in /proc/pid/fd/
        snprintf(file, sizeof(file), "%s/%s", dirpath, dirp->d_name);

        // validate if its not a directory .. we are looking for only files
        struct stat file_type;
        int ret;
        ret = stat(file, &file_type);
        if ((ret != -1) && (!S_ISDIR(file_type.st_mode))) { // if not a directory.. then readlink on the directory
            res = readlink(file, realpath, sizeof(realpath));
            if (res != -1) { // here's the rael name of the file
                printf("file %s\n", realpath);
            }
        }
    }

    closedir(d);
    return 0;
}

int main()
{
    FILE *fp;

    // testing
    fp = fopen("./hello.txt", "w");

    get_open_files(getpid());
}


```

The program opens a text file called `hello.txt` and calls the `get_open_files` with its own PID to see if the file `hello.txt` is resolved.

The directory `/proc/<pid>/fd` is opened for reading and scanned for files (excluding the directories).

The files sometimes can be special files, like the tty serial file, socket, or a device file like character device or a regular file in this case.

`readlink` resolves it into absolute path for the file and returns in the `realpath` of the buffer.

The contents of the `realpath` are then dumped on the screen.



the file `/proc/meminfo` contains the information about the memory. The contents of the file looks like below,

```bash
MemTotal:        8312080 kB
MemFree:         2906856 kB
Buffers:           34032 kB
Cached:           188576 kB
SwapCached:            0 kB
Active:           167556 kB
Inactive:         157876 kB
Active(anon):     103104 kB
Inactive(anon):    17440 kB
Active(file):      64452 kB
Inactive(file):   140436 kB
Unevictable:           0 kB
Mlocked:               0 kB
SwapTotal:      25165824 kB
SwapFree:       25123188 kB
Dirty:                 0 kB
Writeback:             0 kB
AnonPages:        102824 kB
Mapped:            71404 kB
Shmem:             17720 kB
Slab:              13868 kB
SReclaimable:       6744 kB
SUnreclaim:         7124 kB
KernelStack:        2848 kB
PageTables:         2524 kB
NFS_Unstable:          0 kB
Bounce:                0 kB
WritebackTmp:          0 kB
CommitLimit:      515524 kB
Committed_AS:    3450064 kB
VmallocTotal:     122880 kB
VmallocUsed:       21296 kB
VmallocChunk:      66044 kB
HardwareCorrupted:     0 kB
AnonHugePages:      2048 kB
HugePages_Total:       0
HugePages_Free:        0
HugePages_Rsvd:        0
HugePages_Surp:        0
Hugepagesize:       2048 kB
DirectMap4k:       12280 kB
DirectMap4M:      897024 kB

```

where `MemTotal` is the total memory thats available, `MemFree` is the available free memory.

Below program parses the above file and displays the total, free and used. Download [here](https://github.com/DevNaga/gists/blob/master/meminfo.c)

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE *fp;
    const char *meminfo = "/proc/meminfo";
    int ret;
    int k;
    int i;
    char buf[1024];
    char *ptr;
    char memtotal[40];
    char memfree[40];
    int memtotal_int;
    int memfree_int;
    int memused;

    fp = fopen(meminfo, "r");
    if (!fp) {
        return -1;
    }

    memset(memtotal, 0, sizeof(memtotal));

    memtotal_int = 0;
    memfree_int = 0;

    while (fgets(buf, sizeof(buf), fp)) {
        buf[strlen(buf) - 1] = '\0';

        ptr = strstr(buf, "MemTotal:");
        if (ptr) {
            ptr += strlen("MemTotal:");
            i = 0;

            while (ptr[i] == ' ') {
                i ++;
            }

            k = 0;

            while (ptr[i] != ' ') {
                memtotal[k] = ptr[i];
                i ++;
                k ++;
            }

            memtotal[k] = '\0';

            memtotal_int = atoi(memtotal);
        }

        ptr = strstr(buf, "MemFree:");
        if (ptr) {
            ptr += strlen("MemFree:");
            i = 0;

            while (ptr[i] == ' ') {
                i ++;
            }

            k = 0;

            while (ptr[i] != ' ') {
                memfree[k] = ptr[i];
                i ++;
                k ++;
            }

            memfree[k] = '\0';

            memfree_int = atoi(memfree);
        }
    }

    memused = memtotal_int - memfree_int;

    printf("memtotal %f GB memfree %f GB memused %f GB\n", memtotal_int / (1024.0 * 1024), memfree_int / (1024.0 * 1024), memused / (1024.0 * 1024));

    fclose(fp);
}

```

An example output is shown below,

```bash

devnaga@DESKTOP-RTSK8VU:/home/devnaga/test$ ./a.out

memtotal 7.927017 GB memfree 2.774483 GB memused 5.152534 GB  

```


The system hostname is usually  shown under `/proc/sys/kernel/hostname`.

Below is an example that reads the hostname. Download [here](https://github.com/DevNaga/gists/blob/master/hostname.c)

```c
#include <stdio.h>

int main()
{
    FILE *fp;
    const char *hostname = "/proc/sys/kernel/hostname";

    fp = fopen(hostname, "r");
    if (!fp) {
        return -1;
    }


    char host[80];

    fgets(host, sizeof(host), fp);

    fclose(fp);

    printf("hostname %s", host);

    return 0;
}

```




the file `/proc/cpuinfo` contain the information about the cpu. For example, the current system information contain the following.

```bash
dev@Hanzo:~$ cat /proc/cpuinfo
processor       : 0
vendor_id       : GenuineIntel
cpu family      : 6
model           : 78
model name      : Intel(R) Core(TM) i5-6300U CPU @ 2.40GHz
stepping        : 3
cpu MHz         : 2496.000
cache size      : 3072 KB
physical id     : 0
siblings        : 1
core id         : 0
cpu cores       : 1
apicid          : 0
initial apicid  : 0
fpu             : yes
fpu_exception   : yes
cpuid level     : 22
wp              : yes
flags           : fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 syscall nx rdtscp lm constant_tsc rep_good nopl xtopology nonstop_tsc cpuid pni pclmulqdq monitor ssse3 cx16 pcid sse4_1 sse4_2 x2apic movbe popcnt aes xsave avx rdrand hypervisor lahf_lm abm 3dnowprefetch invpcid_single pti retpoline rsb_ctxsw fsgsbase avx2 invpcid rdseed clflushopt
bugs            : cpu_meltdown spectre_v1 spectre_v2 spec_store_bypass
bogomips        : 4992.00
clflush size    : 64
cache_alignment : 64
address sizes   : 39 bits physical, 48 bits virtual
power management:

```
