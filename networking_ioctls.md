# network ioctls

Linux operating system supports a wide variety of network ioctls. There are many applications that use these ioctls to perform some very useful operations. One such command is `ifconfig`.

A sample example of an `ifconfig` command looks as below

```bash
enp0s25: flags=4099<UP,BROADCAST,MULTICAST>  mtu 1500
        ether 68:f7:28:9a:b9:6d  txqueuelen 1000  (Ethernet)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 0  bytes 0 (0.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
        device interrupt 20  memory 0xf0600000-f0620000
```

The `ifconfig` command also performs a series of ioctls and other operations to get this
data.

Let us have a look at the `flags` parameter. The `flags` talk about the state of the network device and the network parameters.

To get the `flags` one must to `SIOCGIFFLAGS` ioctl.

**The struct ifreq**: This is the base structure that we give out to the kernel for either get \/ set of network parameters.
This is also what we are going to use right now.

The below is the definition of the `struct ifreq`.

```c
struct ifreq {
    char ifr_name[IFNAMSIZ]; /* Interface name */
    union {
        struct sockaddr ifr_addr;
        struct sockaddr ifr_dstaddr;
        struct sockaddr ifr_broadaddr;
        struct sockaddr ifr_netmask;
        struct sockaddr ifr_hwaddr;
        short           ifr_flags;
        int             ifr_ifindex;
        int             ifr_metric;
        int             ifr_mtu;
        struct ifmap    ifr_map;
        char            ifr_slave[IFNAMSIZ];
        char            ifr_newname[IFNAMSIZ];
        char           *ifr_data;
    };
};

```

below is the example to get the interface flags.

```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>

int main(int argc, char **argv)
{
    int sock;
    struct ifreq ifr;

    if (argc != 2) {
        fprintf(stderr, "%s <ifname>\n", argv[0]);
        return -1;
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        return -1;

    memset(&ifr, 0, sizeof(struct ifreq));
    memcpy(ifr.ifr_name, argv[1], IFNAMSIZ - 1);

    if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
        fprintf(stderr, "failed to get interface flags for %s\n", argv[1]);
        return -1;
    }

    fprintf(stderr, "interface flags %x\n", ifr.ifr_flags);

    fprintf(stderr, "ifflags details: \n");
    if (ifr.ifr_flags & IFF_UP) {
        fprintf(stderr, "\t Up\n");
    }

    if (ifr.ifr_flags & IFF_BROADCAST) {
        fprintf(stderr, "\t Broadcast\n");
    }

    if (ifr.ifr_flags & IFF_MULTICAST) {
        fprintf(stderr, "\t Multicast\n");
    }

    close(sock);

    return 0;
}

```

The above program opens up a DGRAM socket and performs an ioctl to the kernel with the `SIOCGIFFLAGS`
command. This will then be unpacked in the kernel and passed to the corresponding subsystem i.e. the
networking subsystem. The networking subsystem will then validate the fields and the buffers and fills
the data into the buffer i.e `struct ifreq`. Thus the ioctl returns at the userspace with the data.

We then use the `struct ifreq` to unpack and find out the interface flags data. The interface flags
data is stored in the `ifr_flags` field in the `struct ifreq`.

A sample output of the above programs shows us the following:

```bash
[devnaga@localhost linux]$ ./a.out enp0s25
interface flags 1003
ifflags details: 
         Up
         Broadcast
         Multicast
```

Now we have gotten the method to get the interface flags, let's move to `MTU` field in the `ifconfig` output.

MTU is the largest size of the packet or frame specified in octets. For ethernet, it is mostly 1500 octets.

The below program shows a method to get the MTU of the device.

```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>

int main(int argc, char **argv)
{
    int sock;
    struct ifreq ifr;

    if (argc != 2) {
        fprintf(stderr, "%s <ifname>\n", argv[0]);
        return -1;
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        return -1;

    memset(&ifr, 0, sizeof(struct ifreq));
    memcpy(ifr.ifr_name, argv[1], IFNAMSIZ - 1);

    if (ioctl(sock, SIOCGIFMTU, &ifr) < 0) {
        fprintf(stderr, "failed to get MTU for %s\n", argv[1]);
        return -1;
    }

    fprintf(stderr, "MTU of the device is %d\n", ifr.ifr_mtu);

    close(sock);

    return 0;
}

```

In the above program we used `SIOCGIFMTU` ioctl flag similar to the `SIOCGIFFLAGS`. We used the same DGRAM
socket and the ioctl returns the data into the `ifr_mtu` field of the `struct ifreq`.

A sample output is shown below:

```bash
[devnaga@localhost linux]$ ./a.out enp0s25
MTU of the device is 1500

```

Now let us look at the mac address field of the `ifconfig` output. The mac address is the unique 6 byte address that represents
a device in the Layer2. To get this field we should perform `SIOCGIFHWADDR` ioctl.

The below example shows the usage of the `SIOCGIFHWADDR` ioctl.

```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>

int main(int argc, char **argv)
{
    int sock;
    struct ifreq ifr;

    if (argc != 2) {
        fprintf(stderr, "%s <ifname>\n", argv[0]);
        return -1;
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        return -1;

    memset(&ifr, 0, sizeof(struct ifreq));
    memcpy(ifr.ifr_name, argv[1], IFNAMSIZ - 1);

    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
        fprintf(stderr, "failed to get HWAddress for %s\n", argv[1]);
        return -1;
    }

    uint8_t *hwaddr = ifr.ifr_hwaddr.sa_data;

    fprintf(stderr, "HWAddress: %02x:%02x:%02x:%02x:%02x:%02x\n",
                        hwaddr[0],
                        hwaddr[1],
                        hwaddr[2],
                        hwaddr[3],
                        hwaddr[4],
                        hwaddr[5]);

    close(sock);

    return 0;
}

```

The above program does the similar top level jobs such as opening a socket, closing a socket and using the `struct ifreq`.
The ifr\_hwaddr contains the hardware address of the network device. The `ifr_hwaddr` is of type `struct sockaddr`.

The `struct sockaddr` contains its data member `sa_data` which inturn is the hardware address that the kernel copied.

The sample output of the program is shown below:

```bash
[devnaga@localhost linux]$ ./a.out enp0s25
HWAddress: 68:f7:28:9a:b9:6d

```

The set of hardware address is also possible via the `SIOCSIFHWADDR` ioctl. Here is the example

```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <net/if.h>

int main(int argc, char **argv)
{
    int sock;
    int ret;
    struct ifreq ifr;

    if (argc != 3) {
        fprintf(stderr, "%s <ifname> <HWAddress>\n", argv[0]);
        return -1;
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        return -1;

    memset(&ifr, 0, sizeof(struct ifreq));

    int hwaddr_i[6];

    ret = sscanf(argv[2], "%02x:%02x:%02x:%02x:%02x:%02x",
                            &hwaddr_i[0],
                            &hwaddr_i[1],
                            &hwaddr_i[2],
                            &hwaddr_i[3],
                            &hwaddr_i[4],
                            &hwaddr_i[5]);

    strcpy(ifr.ifr_name, argv[1]);

    ifr.ifr_hwaddr.sa_data[0] = hwaddr_i[0];
    ifr.ifr_hwaddr.sa_data[1] = hwaddr_i[1];
    ifr.ifr_hwaddr.sa_data[2] = hwaddr_i[2];
    ifr.ifr_hwaddr.sa_data[3] = hwaddr_i[3];
    ifr.ifr_hwaddr.sa_data[4] = hwaddr_i[4];
    ifr.ifr_hwaddr.sa_data[5] = hwaddr_i[5];

    ifr.ifr_addr.sa_family = AF_INET;
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;

    ret = ioctl(sock, SIOCSIFHWADDR, &ifr);
    if (ret < 0) {
        fprintf(stderr, "failed to set HWAddress for %s\n", argv[1]);
        perror("ioctl");
        return -1;
    }

    close(sock);

    return 0;
}

```

The above program reads the interface name and the mac address from the command line. It then copies into the
`struct ifreq`. The interface name is copied to the `ifr_name` flag, the family type is put into `ifr.ifr_addr.sa_family`
and is `AF_INET`. We also need to set the family in the `ifr.ifr_hwaddr.sa_family` member as the same `AF_INET`. The mac is
then copied into `ifr.ifr_hwaddr.sa_data` member.

The sample command to set the mac is below.

```bash
[root@localhost devnaga]# ./a.out enp0s25 00:ff:31:ed:ff:e1

[root@localhost devnaga]# ifconfig enp0s25
enp0s25: flags=4099<UP,BROADCAST,MULTICAST>  mtu 1500
        ether 00:ff:31:ed:ff:e1  txqueuelen 1000  (Ethernet)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 0  bytes 0 (0.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
        device interrupt 20  memory 0xf0600000-f0620000  

```

There is also a way to change the interface name. This is done using the `SIOCSIFNAME` ioctl.
Below is the code that demonstrates.

```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <net/if.h>

int main(int argc, char **argv)
{
    int sock;
    int ret;
    struct ifreq ifr;

    if (argc != 3) {
        fprintf(stderr, "%s <ifname> <new-ifname>\n", argv[0]);
        return -1;
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        return -1;

    memset(&ifr, 0, sizeof(struct ifreq));

    strcpy(ifr.ifr_name, argv[1]);

    ret = ioctl(sock, SIOCGIFFLAGS, &ifr);
    if (ret < 0) {
        fprintf(stderr, "failed to get interface flags for %s\n", argv[1]);
        return -1;
    }

    ifr.ifr_flags &= ~IFF_UP;

    ret = ioctl(sock, SIOCSIFFLAGS, &ifr);
    if (ret < 0) {
        fprintf(stderr, "failed to set interface flags for %s\n", argv[1]);
        return -1;
    }

    strcpy(ifr.ifr_newname, argv[2]);

    ret = ioctl(sock, SIOCSIFNAME, &ifr);
    if (ret < 0) {
        fprintf(stderr, "failed to set interface name for %s\n", argv[1]);
        perror("ioctl");
        return -1;
    }

    strcpy(ifr.ifr_name, argv[2]);

    ifr.ifr_flags |= IFF_UP;

    ret = ioctl(sock, SIOCSIFFLAGS, &ifr);
    if (ret < 0) {
        fprintf(stderr, "failed to set interface flags for %s\n", argv[1]);
        return -1;
    }

    close(sock);

    return 0;
}

```

The programs makes the interface go down, otherwise we cannot change the name of the interface.
The interface is made down using the `SIOCSIFFLAGS` ioctl and sets up the interface name and
makes the interface up again using the `SIOCSIFFLAGS`.

# wireless ioctls

The package [wireless-tools](http://www.labs.hpe.com/personal/Jean_Tourrilhes/Linux/Tools.html#latest) provides the needed API to perform many wireless functions.

