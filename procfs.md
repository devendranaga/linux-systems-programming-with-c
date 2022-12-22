
## /proc file system

Linux kernel exposes the runtime information of the kernel and each process into the `/proc` file system as a set of files. `/proc` file system contain various information about the current processes and the kernel.

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

    fclose(fp);
    
    return 0;
}

```

The folder `/proc/<pid>/fd` contains the list of currently opened files by a specific process.

A sample listing on the `/proc/<pid>/fd` shows the following list.

```bash

0  1  2  3  4

```

The actual files referenced by the above can be listed with the `ls -l` command. In general, `readlink` can be used to read those file descriptor names.

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


### meminfo

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

### Hostname

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




### CPUINFO

The file `/proc/cpuinfo` contain the information about the cpu. For example, the current system information contain the following.

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

### modules

The file `/proc/modules` contains the list of kernel modules.

Below is the description of each of the columns.

Column 1 : describe the name of the module
Column 2 : describe the memory size of the module in bytes
Column 3 : describe how many instances of the module are currently loaded. 0 represents an unloaded module
Column 4 : describe if the module depends on the other module to be present in order to work.
Column 5 : describe one of the three states, `Live`, `Loading`, `Unloading`.
Column 6 : describe the current kernel memory offset of the module, generally used by debugging programs

### interrupts

The file `/proc/interrupts` contain the stats of kernel interrupts. An example is shown as below.

```bash

a@linux:~$ cat /proc/interrupts
            CPU0       CPU1       CPU2       CPU3
   0:          7          0          0          0  IR-IO-APIC    2-edge      timer
   8:          0          0          0          0  IR-IO-APIC    8-edge      rtc0
   9:          0       3950          0          0  IR-IO-APIC    9-fasteoi   acpi
  14:          0          0          0          0  IR-IO-APIC   14-fasteoi   INT344B:00
  16:          0          0    1988837          0  IR-IO-APIC   16-fasteoi   i2c_designware.0, idma64.0
  17:          0          0          0          0  IR-IO-APIC   17-fasteoi   i2c_designware.1, idma64.1
  18:          0          0          0          0  IR-IO-APIC   18-fasteoi   i2c_designware.2, idma64.2
  19:          0          0          0          0  IR-IO-APIC   19-fasteoi   i2c_designware.3, idma64.3
  86:          0          0          0      33568  IR-IO-APIC   86-fasteoi   MSHW0030:00
 120:          0          0          0          0  DMAR-MSI    0-edge      dmar0
 121:          0          0          0          0  DMAR-MSI    1-edge      dmar1
 122:          0          0          0          0  IR-PCI-MSI 458752-edge      PCIe PME, pciehp
 123:          0          0          0          0  IR-PCI-MSI 475136-edge      PCIe PME
 124:          0          0          0          0  IR-PCI-MSI 481280-edge      PCIe PME
 125:    3232012     273240      68821          0  IR-PCI-MSI 327680-edge      xhci_hcd
 126:         12          0          4          0  IR-PCI-MSI 1048576-edge      nvme0q0
 127:       1328     166131      75027     541487  IR-PCI-MSI 32768-edge      i915
 128:     132986          0          0          0  IR-PCI-MSI 1048577-edge      nvme0q1
 129:          0      95715          0          0  IR-PCI-MSI 1048578-edge      nvme0q2
 130:          0          0     107829          0  IR-PCI-MSI 1048579-edge      nvme0q3
 131:          0          0          0     100184  IR-PCI-MSI 1048580-edge      nvme0q4
 132:         38          0          0          0  IR-PCI-MSI 360448-edge      mei_me
 135:          0          0          0          0  als-dev1       als_consumer1
 139:          0          0          0          0  accel_3d-dev3       accel_3d_consumer3
 141:          0      44161          0          0  IR-PCI-MSI 514048-edge      snd_hda_intel:card0
 NMI:        186        180        181        179   Non-maskable interrupts
 LOC:    1401531    1396165    1407250    1412530   Local timer interrupts
 SPU:          0          0          0          0   Spurious interrupts
 PMI:        186        180        181        179   Performance monitoring interrupts
 IWI:        152      31837       9771      44339   IRQ work interrupts
 RTR:          0          0          0          0   APIC ICR read retries
 RES:    1155014    1228878    1157503    1164861   Rescheduling interrupts
 CAL:    2028593    2040548    2011511    1850212   Function call interrupts
 TLB:    4055975    4075325    4098490    4030782   TLB shootdowns
 TRM:          0          0          0          0   Thermal event interrupts
 THR:          0          0          0          0   Threshold APIC interrupts
 DFR:          0          0          0          0   Deferred Error APIC interrupts
 MCE:          0          0          0          0   Machine check exceptions
 MCP:         15         16         16         16   Machine check polls
 HYP:          0          0          0          0   Hypervisor callback interrupts
 HRE:          0          0          0          0   Hyper-V reenlightenment interrupts
 HVS:          0          0          0          0   Hyper-V stimer0 interrupts
 ERR:          0
 MIS:          0
 PIN:          0          0          0          0   Posted-interrupt notification event
 NPI:          0          0          0          0   Nested posted-interrupt event
 PIW:          0          0          0          0   Posted-interrupt wakeup event

```


Column 1: describe the irq number
Column 2-5: describe the number of interrupts per CPU (my system has 4 cpus)
Column 6: Type of the interrupt
Column 7: The way interrupt is being triggered (i guess, either level triggered or edge triggered etc)
Column 8: the device driver

### /proc/stat

The file `/proc/stat` represents the stats about the system. An example of it is shown below.

```bash

linux:~$ cat /proc/stat
cpu  783230 122383 189177 1322985 9902 0 28016 0 0 0
cpu0 198197 29752 47418 325399 2596 0 12620 0 0 0
cpu1 194183 31165 48246 331448 2493 0 8150 0 0 0
cpu2 195223 31839 46619 331334 2386 0 3804 0 0 0
cpu3 195625 29626 46893 334801 2426 0 3440 0 0 0
intr 29286570 7 0 0 0 0 0 0 0 0 4971 0 0 0 0 0 0 2501506 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 42423 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3855139 16 1009039 135787 97393 110145 102032 38 0 0 0 0 0 0 0 0 119751 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
ctxt 72587926
btime 1585371666
processes 15991
procs_running 3
procs_blocked 0
softirq 18887110 592022 4788213 1230 2476370 2575 0 3562475 4456054 38895 2969276

```



### /proc/<pid>/cmdline

The file `/proc/<pid>/cmdline` shows the current program's command line when being executed. Replace pid with the id from `ps`.

For example, output of `cat /proc/17355/cmdline` shows as follows.

```bash

linux:~$ cat /proc/17355/cmdline
vimmanuscript/procfs.md

```

### /proc/net/dev

The file `/proc/net/dev` contain the stats regarding the networking devices in the system.

The command `ifconfig` command lists various networking interfaces. Like below.


```bash

enxd03745447deb: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.0.12 netmask 255.255.255.0  broadcast 192.168.0.255
        inet6 fe80::d237:45ff:fe44:7deb  prefixlen 64  scopeid 0x20<link>
        ether d0:37:45:44:7d:eb  txqueuelen 1000  (Ethernet)
        RX packets 3495452  bytes 4574723893 (4.5 GB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 1816643  bytes 184173070 (184.1 MB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
        inet6 ::1  prefixlen 128  scopeid 0x10<host>
        loop  txqueuelen 1000  (Local Loopback)
        RX packets 408000  bytes 801938605 (801.9 MB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 408000  bytes 801938605 (801.9 MB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

lxcbr0: flags=4099<UP,BROADCAST,MULTICAST>  mtu 1500
        inet 10.0.3.1  netmask 255.255.255.0  broadcast 0.0.0.0
        ether 00:16:3e:00:00:00  txqueuelen 1000  (Ethernet)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 0  bytes 0 (0.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

```

### /proc/net/arp

The file `/proc/net/arp` contain the stats regarding the arp table in the system.

The command `arp` command lists the learnt mac addresses by the device. Like below.


```bash
IP address       HW type     Flags       HW address            Mask     Device
192.168.0.1      0x1         0x2         70:4f:57:60:46:55     *        wlp4s0
```

