# popen and pclose

`popen` and `pclose` are the useful API that are another form of executing a shell or a command with in the process.

The prototypes are as follows.

`FILE *popen(const char *command, const char *mode);`

`int pclose(FILE *stream);`

The `popen` call executes the shell command as a sub process. However, instead of waiting for the completion of the command, it returns a file pointer.

The file pointer can then be used to read or write to the stream. Meaning, a read gets the output of the command and a write sends the data to the sub process.

In case of failure, the file pointer returned will be `NULL`.

The `pclose` will then close the file pointer.

The following code sample shows the `popen` and `pclose`. You can download it in [here](https://github.com/DevNaga/gists/blob/master/popen_pclose.c)

```c
#include <stdio.h>

int main(void)
{
	char data[64];
	FILE *fp;

	fp = popen("ls -l", "r");
	if (!fp) {
		fprintf(stderr, "failed to open ls -l \n");
		return -1;
	}

	while (fgets(data, sizeof(data), fp)) {
		fprintf(stderr, "%s", data);
	}

	pclose(fp);

	return 0;
}
```

