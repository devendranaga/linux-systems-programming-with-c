# syslogd


Syslogd is a daemon that logs every single event to a file stored in the standard directory. The event can come from different sources such as the kernel, user programs etc.

Syslogd exposes the `syslog` API to the userspace programs to interact with the syslogd daemon.

The `syslog` API packs the message into the format that is understandable by the syslogd daemon and sends it to the daemon over the unix socket. The daemon then unpacks and logs the message into the file.

The syslogs are also called system logs and are stored under `/var/log/` with a common name of either `syslog` or `messages`.

With the systemd in the latest linux operating systems, the functionality of syslogd has faded. However, the syslogd is still an important gem in the embedded environment.

With different operating systems providing the same syslog API behavior and support the syslog API may become generic and portable.

The `syslog` API prototype:

`void syslog(int priority, const char *format, ...);`


The priority is an OR combination of the facility and level.

the facility has the following values from the manual page (`man 3 syslog`). But here we only describe those that are most commonly used and easy to get on with in the coding.

| facility | description |
| -- | -- |
| LOG_AUTH | security / authorization messages |
| LOG_DAEMON | system daemon messages |
| LOG_KERN | kernel messages |
| LOG_USER | user level generated messages |

the level has the following values from the manual page (`man 3 syslog`) and here also only the most used ones.

| level | description |
| -- | -- |
| LOG_EMERG | emergency messages |
| LOG_ALERT | very important messages |
| LOG_CRIT | critical messages |
| LOG_ERR | error conditions |
| LOG_WARNING |  warning conditions |
| LOG_NOTICE | normal messages |
| LOG_INFO | informative messages |
| LOG_DEBUG | debugging messages |

however, the most common example of the `syslog` API is the following:

`syslog(LOG_ERR, "invalid length of data on the socket!\n");`

or another form can be that

`syslog(LOG_AUTH | LOG_ALERT, "unprivilied access from 192.168.1.1:111\n");`

The linux kernel prints all its messages to the ring buffer. The ring buffer can be accessed via the `/dev/kmsg`.

Some of syslog implementations read the `/dev/kmsg` and parse and then print them into the log file under `/var/log/`.

## /dev/kmsg

`/dev/kmsg` is a device file that stores the kernel logging information.
