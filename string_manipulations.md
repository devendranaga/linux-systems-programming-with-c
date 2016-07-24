# string manipulations

The C library \(libc\) provides an interface to manipulate with the strings.

A string is a sequence of characters. A sequence of characters are stored in a character array.

```
char char_array[10];
```

The above line defines a `char_array` of length 10 bytes. To keep characters in the array we could do the following.

```c
char_array[0] = 'l';
char_array[1] = 'i';
char_array[2] = 'n';
char_array[3] = 'u';
char_array[4] = 'x';
char_array[5] = '\0';
```

A string is always terminated by a null termination mark `'\0'`. The null terminator marks the end of the character array or string.

Or another way of assigning the string is the following.

```c
char char_array[] = "linux";
```

Here in the char\_array, we do not have to specify the size. Such variables are stored in the data section part of the final executable.

The `printf` or `fprintf` functions can be used to print the strings. The format specifier `%s` is used to print the string.

The below code prints the `char_array` on to the console.

```c
printf("%s", char_array);
```

Reading of the strings can be performed by the use of `fgets` function. However, the `scanf` can also be used but is too dangerous to read strings. Thus it is not a preferrable choice when reading the strings from the command line. The sample usage of `fgets` when reading the strings looks as follows...

```c
char data[10];

fgets(data, sizeof(data), stdin);
```

We read the string `data` of length 10 from the `stdin`. Where `stdin` is the standard input as we know it.

The standard library provides a string manipulation functions such as the following. Include the header file `<string.h>` for the API declarations.

| function | description |
| :--- | :--- |
| strlen\(str\) | calculate the length of the string |
| strcpy\(dst, src\) | copy the string from src to dst |
| strcat\(str2, str1\) | concatenate str2 and str1 |
| strcmp\(str1, str2\) | compare two strings str1 and str2 |
| strchr\(str, chr\) | find a character chr in the str |
| strstr\(str1, str2\) | find the str2 position with in str1 |

The above API are enough to perform the string manipulation of almost any kind of strings.

### Strlen

The `strlen` function usage is demoed below.

prototype: `size_t strlen(const char *s);`

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    int len;

    if (argc != 2) {
        printf("%s <string name>\n", argv[0]);
        return -1;
    }

    len = strlen(argv[1]);
    printf("length of the string is %d\n", len);

    return 0;
}
```

Alternatively the length of the string can be calculated manually. The length of the string is the count of characters till the null termination mark except the null terminator.

```c
int own_strlen(char *string)
{
    int i = 0;

    while (*string != '\0') {
        // increment i if there is a valid character
        i++;
        // move to the next address
        string++;
    }

    return i;
}
```

### Strcpy

The `strcpy` function usage is demoed below.

prototype: `char *strcpy(char *dst, const char *src);`

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    char *dst;
    int dst_len;

    if (argc != 2) {
        printf("%s <string name>\n", argv[0]);
        return -1;
    }

    // find the length of the destination and add 1 for null terminator
    dst_len = strlen(argv[1]) + 1;

    dst = calloc(1, dst_len);
    if (!dst) {
        return -1;
    }

    strcpy(dst, argv[1]);

    printf("Source %s Destination %s\n", argv[1], dst);

    free(dst);

    return 0;
}
```

Alternatively we can write own string copy function as the following.

```c
char* own_strcpy(char *src, char *dst)
{
    int i = 0;

    while (*dst != '\0') {
        src[i] = *dst;
        i++;
    }

    return src;
}
```

### Strcat

The `strcat` function concatenates or joins two strings together. The second string is joined at the end of the second string.

prototype: `char *strcat(char *s1, const char *s2);`

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char string1[100];

    if (argc != 3) {
        fprintf(stderr, "%s <string1> <string2>\n", argv[0]);
        return -1;
    }

    strcpy(string1, argv[1]);
    strcat(string1, argv[2]);

    printf("Source1 %s Source2 %s Destination %s\n", argv[1], argv[2], string1);

    return 0;
}
```

### Strcmp

The `strcmp` function compares two strings. If the strings are not equal then a non zero value is returned otherwise zero is returned.

prototype: `int strcmp(const char *s1, const char *s2);`

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int ret;
    char *string1, *string2;

    if (argc != 3) {
        fprintf(stderr, "%s <string1> <string2>\n", argv[0]);
        return -1;
    }

    string1 = argv[1];
    string2 = argv[2];

    ret = strcmp(string1, string2);

    printf("strings are %s\n", (ret == 0) ? "Equal" : "Not Equal");
    return 0;
}
```

### Strstr

strstr locates the first occurence of the given substring in the main string.

prototype: `char *strstr(const char *s1, const char *s2);`

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int ret;
    char *main_str, *sub_str, *match;

    if (argc != 3) {
        fprintf(stderr, "%s <main string> <sub string>\n", argv[0]);
        return 0;
    }

    main_str = argv[1];
    sub_str = argv[2];

    match = strstr(main_str, sub_str);
    printf("match found %s\n", match ? "Yes": "No");
    if (match) {
        printf("matching address %p string %s\n", match, match);
    }

    return 0;
}
```

### Strchr

strchr API locates the first occurence of the given character in the original string and returns the address of it.

prototype: `char *strchr(const char *s, int c);`

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    int ret;
    char *string, ch, *string_pos;

    if (argc != 3) {
        fprintf(stderr, "%s <string> <character>\n", argv[0]);
        return -1;
    }

    string = argv[1];
    ch = argv[2][0];

    string_pos = strchr(string, ch);
    if (string_pos) {
        fprintf(stderr, "character pos found %p and string %s\n", string_pos, string_pos);
    } else {
        fprintf(stderr, "character pos not found\n");
    }

    return 0;
}

```

### Strcasecmp

strcasecmp compares the two strings and matches them ignoring the case.

to use this API we should include `<strings.h>`.

prototype: `int strcasecmp(const char *s1, const char *s2);`

```c
#include <stdio.h>
#include <strings.h>

int main(int argc, char **argv)
{
    int ret;
    char *string1, *string2;

    if (argc != 3) {
        fprintf(stderr, "%s <string1> <string2>\n", argv[0]);
        return -1;
    }

    string1 = argv[1];
    string2 = argv[2];

    ret = strcasecmp(string1, string2);

    printf("strings are %s\n", (ret == 0) ? "Equal": "Not Equal");
    return 0;
}

```

### Strstr

strstr finds a substring inside a string. It returns the address of the first character if the substring has been found and null otherwise.

prototype: `char *strstr(const char *haystack, const char *needle);`

```c
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv)
{
        char *str;
        int ret;

        if (argc != 3) {
                printf("%s <string1> <substring>\n", argv[0]);
                return -1;
        }

        str = strstr(argv[1], argv[2]);
        if (str) {
                fprintf(stderr, "Found substring %s\n", str);
        } else {
                fprintf(stderr, "Substring not found\n");
        }

        return 0;
}

```

**Problems**:

The following problems are given to encourage the reader to put things that are learnt, into practice.

1. Program to find the frequency of occurence of a character `e` in the string "frequency".
2. Program to find the length of a string "Meaningful".
3. Program to sort the set of strings in alphabetical order. The strings are "Apple", "Banana", "Pine Apple", "Grapes", "Guava" and "Peach".
4. Given two strings "Apple" and "Banana". Concatenate from the third character of both of the strings. Meaning the final output would be "Appana".
5. Look up a string in a set of strings. Take any large set of strings and look up any string.

