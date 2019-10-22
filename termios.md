## Terminal I/O

## tcgetattr

### cfgetispeed

Gets the speed of the serial device. Lets open the `/dev/tty` for this.

```c
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main()
{
    struct termios term;
    int ret;
    int fd;
    speed_t speed;

    fd = open("/dev/tty", O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    ret = tcgetattr(fd, &term);
    if (ret < 0) {
        perror("tcgetattr");
        return -1;
    }

    speed = cfgetispeed(&term);
    switch (speed) {
        case B0:
            printf("Speed 0\n");
        break;
        case B50:
            printf("Speed 50\n");
        break;
        case B75:
            printf("Speed 75\n");
        break;
        case B110:
            printf("Speed 110\n");
        break;
        case B134:
            printf("speed 134\n");
        break;
        case B150:
            printf("Speed 150\n");
        break;
        case B200:
            printf("Speed 200\n");
        break;
        case B300:
            printf("Speed 300\n");
        break;
        case B600:
            printf("Speed 600\n");
        break;
        case B1200:
            printf("Speed 1200\n");
        break;
        case B1800:
            printf("Speed 1800\n");
        break;
        case B2400:
            printf("Speed 2400\n");
        break;
        case B4800:
            printf("Speed 4800\n");
        break;
        case B9600:
            printf("Speed 9600\n");
        break;
        case B19200:
            printf("Speed 19200\n");
        break;
        case B38400:
            printf("Speed 38400\n");
        break;
        case B57600:
            printf("Speed 57600\n");
        break;
        case B115200:
            printf("Speed 115200\n");
        break;
        case B230400:
            printf("Speed 230400\n");
        break;
    }

    close(fd);
    return 0;
}

```

## tcsetattr

