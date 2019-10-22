## Netlink sockets

Netlink protocol is `AF_NETLINK`. The type of socket can either be `SOCK_RAW` or `SOCK_DGRAM`.

example of the socket call:

```c
sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
```

Where `AF_NETLINK` is the protocol, `SOCK_RAW` is the socket type and `NETLINK_ROUTE` is the netlink family.

The following are the netlink families that are supported by NETLINK.

| family | description|
|---|---|
|NETLINK_ROUTE| Receives routing subsystem updates on the routing table, ip address, link parameters, neighbour setup, qdisc, tc and packet classifiers etc..|
|NETLINK_FIREWALL| Receives packet sent by the ipv4 firewall |
|NETLINK_ARPD|Arp table management from the userspace|
|NETLINK_ROUTE6|Ipv6 routing table updates (send and recieve)|
|NETLINK_IP6_FW|Receive packets that failed the firewall checks|
 

Overview of currently active netlink sockets in the system

```bash
ss -f netlink
```