# Appendix. E Programming problems

This appendix details about some of the programming problems for practise.

## processes / fork

## file operations / directory manip

## networking

1. displaying ARP entries

ARP entries are stored in `/proc/net/arp`.

Below is an example program demonstrates the use of the `/proc/net/arp` and dump contents on the console.

```cpp

#include <iostream>
#include <string>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

int get_value(char *in, int off, char *val)
{
    int id = 0;

    while ((in[off] != '\0') && (in[off] != ' ')) {
        val[id] = in[off];
        id ++;
        off ++;
    }
    val[id] = '\0';

    while ((in[off] != '\0') && (in[off] == ' ')) {
        off ++;
    }


    return off;
}

int main()
{
    char buf[1024];
    FILE *fp;

    fp = fopen("/proc/net/arp", "r");
    if (!fp) {
        return -1;
    }

    fgets(buf, sizeof(buf), fp);
    memset(buf, 0, sizeof(buf));
    while (fgets(buf, sizeof(buf), fp)) {
        char hw_type[20];
        char flags[20];
        char hwaddr[40];
        char mask[20];
        char device[20];
        char ipaddr[40];
        int id = 0;
        int off = 0;

        buf[strlen(buf) - 1] = '\0';

        memset(ipaddr, 0, sizeof(ipaddr));
        off = get_value(buf, off, ipaddr);
        off = get_value(buf, off, hw_type);
        off = get_value(buf, off, flags);
        off = get_value(buf, off, hwaddr);
        off = get_value(buf, off, mask);
        off = get_value(buf, off, device);

        printf("ipaddr : %s\n", ipaddr);
        printf("hwtype: %s\n", hw_type);
        printf("flags: %s\n", flags);
        printf("hwaddr: %s\n", hwaddr);
        printf("mask: %s\n", mask);
        printf("device: %s\n", device);
    }
    fclose(fp);
}

```

## pthreads

## message queues
