# atoi and string to number conversion API


The `atoi` function perform an asci to integer conversion of a number stored in the string form. If it can't convert it, then it would return simply 0. However, value 0 is also a number and is valid. So in most of the cases `atoi` is only used for simple numbers and to programs that the input number string expected is always correct. In most of the cases, the `strtol` family of functions are preferred.

We have written our own `atoi` function here. This version detects the errors by checking if a character is non number (`isdigit` macro from the `<ctype.h>`) and fails on an invalid input.

The rest of the code here performs a scanning of integer from a string input.

#### our own string to integer conversion function
```c
#include <stdio.h>
#include <ctype.h>

int our_atoi(char *string)
{
    int num;
    int ret;
    int i;
    int len = strlen(string);

    for (i = 0; i < len; i++) {
        if (!isdigit(string[i]))
            return -1;
    }

    ret = sscanf(string, "%d", &num);
    if (ret == 1)
        return num;

    return -1;
}
```

# strtol

The strtol converts a string to the `long` type. The prototype looks as follows.

```c
long strtol(const char *str, char **endptr, int base)
```

It returns the converted value from the string `str`.

If the string does not contain the number (or any character that is present in the string), it will store the string from that character into the `endptr`.

The `base` variable is used to tell the function if the string is in base 10 (i.e decimal numbers) or base 16 (hexadecimal numbers).

For hexadecimal numbers the input can either have the `0x` or does not need to.

The below example shows the usage of `strtol`. It takes an input string from the command line, uses `strtol` and converts it into the decimal number. After the conversion, it prints the number on to the console.

The strtol example: (base 10)

```c
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int data;
    char *errorp = NULL;

    if (argc != 2) {
        printf("%s integer\n", argv[0]);
        return -1;
    }

    data = strtol(argv[1], &errorp, 10);
    if (errorp && (errorp[0] != '\0')) {
        printf("invalid number %s\n", argv[0]);
        return -1;
    }

    printf("converted %d\n", data);

    return 0;
}

```

With this example, we now write a safe string to integer conversion API instead of using the buggy `atoi` API.

The below function converts a string (the number is in decimal fashion) to an integer.

```c
int string_to_int_d(char *string, int *int_var)
{
    int var;
    char *errorp = NULL;

    var = strtol(string, &errorp, 10);
    if (errorp && (errorp[0] != '\0')) {
        return -1;
    }

    *int_var = var;
    return 0;
}
```

The below function converts a string (the number is in hexadecimal fashion) to an integer.

```c
int string_to_int_h(char *string, int *int_var)
{
    int var;
    char *errorp = NULL;

    var = strtol(string, &errorp, 16);
    if (errorp && (errorp[0] != '\0')) {
        return -1;
    }

    *int_var = var;
    return 0;
}
```

# strtoul

The `strtoul` function converts a number in the string into the unsigned long. The prototype of the function is as follows.

```c
unsigned long int strtoul(const char *nptr, char **endptr, int base);
```

The following example shows the usage of the API.

```c
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char *endptr = NULL;
    uint32_t data;
	int base;

    if (argc != 3) {
		printf("%s [number] [base - 10, 16]\n", argv[0]);
		return -1;
    }

	base= atoi(argv[2]);

    data = strtoul(argv[1], &endptr, base);
	if (endptr && (endptr[0] != '\0')) {
		printf("invalid string number %s\n", argv[1]);
		return -1;
	}

	printf("Data %u\n", data);

	return 0;
}
```

# strtod

The `strtod` converts a string into a `double`. The prototype looks as follows.

```c
double strtod(const char *nptr, char **endptr);
```

Just like other functions, it is always necessary to check the `endptr`.

```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int ret;
    char *endptr = NULL;
    double val;

    if (argc != 2) {
        printf("%s [double] \n", argv[0]);
        return -1;
    }

    val = strtod(argv[1], &endptr);
    if (endptr && (endptr[0] != '\0')) {
        printf("invalid double value %s\n", argv[1]);
        return -1;
    }

    printf("value %f\n", val);

    return 0;
}
```
