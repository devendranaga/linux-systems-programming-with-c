# AF_ALG

`AF_ALG` is an interface provided by the kernel to perform the crypto operations.

The base example is provided in the LKML list [here](https://lwn.net/Articles/410833/).

Here is the slightly modified program from the same list. This program does the sha1.

You can download this program [here](https://github.com/DevNaga/gists/blob/master/crypto_sha1.c).

```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_alg.h>

int main(int argc, char **argv)
{
	int cli_fd;
	int ser_fd;

	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "hash",
		.salg_name = "sha1",
	};
	int i;
	char buf[1000];

	if (argc != 2) {
		fprintf(stderr, "%s [input]\n", argv[0]);
		return -1;
	}

	ser_fd = socket(AF_ALG, SOCK_SEQPACKET, 0);
	if (ser_fd < 0)
		return -1;

    int ret;

	ret = bind(ser_fd, (struct sockaddr *)&sa, sizeof(sa));
	if (ret < 0)
		return -1;

	cli_fd = accept(ser_fd, NULL, NULL);
	if (cli_fd < 0)
		return -1;

	write(cli_fd, argv[1], strlen(argv[1]));
	ret = read(cli_fd, buf, sizeof(buf));
	if (ret < 0)
		return -1;

	for (i = 0; i < ret; i ++) {
		printf("%02x", buf[i] & 0xff);
	}
	printf("\n");

	close(cli_fd);
	close(ser_fd);

	return 0;
}
```

Here is another that does the md5. You can download the program [here](https://github.com/DevNaga/gists/blob/master/crypto_md5.c).

```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_alg.h>

int main(int argc, char **argv)
{
	int cli_fd;
	int ser_fd;

	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "hash",
		.salg_name = "md5",
	};
	int i;
	char buf[1000];

	if (argc != 2) {
		fprintf(stderr, "%s [input]\n", argv[0]);
		return -1;
	}

	ser_fd = socket(AF_ALG, SOCK_SEQPACKET, 0);
	if (ser_fd < 0)
		return -1;

    int ret;

	ret = bind(ser_fd, (struct sockaddr *)&sa, sizeof(sa));
	if (ret < 0)
		return -1;

	cli_fd = accept(ser_fd, NULL, NULL);
	if (cli_fd < 0)
		return -1;

	write(cli_fd, argv[1], strlen(argv[1]));
	ret = read(cli_fd, buf, sizeof(buf));
	if (ret < 0)
		return -1;

	for (i = 0; i < ret; i ++) {
		printf("%02x", buf[i] & 0xff);
	}
	printf("\n");

	close(cli_fd);
	close(ser_fd);

	return 0;
}
```

Another sample program to describe the available hash functions is here.

You can also download the program in [here](https://github.com/DevNaga/gists/blob/master/crypto_hashes.c)

```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_alg.h>

int main(int argc, char **argv)
{
	int cli_fd;
	int ser_fd;

	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "hash",
		.salg_name = "sha1",
	};
	int i;
	char buf[1000];

	if (argc != 3) {
		fprintf(stderr, "%s [hash_function] [input]\n"
						"Where hash_function is one of the below\n"
						"\t 1. crct10dif\n"
						"\t 2. sha224\n"
						"\t 3. sha256\n"
						"\t 4. sha1\n"
						"\t 5. md5\n"
						"\t 6. md4\n",
						argv[0]);
		return -1;
	}

    strcpy(sa.salg_name, argv[1]);

	ser_fd = socket(AF_ALG, SOCK_SEQPACKET, 0);
	if (ser_fd < 0)
		return -1;

    int ret;

	ret = bind(ser_fd, (struct sockaddr *)&sa, sizeof(sa));
	if (ret < 0)
		return -1;

	cli_fd = accept(ser_fd, NULL, NULL);
	if (cli_fd < 0)
		return -1;

	write(cli_fd, argv[2], strlen(argv[2]));
	ret = read(cli_fd, buf, sizeof(buf));
	if (ret < 0)
		return -1;

	for (i = 0; i < ret; i ++) {
		printf("%02x", buf[i] & 0xff);
	}
	printf("\n");

	close(cli_fd);
	close(ser_fd);

	return 0;
}
```

# scatter gather i/o

Example of the `writev` is as below. Download it from [here](https://github.com/DevNaga/gists/blob/master/sgio.c)

```c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>

int main(int argc, char **argv)
{
	int fd;

    if (argc < 2) {
		fprintf(stderr, "%s [filename] [content]\n", argv[0]);
		return -1;
	}

	fd = open(argv[1], O_CREAT | O_RDWR, S_IRWXU);
	if (fd < 0) {
		fprintf(stderr, "failed to open %s\n", argv[1]);
		return -1;
	}

	int i;
	int count = 0;
	struct iovec iov[argc - 1];

	for (i = 2; i < argc; i ++) {
		iov[i - 2].iov_base = argv[i];
		iov[i - 2].iov_len = strlen(argv[i]);
		count ++;
	}

    char newline_str[] =  "\n";

    iov[count].iov_base = newline_str;
	iov[count].iov_len = strlen(newline_str);
	count ++;

	writev(fd, iov, count);

	close(fd);
}
```

# sendmsg / recvmsg

# Raw sockets

The Raw sockets are the primary sources of evaluating and testing a system's networking stack. They are very useful to craft a packet and test the network stack ability of the device under test.

They are also useful to offload the networking stack to the user space to improve the performance or throughput.
