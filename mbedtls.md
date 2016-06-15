# mbedtls

Mbedtls is an encryption library that is similar to the **openssl**.

Mbedtls provide the `hash`, `encryption`, `random number generator`, `ssl` and `random` number generator.

The Mbedtls is an easy to use cryptographic library.

The Mbedtls API change continuously.. so i have kept the flags  makefile that looks something like below:

    $(MBEDTLS_DIR) = $(HOME)/GitHubProjects/mbedtls/
    
    CFLAGS += -Wall
    INCLUDES += -I$(MBEDTLS_DIR)/include/
    LIBRARIES += $(MBEDTLS_DIR)/library/libmbedtls.a $(MBEDTLS_DIR)/library/libmbedcrypto.a $(MBEDTLS_DIR)/library/libmbedx509.a

1. hash functions:
  1. md5 API:
    
     md5 is an algorithm that produces a 16 byte hash out of an input string. The hashing algorithms always follow the avalanche effect (A small variation in the input provides a large variation in the hash). (You can download the example [here](https://github.com/DevNaga/gists/blob/master/hash_Test.c))
     
     MD5 API:     
      
          md5(input, len, hash);

     Example:
     
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
          
     We use the above makefile that we have created and generate our binary.      
     Run the binary with the arguments to see the produced 16 byte hash value. This is the hash of the input that we have given.
     
2. Cipher information

    cipher list that is supported by the library can be get using the `mbedtls_cipher_list` API. (Downloadable [here](https://github.com/DevNaga/gists/blob/master/cipher_list.c))

    Here is the simple example,

    Example:
```
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

3. MD information

   1. the list of available MD algorithms can be get using the `mbedtls_md_list` API. The below example shows how one can do.
 (Downloadable [here](https://github.com/DevNaga/gists/blob/master/md_list.c))
 
 
 ```
 
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

```

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
