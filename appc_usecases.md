# Appendix . C USE CASES and Frameworks

This appendix is focused on the realworld middleware software and frameworks that come in handy at the job or doing any software development. As the current world is getting more connected and communicated, In most realworld applications, at some point it is mandatory to have the following requirements.

1. communication means - network knowledge
2. positioning / GPS - self identification / IOT
3. wireless packet analysis - for cybersecurity / testing / validation
4. serialisation - for communication between machines
5. security defensive practises and mechanisms employed with in the linux kernel and exploting them in user programs

Before we go into the details, consider some of the most important points in software development :

1. Always check your warnings
2. always fix warnings and errors (enable `-Wall -Wextra -Werror`)
3. in a debug environment, always have `-g and -ggdbg` and remove `-s` and `-O`. Check for **Appendix. D GCC flags** for more information
4. Always build test and run the software atleast once on a hardware before you commit changes in a multi-personal project

## Process Manager

## seccomp


Below is one example of `seccomp`. Download [here](https://github.com/DevNaga/gists/blob/master/prctl.c)

**Example:**

```c
#include <stdio.h>
#include <sys/prctl.h>
#include <linux/seccomp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT);

    int fd;

    fd = open("./test", O_RDWR | O_CREAT, S_IRWXU);
    if (fd < 0) {
        return -1;
    }

    printf("opened file although restricted in seccomp\n");

    return 0;
}

```

The `seccomp` system call is a superset of the `prctl` method and has more detailed interface via the `BPF` (Berkeley Packet Filter)

A more detailed and easier implementation of such is the userspace seccomp library.

install it by running the following command:

```shell
sudo apt install libseccomp-dev
```

Below is one example of the `seccomp` API. Download [here](https://github.com/DevNaga/gists/blob/master/seccomp_init.c)

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <seccomp.h>

int main()
{
    scmp_filter_ctx ctx;

    ctx = seccomp_init(SCMP_ACT_KILL);

    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);

    seccomp_load(ctx);

    int fd;

    fd = open("./test", O_RDWR | O_CREAT, S_IRWXU);
    if (fd < 0) {
        printf("failed to create file \n");
        return -1;
    }

    printf("file is created\n");

    return 0;
}

```

## socket library

We are going to write a socket library as an exercise to what we have learnt in the socket programming.

The socket library has the following API. Header file is defined [here](https://github.com/DevNaga/gists/blob/master/socketlib.h)


```c
// create new tcp socket
int new_tcp_socket();

// create new udp socket
int new_udp_socket();

// create new unix tcp socket
int new_tcp_unix_socket();

// create new unix udp socket
int new_udp_unix_socket();

// delete socket
int destroy_socket(int fd);

// create server based on `ip` and `port` and listen for `n_conn`
int new_tcp_server(const char *ip, int port, int n_conns);

// accept conn on the socket, and send out dest_addr and port back
int accept_conn(int server_fd, int family, char *dest_addr, uint32_t *dest_port);

// create new udp server based off of `ip` and `port`
int new_udp_server(const char *ip, int port);

// create a new tcp unix server with given path
int new_tcp_unix_server(const char *path);

// create a new udp unix server with given path
int new_udp_unix_server(const char *path);

// create a new tcp unix client with given path
int new_tcp_unix_client(const char *path);

// create a new udp unix client with given path
int new_udp_unix_client(const char *path);

// send message on the fd
// dest_addr is valid for udp fd and unix fd
int send_message(int fd, int family, void *msg, int msglen, char *dest_addr, int dest_port);

// recieve message on the fd 
// dest is valid for udp fd and unix fd
int recv_message(int fd, int family, void *msg, int msglen, char *dest, uint32_t *dest_port);

// callback list
struct socket_loop_callbacks {
    int fd;
    void (*callback)(int fd, void *ptr);
    struct socket_loop_callbacks *next;
};

// privatepointer to socket_loop class
struct socket_loop {
    fd_set allfd;
    struct socket_loop_callbacks *callbacks;
};

// create a pointer to socket_loop and return
void *new_socket_loop();

// use the sock_loop returned and add fd to the fd_set
int add_fd_to_socket_loop(int fd, void *sock_loop, void *ptr, void (*callback)(int fd, void *ptr));

// remove fd from socket_loop
int del_fd_from_socket_loop(int fd, void *sock_loop, void *ptr, void (*callback)(int fd, void *ptr));

// run the socket loop and call callbacks 
int socket_loop_run(void *sock_loop);

// set multicast option on the socket for tx
int socket_set_multicast(int fd);

// subscribe for multicast data
int socket_add_to_mcast_group(int fd, char *mcast_ip);

// reuse socket address
int socket_reuse_addr(int fd);

```

## Event Library

In this chapter we are going to write an event library that allows us to do multiple jobs in a single process.

## GPSd

### Introduction

The [GPSd](http://www.catb.org/gpsd/) is a service that communicates with the GPS hardware and gets us the GPS fix and position. GPSd is an opensource software that is widely used in a lot of embedded systems including phones.

Many GPS devices provide the data over a serial connection. The GPS device reports the data over the serial interface once per second. The once per second can also be configured once per some millseconds for say 100 milliseconds. The configuration may involve setting the serial port baud rate and using special firmware commands asking the device to perform a speed change of the data.

The GPSd software then reads and parses the information coming over the serial connection and finds out if the satellites gives us a fix with current time, latitude, longitude, elevation, speed and heading. There are some more parameters that GPSd gives us such as Dilution of precision and noise statistics. However, most of the GPSes won't give us everything. The GPSes may provide the information to the userspace via their own protocol or via the NMEA protocol. NMEA protocol is a standard protocol implemented by almost all GPS receivers and understood by almost all software modules. The most prominent messages ni the NMEA protocol are GPGGA, GPGSA, GPGST, GPRMC and GPGSV. These are also called as NMEA strings. More about the NMEA is [here](http://aprs.gids.nl/nmea/)


Most of the GPSes lose the fix intermittently when the device comes across the trees and building complexes. At this point of time a special algorithm such as position estimation may be employed (in rare GPSes it is implemented) and gives out the estimated GPS value.

### GPSd interfacing

The GPSd is a service daemon that listens on the TCP socket for clients and opens a serial connection with (almost all) GPS devices.

The GPSd provides the data to the clients in a JSON formatted strings. The strings are then decoded at the client apps using the GPSd client library API.

API list:

The following are the most important API that are used to interface with the GPSd and get the GPS information

Here is the [link](http://www.catb.org/gpsd/libgps.html) to the GPSd API. And part of the API description is taken from the link.

The include file for the client library is `gps.h` and the linker flags are `lgps` and `lm`.

| API | description |
| --| -- |
| gps_open | create a socket to the GPS daemon |
| gps_read | read from the GPS daemon |
| gps_stream | stream the report to the user |
| gps_unpack | parse JSON from a specified buffer into a session structure|
| gps_close | close the socket connection with the GPS daemon |


The `gps_open` prototype is as follows.

```c
int gps_open(char *server, int port, struct gps_data_t *gps_data);
```

For most of the time, the `server` and `port` arguments are set to 0. The `gps_data` structure is initialised and returned 0 on success. -1 is returned on failure.

The GPSd client library provides an API to automatically stream the fix back to the application. This is done using the `gps_stream` API.

The `gps_stream` prototype is as follows.

```c
int gps_stream(struct gps_data *gps_data, unsigned int flags, void *data);
```

the `flags` arguments allows the method or / and type of streaming.

One of the most commonly used `flags` are `WATCH_ENABLE`.

The `gps_read` prototype is as follows.

```c
int gps_read(struct gps_data_t *gps_data);
```

The `gps_read` API polls the GPS daemon for the data. Thus the polling frequency also does matter on your GPS fix quality or difference between two GPS fixes. The good quality of the GPS fix is when your poll rate becomes same as the update rate on the serial device (GPS hardware communication channel).

The GPSd provides another API to selectively wait for the application to wait for GPS data.

```c
bool gps_waiting(struct gps_data_t *gps_data, int timeout);
```

The `gps_waiting` waits for the GPS data from the GPS socket for `timeout` milliseconds. If the data comes with in the `timeout` the function returns true and otherwise on a timeout, it returns false. Upon the `gps_waiting` returning `true` the `gps_read` API is called to get the GPS data.

The `gps_close` prototype is as follows.

```c
void gps_close(struct gps_data_t *gps_data);
```

The `gps_close` closes or ends the session.


One example of the GPS API is follows.

```c
int main(int argc, char **argv)
{
    struct gps_data_t gps_data;
    int ret;

    ret = gps_open(NULL, 0, &gps_data);
    if (ret < 0) {
        fprintf(stderr, "failed to open connection to GPSd\n");
        return -1;
    }

    gps_stream(&gps_data, WATCH_ENABLE, NULL);

    while (1) {
        if (gps_waiting(&gps_data, 500)) {
            if (gps_read(&gps_data) == -1) {
                fprintf(stderr, "failed to read GPS data\n");
                return -1;
            }
            if ((gps_data.status == STATUS_FIX) && (gps_data.mode >= 2)) {
                fprintf(stderr, "GPS mode : %d\n", gps_data.mode);
                if (!isnan(gps_data.latitude)) {
                    fprintf(stderr, "GPS latitude : %f\n", gps_data.latitude);
                }

                if (!isnan(gps_data.longitude)) {
                   fprintf(stderr, "GPS longitude : %f\n", gps_data.longitude);
                }
            }
        }
    }

    gps_stream(&gps_data, WATCH_DISABLE, NULL);
    gps_close(&gps_data);
}
```

(Remember to use our `select` interface's timeout option is also an alternative to the `gps_waiting` in here that when the timer expire periodically and calling the `gps_read`)

## mbedtls

Mbedtls is an encryption library that is similar to the **openssl**.

Mbedtls provide the `hash`, `encryption`, `random number generator`, `ssl` and `random` number generator.

The Mbedtls is an easy to use cryptographic library.

The Mbedtls API change continuously.. so i have kept the flags  makefile that looks something like below:

```shell
$(MBEDTLS_DIR) = $(HOME)/GitHubProjects/mbedtls/
    
CFLAGS += -Wall
INCLUDES += -I$(MBEDTLS_DIR)/include/
LIBRARIES += $(MBEDTLS_DIR)/library/libmbedtls.a $(MBEDTLS_DIR)/library/libmbedcrypto.a $(MBEDTLS_DIR)/library/libmbedx509.a
```

### hash functions:

### md5 API:
    
md5 is an algorithm that produces a 16 byte hash out of an input string. The hashing algorithms always follow the avalanche effect (A small variation in the input provides a large variation in the hash). (You can download the example [here](https://github.com/DevNaga/gists/blob/master/hash_Test.c))
     
**MD5 API description**:     

```c
md5(input, len, hash);
```

#### Example:
     
```c
#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/md5.h"
#include <stdio.h>
#include <stdint.h>
          
int digest_print(uint8_t *str, int len)
{
    int i;
              
    for (i = 0; i < len; i ++) {
        printf("%02x", str[i] & 0xff);
    }
              
    printf("\n");
              
    return 0;
}
          
int main(int argc, char **argv)
{
    int i;
    uint8_t digest[16];
              
    if (argc != 2) {
        printf("%s <string>\n", argv[0]);
        return -1;
    }
              
    mbedtls_md5(argv[1], strlen(argv[1]), digest);
              
    digest_print(digest, sizeof(digest));
              
    return 0;
}
          
```

We use the above makefile that we have created and generate our binary.      
Run the binary with the arguments to see the produced 16 byte hash value. This is the hash of the input that we have given.

### Cipher information

cipher list that is supported by the library can be get using the `mbedtls_cipher_list` API. (Downloadable [here](https://github.com/DevNaga/gists/blob/master/cipher_list.c))

Here is the simple example,

**Example**:

```c
#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/cipher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    int *cipher_list;
    mbedtls_cipher_info_t *cipher_info;

    cipher_list = mbedtls_cipher_list();
   	if (!cipher_list) {
    	fprintf(stderr, "failed to get cipher list\n");
	    return -1;
    }

    while (*cipher_list) {
	    cipher_info = mbedtls_cipher_info_from_type(*cipher_list);
	    fprintf(stderr, "cipher name \t%s\n", cipher_info->name);
	    fprintf(stderr, "cipher keylen \t%d\n", cipher_info->key_bitlen);
  	    cipher_list ++;
    }

    return 0;
}
```

### MD information

The list of available MD algorithms can be get using the `mbedtls_md_list` API. The below example shows how one can do. (Downloadable [here](https://github.com/DevNaga/gists/blob/master/md_list.c))

```c
 
#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/md_internal.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char **argv)
{
	int *md_list;

	md_list = mbedtls_md_list();
	if (!md_list) {
		fprintf(stderr, "failed to get mdlist\n");
		return -1;
	}

	while (*md_list) {
		mbedtls_md_info_t *md_info;

		md_info = mbedtls_md_info_from_type(*md_list);
		fprintf(stderr, "MD name: \t%s\n", md_info->name);
		fprintf(stderr, "MD size: \t%d\n", md_info->size);

		md_list ++;
	}

	return 0;
}

```

More versatile program is below which finds the hash for a given hashing algorithm and the string.


```c

#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/md_internal.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char **argv)
{
	int *md_list;

    if (argc != 3) {
    	printf("%s <algorithm> <string-to-hash>\n", argv[0]);
    	return -1;
    }

	md_list = mbedtls_md_list();
	if (!md_list) {
		fprintf(stderr, "failed to get mdlist\n");
		return -1;
	}

	while (*md_list) {
		mbedtls_md_info_t *md_info;

		md_info = mbedtls_md_info_from_type(*md_list);

        if (!strcasecmp(md_info->name, argv[1])) {
        	int i;
        	int hash_len;
        	uint8_t hash[200];

            memset(hash, 0, sizeof(hash));

        	md_info->digest_func(argv[2], strlen(argv[2]), hash);

            fprintf(stderr, "MD name: \t%s\n", md_info->name);
    		fprintf(stderr, "MD size: \t%d\n", md_info->size);

        	printf("hash: ");
        	for (i = 0; i < md_info->size; i ++) {
        		printf("%02x", hash[i]);
        	}

        	printf("\n");
        	break;
        }
		md_list ++;
	}

	return 0;
}

```

## Version control systems

###Git

1.  Git is very powerful version control system.
2.  Created by Linus Torvalds, also the creator of Linux Operating system.
3.  Git allows versioning of a file or set of files into a group called repository stored somewhere in the server.
4.  Git can also be thought of as a time travel system to go back in time to find the versions of the file or set of files that are good / bad and find what changed and who changed it. Also allows going back to the older or last good set of files.
5.  In most cases Git is used to maintain the code. In many other cases even the documents.
6.  This very book is written on a platform that uses the Git as one of its backend.
7.  installation of the git:

    on Ubuntu:
    
        sudo apt-get install git-all
        
    on Fedora:
    
        # dnf install git-all
   

7.  Git commands


| command | description |
|---------|-------------|
|add  |      Add file contents to the index|
|bisect |    Find by binary search the change that introduced a bug |
|branch  |    List, create, or delete branches |
|checkout |   Checkout a branch or paths to the working tree |
| clone  |     Clone a repository into a new directory |
| commit  |    Record changes to the repository |
| diff  |      Show changes between commits, commit and working tree, etc |
| fetch  |    Download objects and refs from another repository |
| grep   |     Print lines matching a pattern |
| init   |     Create an empty Git repository or reinitialize an existing one |
|log    |     Show commit logs |
|merge  |     Join two or more development histories together |
| mv    |       Move or rename a file, a directory, or a symlink |
| pull      |   Fetch from and integrate with another repository or a local branch |
| push     |    Update remote refs along with associated objects |
|rebase  |    Forward-port local commits to the updated upstream head |
| reset   |    Reset current HEAD to the specified state |
| rm     |     Remove files from the working tree and from the index |
|show  |      Show various types of objects |
| status  |    Show the working tree status |
|tag   |      Create, list, delete or verify a tag object signed with GPG |


## git examples (command line)

#### clone a repository:

```bash
git clone https://github.com/DevNaga/gists.git
```

#### check the status of the branch

```bash
git status
```

#### configure the git username

```bash
git config --add user.name "devnaga"
```

#### confiure the git user email address

```bash
git config --add user.email "devendra.aaru@gmail.com"
```

#### configure the color screen for branch and status

```bash
git config --add color.diff auto
git config --add color.branch auto
git config --add color.status auto
```

#### configure the difftool

```bash
git config --add diff.tool meld
```

#### configure the favourite editor for the commit

```bash
git config --add core.editor subl
```

#### pull the code in the repository:

```bash
git pull
```

#### launch a difftool

```bash
git difftool
```

#### create a branch:

```bash
git branch new
```

#### delete a branch:

```bash
git branch -D new
```

#### stash local changes

```bash
git stash
```

#### list the stashed items

```bash
git stash list
```

#### show the stashed change

```bash
git stash show -p stash@{0}
```

where `0` is the number of the stash.

#### diff the local changes

```bash
git diff
```

#### counters of the diff

```bash
git diff --stat
```

#### view the changes in the staging

```bash
git diff --cached
```

#### view the counters of the changes in the staging

```bash
git diff --stat --cached
```

#### commit the changes

```bash
git commit
```

#### push the changes

```bash
git push origin <branch-name>
```

#### merge a branch

```bash
git merge origin/<branch-name>
```

#### rebase the branch

```bash
git rebase -i
```

where i is for the interactive rebase.


## Git clients
1. https://www.gitkraken.com/
2. https://desktop.github.com/

More on the git is at the below links:

### Links:

https://git-scm.com/book/en/v2/Getting-Started-Git-Basics


## Openssl

Openssl is a widely used crypto library and SSL communications library. Below are some of the examples of the openssl library

### IV - Initial Vector

IV is used only at the beginning. The IV is used as the initial value for the rounds. IVs are given to the receivers to aid in decrypt stage.

### symmetric key

Symmetric keys are kept secretly somewhere.

### nonce - Number used only once

### useful context privs

1. EVP_CIPHER_CTX

### Useful API

```c
1. EVP_BytesToKey
2. EVP_aes_256_cbc()
3. EVP_sha1
4. EVP_CIPHER_CTX_init
5. EVP_EncryptInit_ex
6. EVP_DecryptInit_ex
7. EVP_EncryptUpdate
8. EVP_EncryptFinal_ex
9. EVP_DecryptUpdate
10. EVP_DecryptFinal_ex
11. EVP_CIPER_CTX_cleanup

```


### useful include

```c
1. openssl/evp.h
2. openssl/sha.h

```



Ubuntu does not have the OpenSSL libraries. Use the following command line to install the OpenSSL libraries.

```shell
sudo apt install libssl-dev
```

Linker flags: **-lssl -lcrypto**

**Example: SHA1 generation**

```c
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <openssl/sha.h>

int main(int argc, char **argv)
{
        int fd;
        uint8_t sha1sum[40];

        if (argc != 2) {
                return -1;
        }

        fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
                return -1;
        }

        SHA_CTX c;

        SHA1_Init(&c);

        for (;;) {
                char data[2000];
                int ret;

                memset(data, 0, sizeof(data));
                ret = read(fd, data, sizeof(data));
                if (ret <= 0) {
                        break;
                }
                SHA1_Update(&c, data, ret);
        }
        SHA1_Final(sha1sum, &c);

        int i;

        printf("sha1sum: ");
        for (i = 0; i < SHA_DIGEST_LENGTH; i ++) {
                printf("%02x", sha1sum[i]);
        }
        printf("\n");

        return 0;
}
```

**Example: SHA256 generation**

```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/sha.h>

int main(int argc, char **argv)
{
	if (argc != 2) {
		return -1;
	}

	int fd;

	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		return -1;
	}

	SHA256_CTX ctx;

	SHA256_Init(&ctx);
	int i;

	int ret;

	for (;;) {
		char data[2000];

		memset(data, 0, sizeof(data));
		ret = read(fd, data, sizeof(data));
		if (ret <= 0) {
			break;
		}
		SHA256_Update(&ctx, data, ret);
	}

	uint8_t md[64];

	SHA256_Final(md, &ctx);

	printf("[%s] sha256sum: ", argv[1]);
	for (i = 0; i < SHA256_DIGEST_LENGTH; i ++) {
		printf("%02x", md[i]);
	}
	printf("\n");
}
```

**Example: SHA512 generation**

```c
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <openssl/sha.h>

int main(int argc, char **argv)
{
    int fd;
    int i;
    int ret;

    if (argc != 2) {
        return -1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    SHA512_CTX ctx;

    SHA512_Init(&ctx);
    for (;;) {
        char data[2000];

        memset(data, 0, sizeof(data));
        ret = read(fd, data, sizeof(data));
        if (ret <= 0) {
            break;
        }

        SHA512_Update(&ctx, data, ret);
    }

    uint8_t md[64];

    SHA512_Final(md, &ctx);

    printf("[%s] sha512sum:  ", argv[1]);
    for (i = 0; i < SHA512_DIGEST_LENGTH; i ++) {
        printf("%02x", md[i]);
    }
    printf("\n");

    return 0;
}
```

**Example: Random number generator**

```c
/**
 * Program to generate rand num @ linux OS
 *
 * Dev Naga (devendra.aaru@gmail.com)
 */
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
    int fd;
    uint8_t rand[64];
    int ret;
    int no_bytes;
    int i;

    if (argc != 2) {
        return -1;
    }

    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    no_bytes = atoi(argv[1]);

    if (no_bytes > sizeof(rand)) {
        return -1;
    }

    memset(rand, 0, sizeof(rand));
    ret = read(fd, rand, no_bytes);
    if (ret != no_bytes) {
        return -1;
    }

    for (i = 0; i < no_bytes; i ++) {
        printf("%02x", rand[i]);
    }
    printf("\n");

    return 0;
}
```

## protocol buffers

### Intro

* usually the message communication involve exchanging the data between one or more machines. The data may contain variables of different types and are to be kept in the same fashion at the sender that the receiver understands. Meaning a common understanding to be known between the two or more parties.



* The information is then represented in the structured format that defines the placement and types of data that the structure can contain. The structure is then transmitted over the network.


* However, serialisation comes into the picture when we are transmitting data between two different machines of different architectures (such as big and little endian). Thus we convert everything that we transmit into the network endian (usually big endian) and transmit and the receiver converts it back to host endian (its machine endian format) and reads the data after. Then, it comes to the encoding of types and data, for ex, the `long` type is of 4 bytes on a 32 bit machine, however in the most common present world we are using almost all 64 bit machines. However, the exchange of communication is also possible with the embedded devices such as phones, GPS receivers, routers, switches etc. The exact representation of the data is to be defined in this situation. This is where the serialisation comes into the picture.


* Serialsing the data involves telling the other party the type of the each element in the structure that we are sending, the size each element can hold to etc.


* One of the serialisation formats is the protocol buffer format created by Google.


* The protocol buffers asks to define the structure that we wanted to send in a `.proto` format.


* For example:

```protobuf
package position_info;
        
enum ModeOfFix {
    Mode_No_fix = 0;
    Mode_2D_fix = 1;
    Mode_3D_fix = 2;
}
    
message gpsdata {
    required ModeOfFix fix_mode = 1 [default = Mode_No_fix];
    required double latitude = 2;
    required double longitude = 3;
    required double altitude = 4;
}
```
       
The above example defines a fix mode, latitude, longitude and altitude in a structure position_information. The position_information is what we wanted to send over the network. The position_information may be filled from the GPS data. See that the protobuf allows us to send the double types over the network. The .proto file starts with a package definition to avoid naming conflicts between different packages or different structure defintions in different files.

Here we are going to concentrate mostly on the protobuf-c.

### Data types of the protobuf format

|data type | Description |
|-|-|
|double | double data type in C |
|float | float data type in C |
|int32 | Variable length encoding and only encodes positive 32 bit numbers |
|int64 | Variable length encoding and only encodes positive 64 bit numbers |
| uint32 | only positive 32 bit |
| uint64 | only positive 64 bit |
| sint32 | Variable length encoding and only encodes negative 32 bit numbers |
| sint64 | Variable length encoding and only encode negative 64 bit numbers |
| fixed32 | fixed 32 bit unsigned |
| fixed64 | fixed 64 bit unsigned |
| sfixed32 | always 32 bit signed |
| sfixed64 | always 64 bit signed |
| bool | boolean data type |
| string | strings |
| bytes | byte array |


For C, there is another latest C bindings implementation of protobuf is [here](https://github.com/protobuf-c/protobuf-c.git). In order to use this first we need protobuf compiler which can be found [here](https://github.com/google/protobuf.git).

To compile the protobuf, here are the following instructions.

```bash
./autogen.sh
./configure
make
sudo make install
```

You might encounter an issue in locating the library `libprotobuf.so.17`, simply do the following at `protobuf`.

```bash
cp -r ./src/.libs/libprotobuf.so* /usr/lib/
sudo ldconfig
```

The same steps are followed for the protobuf-c bindings.

Lets run the compiler on the above proto definition as follows. Copy the contents to a file and name it `test.proto`.

```bash
protoc-c --c_out=. test.proto
```

The above command generates one .C file and one .h file.

Here is a brief explanation on the protobuf file above.

The `message` field is a C structure. The `enum` is a C enum. each element of the `message` or the `enum` must contain a number that defines the order of the elements that appear in the output buffer. The `message` is used to define the content of the data that is to be serialized.

Each element described, can be an optional element (only rarely used thus no need to encode it always) and mandatory elements. The optional elements are used to reduce the overall encoded payload. The mandatory elements are tagged with `required` attribute before the declaration of the variable. The optional elements are tagged with `optional` attribute.

The sample test program that utilizes the generated code is as follows. The below code encodes / serializes the structured data.

```c
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "test.pb-c.h"

int main(int argc, char **argv)
{
    PositionInfo__Gpsdata gps = POSITION_INFO__GPSDATA__INIT;
    int i;
    uint8_t buf[300];
    int len;

    gps.fix = POSITION_INFO__MODE_OF_FIX__Mode_No_fix;
    gps.latitude = 1.1;
    gps.longitude = 12.2;
    gps.altitude = 3.3;

    len = position_info__gpsdata__get_packed_size(&gps);

    position_info__gpsdata__pack(&gps, buf);

    fprintf(stderr, "Writing %d serialized bytes\n", len);

    for (i = 0; i < len; i ++) {
        printf("%02x", buf[i]);
    }

    printf("\n");

    return 0;
}

```

The main important functions are getting the length of a payload and packing the bytes into the buf object. Here are the two APIs that are auto generated by the compiler.

```c
position_info__gpsdata__get_packed_size
position_info__gpsdata__pack
```

The generated bytes are then copied over to the network using the networking sockets described in the before chapters in this book.

The sample code above with the generated C file is compiled as follows. Copy the sample code into a file call posinfo.c.

```bash
gcc posinfo.c test.pb-c.c -lprotobuf-c -I.
```

Running the program gives the following.

```bash
Writing 29 serialized bytes
0800119a9999999999f13f196666666666662840216666666666660a40
```

Lets add code to decode the above messgae. The final program looks below.

```c
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "test.pb-c.h"

int main(int argc, char **argv)
{
    PositionInfo__Gpsdata gps = POSITION_INFO__GPSDATA__INIT;
    PositionInfo__Gpsdata *gps_dec;
    int i;
    uint8_t buf[300];
    int len;

    gps.fix = POSITION_INFO__MODE_OF_FIX__Mode_No_fix;
    gps.latitude = 1.1;
    gps.longitude = 12.2;
    gps.altitude = 3.3;

    len = position_info__gpsdata__get_packed_size(&gps);

    position_info__gpsdata__pack(&gps, buf);

    fprintf(stderr, "Writing %d serialized bytes\n", len);

    for (i = 0; i < len; i ++) {
        printf("%02x", buf[i]);
    }

    printf("\n");

    gps_dec = position_info__gpsdata__unpack(NULL, len, buf);
    if (!gps_dec) {
        return -1;
    }

    printf("fix %d\n", gps_dec->fix);
    printf("lat %f long %f alt %f\n",
                    gps_dec->latitude,
                    gps_dec->longitude,
                    gps_dec->altitude);

    position_info__gpsdata__free_unpacked(gps_dec, NULL);

    return 0;
}
```

The result of the output when compiled and run is as follows.

```bash
Writing 29 serialized bytes
0800119a9999999999f13f196666666666662840216666666666660a40
fix 0
lat 1.100000 long 12.200000 alt 3.300000
```

One of the other features of the protobuf are the use of repeated fields and optional fields. Sometimes, we would like to pass an array of n members whose size is known dynamically at the run time. So protobuf support this concept.

Consider the below proto file for example.

```protobuf
message repeated_msg {
    repeated int32 n_apples = 1;
};
```

Now, similar to this, the optional elements are tagged before.

```protobuf
message repeated_msg_optional {
    repeated int32 n_apples = 1;
    required int32 n_baskets = 2;
    optional int32 n_apple_trees = 3;
}
```

Let's say that the `n_baskets` in the above proto definition is not filled while serializing. Thus it may contain the garbage data and the `default` attribute comes to the rescue.

The below definition fixes that.

```protobuf
message repeated_msg_optional {
    repeated int32 n_apples = 1;
    required int32 n_baskets = 2 [default = 10]
    optional int32 n_apple_trees = 3;
}
```

The apples are used to make the apple juice and lets' link the apples structure into the juice structure.


```protobuf
// juice structure
message juice {
    required apples quantity = 1;
    required int32 liters = 2;
}

// apple structure
message apples {
    repeated n_apples = 1;
    required n_baskets = 2 [default = 10]
}
```

##More on protocol buffers:

[1]. https://developers.google.com/protocol-buffers/
[2]. https://developers.google.com/protocol-buffers/docs/proto?csw=1

## CAN controller (socketCAN interface)

### Linux socketCAN layer

linux interface provides a virtual socket CAN layer that is used from the app just like the use of `socket` with `AF_RAW`, linux socketCAN layer provides `AF_CAN` family.

in case there is no CAN hardware, the kernel provide a virtual linux driver.

Here's how to setup the virtual CAN interface,


```bash
sudo modprobe can_dev
sudo modprobe can
sudo modprobe can_raw
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
```

cross verify if the link is available,

```bash
ip link show vcan0
```

this should show up `vcan0` interface.

install `can-tools`. On Ubuntu, one can install using `apt`.

```bash
sudo apt install can-tools
```

use header file `linux/can.h` for any of the CAN data structures.

The CAN protocol uses the family `AF_CAN` (defined in linux/can.h) and the `CAN_RAW` to send raw CAN frames.

A CAN frame is a TLV data frame where the TAG refers to the CAN id, length is the length of the Data field and V is the data in bytes. The data could go till maximum of 8 bytes.

the header `linux/can.h` contains the can frame `struct can_frame` defined:

```c
struct can_frame {
    canid_t canid;
    __u8 dlc;
    __u8 pad;
    __u8 res0;
    __u8 res1;
    __u8 data[CAN_MAX_DLEN];
};
```

Below example provides the usage of the linux CAN layer APIs. Download [here](https://github.com/DevNaga/gists/blob/master/can_frame.c)

```c
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int fd;
    struct sockaddr_can can;
    struct can_frame canframe;
    struct ifreq ifr;
    int ret;

    fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (fd < 0) {
        return -1;
    }

    strcpy(ifr.ifr_name, "vcan0");
    ret = ioctl(fd, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        return -1;
    }

    can.can_family = AF_CAN;
    can.can_ifindex = ifr.ifr_ifindex;

    ret = bind(fd, (struct sockaddr *)&can, sizeof(can));
    if (ret < 0) {
        return -1;
    }

    static int i;

    while (1) {
        sleep(1);

        canframe.can_id = 0x600 + i;
        canframe.can_dlc = 8;
        canframe.data[0] = 0x44 + i;
        canframe.data[1] = 0x44 + i;
        canframe.data[2] = 0x44 + i;
        canframe.data[3] = 0x44 + i;
        canframe.data[4] = 0x44 + i;
        canframe.data[5] = 0x44 + i;
        canframe.data[6] = 0x44 + i;
        canframe.data[7] = 0x44 + i;

        printf("write %02x\n", canframe.can_id);

        ret = write(fd, (void *)&canframe, sizeof(canframe));
        if (ret < 0) {
            return -1;
        }

        i ++;
        if (i > 4) {
            i = 0;
        }
    }

    return 0;
}

```

in the above example, the structure `struct sockaddr_can` is used to fill in the CAN specific information such as `ifindex`, `family`, so that a `send` operation can be performed on the CAN device.

the `ifindex` is interface index of the device which can be found using `SIOCGIFINDEX` ioctl call. the ioctl call as described in before topics can be used to get the interface index on a CAN device.

to bind the socket to all the interfaces the `can_ifindex` must be set to 0.

If the `bind` is done on all interfaces, then it is recommended to use `recvfrom` than `recv` to know which interface the CAN frames being received. and to send it is recommended to use `sendto` than `send` or `write`.

The above program writes a series of CAN messages with IDs ranging from 0x600 till 0x604 and after that repeats the same every second.

compile and run the above program. The program expects to have a virtual can device `vcan0`. First the interface needs to be created and configured before running the program.

now after running the above program, run the can trace.

```shell
candump vcan0

  vcan0  600   [8]  44 44 44 44 44 44 44 44
  vcan0  601   [8]  45 45 45 45 45 45 45 45
  vcan0  602   [8]  46 46 46 46 46 46 46 46
  vcan0  603   [8]  47 47 47 47 47 47 47 47
  vcan0  604   [8]  48 48 48 48 48 48 48 48

```

The above displays the CAN message going over to the vcan0.

One of the examples of receiving CAN messages is described below. Download [here]()

```c
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <linux/can/raw.h>

int main(int argc, char **argv)
{
    int fd;
    struct sockaddr_can can;
    struct can_frame canframe;
    struct ifreq ifr;
    int ret;

    fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (fd < 0) {
        return -1;
    }

    strcpy(ifr.ifr_name, "vcan0");
    ret = ioctl(fd, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        return -1;
    }

    can.can_family = AF_CAN;
    can.can_ifindex = ifr.ifr_ifindex;

    ret = bind(fd, (struct sockaddr *)&can, sizeof(can));
    if (ret < 0) {
        return -1;
    }

#if 0
    canframe.can_id = 0x600;
    canframe.can_dlc = 8;
    canframe.data[0] = 0x44;
    canframe.data[1] = 0x44;
    canframe.data[2] = 0x44;
    canframe.data[3] = 0x44;
    canframe.data[4] = 0x44;
    canframe.data[5] = 0x44;
    canframe.data[6] = 0x44;
    canframe.data[7] = 0x44;
#endif

    struct can_filter f[2];

    f[0].can_id = 0x600;
    f[0].can_mask = CAN_SFF_MASK;

    f[1].can_id = 0x601;
    f[1].can_mask = CAN_SFF_MASK;

    ret = setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, &f, sizeof(f));
    if (ret < 0) {
        return -1;
    }

    while (1) {
        sleep(1);

        ret = read(fd, (void *)&canframe, sizeof(canframe));
        if (ret < 0) {
            return -1;
        }

        printf("%02x %02x ", canframe.can_id, canframe.can_dlc);
        int i;

        for (i = 0; i < canframe.can_dlc; i ++) {
            printf("%02x:", canframe.data[i]);
        }
        printf("\n");
    }

    return 0;
}

```

The above code has the setup code similar to the `write` on the CAN, except the filters and the `read` calls.

to selectively filter out the CAN frames at the network layer, the `SOL_CAN_RAW` socket option is used. Below is one example code snippet.

```c
struct can_filter f[2];

f[0].can_id = 0x600;
f[0].can_mask = CAN_SFF_MASK; // standard CAN frame only

f[1].can_id = 0x601;
f[1].can_mask = CAN_SFF_MASK;

ret = setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, &f, sizeof(f));
if (ret < 0) {
    return -1;
}

```

The above example use a structure `struct can_filter`. It contains the below format,

```c
struct can_filter {
    can_id_t can_id;
    can_id_t can_mask;
};

```

the `can_id` is the ID of the frame that the program wants to receive and the mask can be one of `CAN_SFF_MASK` or `CAN_EFF_MASK`.

if the standard CAN messages to be received then `CAN_SFF_MASK` is used, and if the extended CAN messages are to be received then `CAN_EFF_MASK` is used.

the `setsockopt` option `SOL_CAN_RAW` accepts a `CAN_RAW_FILTER` with a filter of type `struct can_filter`. This can be an array and set to the `setsockopt` as described above.

The `read` system call above reads each CAN frame and dumps them on the screen, only the DLC `can_dlc` bytes are set in the `data` portion and the rest must be treated as not set or invalid while processing the CAN data.

The structure `struct can_frame` is filled with `can_id` `can_dlc` and `data` members upon a `recvfrom` call when given as argument to `recvfrom`.

Below is one of the example of such case.

```c
struct sockaddr_can can;
struct ifreq ifr;
socklen_t len = sizeof(can);
struct can_frame f;

ret = recvfrom(fd, &f, sizeof(f), 0, (struct sockaddr *)&can, &len);
if (ret < 0) {
    return -1;
}

if (ret != sizeof(f)) {
    return -1;
}

ifr.ifr_ifindex = can.can_ifindex;
ret = ioctl(fd, SIOCGIFNAME, &ifr);
if (ret < 0) {
    return -1;
}

printf("CAN ifname %s\n", ifr.ifr_name);
```

### CAN broadcast manager (BCM)



## SQL

`SQL` is a systems query language and is a database thats very popular and widely used.

1. CREATE TABLE celebs (id INTEGER, name TEXT, age INTEGER);

Creates a new Table entry
2. INSERT INTO celebs (id, name, age) VALUES(1, 'Jason Statham', 40);
3. SELECT * FROM celebs


Message exchange methods such as XML and JSON are heavy on the communication channel (internet for example) between the client and server. 

## XML

XML is another way for communication between the nodes, client and servers or between the subscriber and publisher.

```xml
<xml version="1.0" ?>
<gnss_message>
    <latitude>42.1491</latitude>
    <longitude>-83.32131</longitude>
    <elevation>100</elevation>
</gnss_message>
```

A structure name must be within `<` and `>`. A structure always have the closing statement. The structure must be same name but in between `</` and `>`.

## Jsoncpp

JSON is the form for JavaScript Object Notation. Json is a popular schema for programs written in Javascript. As well in most of the systems programming techniques and services. The most common uses of json is in message exchange between the client and server or between the publisher and subscribers.

The syntax format of json looks as follows.

```json

"gnss_message": {
    "latitude": 42.1491,
    "longitude": -83.32131,
    "elevation": 100,
    "track": 211
}

```

The above is a message that defines a `gnss_message` structure. It contains 4 parameters of type `double`. Each line above ends with a `,` describing there is a continuation or further presence of elements. The last line does not terminate with a `,` indicating that its the end of elements. The `}` completes the structure `gnss_message`.

Another example of the message could be the following.

```json

"hardware_information": {
    "cpu_name": "intel core i7",
    "harddrive_avaialble": true,
    "has_ssd": true
}

```

Above message defines further types `stirng` and `bool`.

Further a message can be written within another structure.

```json

{
"camera_set": [
    {
        "type": "fish_eye",
        "position": "front",
        "send_address": "/dev/camera0",
        "camera_resolution": "1024x768"
    },
    {
        "type": "fish_eye",
        "position": "back",
        "send_address": "/dev/camera1",
        "camera_resolution": "1024x768"
    }
]

}


```

Above message defines an array of camera hardware information. It has two camera sets, front and back sets.

### Using jsoncpp

1. Install `jsoncpp`

```bash

sudo apt install libjsoncpp-dev

```

include headerfile `jsoncpp/json/json.h` to access the API.

Link with the `-ljsonpp`.


Below is an example of reading from a file.


```cpp

#include <iostream>
#include <string>
#include <fstream>
#include <jsoncpp/json/json.h>

struct camera {
    std::string type;
    std::string position;
    std::string send_address;
    std::string camera_resolution;
};

struct camera_data {
    camera cameras[2];
};

int main()
{
    Json::Value root;
    std::ifstream config("./camera.json", std::ifstream::binary);

    config >> root;

    int i = 0;
    camera_data data;
    for (auto it : root["camera_set"]) {
        data.cameras[i].type = it["type"].asString();
        data.cameras[i].position = it["position"].asString();
        data.cameras[i].send_address = it["send_address"].asString();
        data.cameras[i].camera_resolution = it["camera_resolution"].asString();

        i ++;
    }

    for (i = 0; i < 2; i ++) {
        printf("camera [%d]: \n", i);
        printf("\t type : %s\n", data.cameras[i].type.c_str());
        printf("\t position : %s\n", data.cameras[i].position.c_str());
        printf("\t send_address : %s\n", data.cameras[i].send_address.c_str());
        printf("\t camera_resolution : %s\n", data.cameras[i].camera_resolution.c_str());
    }
}

```



