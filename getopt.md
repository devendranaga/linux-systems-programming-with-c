# getopt

* parses arguments with options
* example:

```c
    #include <stido.h>
    #include <unistd.h>
    #include <getopt.h>

    int main(int argc, char **argv)
    {
        int opt;
        int time_delay = 0;

        while ((opt = getopt(argc, argv, "t:")) != -1) {
            switch (opt) {
                case 't':
                    time_delay = atoi(optarg);
                break;
                default:
                    printf("%s -t <time-delay in sec>\n", argv[0]);
                    return -1;
            }
        }

        printf("sleeping for %d\n", time_delay);
        sleep(time_delay);

        return 0;
    }
```

**    Example: getopt base example**

* We compile and generate the binary as the following:
      gcc -Wall getopt\_example.c -o getopt\_example

  ```bash
  ./getopt_example -t 1
  ```

  The above code sleeps for 1 second and stops the execution

* include files : `<getopt.h>` and `<unistd.h>`.

* getopt defines a set of variable that can be used by the program.

  * **opterr**: If the value of this variable is nonzero, then getopt prints an error message to the standard error stream if it encounters an unknown option character or an option with a missing required argument. If you set this variable to zero, getopt does not print any messages, but it still returns the character ? to indicate an error.

    * For ex: if we run the above example as the following `./getopt_example -d`. It would print `./getopt_example: illegal option -- d` on to the screen.

  * **optopt**: When getopt encounters an unknown option character or an option with a missing required argument, it stores that option character in this variable. You can use this for providing your own diagnostic messages.

  * **optind**:  This variable is set by getopt to the index of the next element of the argv array to be processed. Once getopt has found all of the option arguments, you can use this variable to determine where the remaining non-option arguments begin. The initial value of this variable is 1.

  * **optarg**: This variable is set by getopt to point at the value of the option argument, for those options that accept arguments.

    * we get this option in our example of the getopt and use it to convert into an integer that gives us the time to sleep in the code. The `optarg` is the most commonly used argument in any basic to an intermediate level program.



# getopt\_long

Some commands accept the input as the following...

```
command --list
```

The two  `--` allow to provide a descriptive command from the command line.

The option data structure looks as below that is passed as argument to `getopt_long`.

The data structure is as follows...

```c
struct option {
    const char *name;
    int has_arg;
    int *flag;
    int val;
};
```

Here is the description:

`name`: name of the long options.

`has_arg`:  `no_argument` \(0\) if the option does not take an argument. `required_argument` \(1\) if the option requires an argument. `optional_argument` \(2\) if the option takes an optional argument.

`flag`: usually set to 0.

`val`: the value to be used in short notation in case getopt is used.



```c
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

int main(int argc, char **argv)
{
    int opt;
    int opt_id = 0;
    static struct option long_opt[] = {
        {"option1", required_argument, 0, 'x'},
        {"option2", required_argument, 0, 'y'},
        {"option3", required_argument, 0, 'z'},
        {"usage", no_argument, 0, 'p'},
        {0, 0, 0, 0},
    };

    while ((opt = getopt_long(argc, argv, "x:y:z:p", long_opt, &opt_id)) != -1) {
        switch (opt) {
            case 'x':
                printf("option1 given %s\n", optarg);
            break;
            case 'y':
                printf("option2 given %s\n", optarg);
            break;
            case 'z':
                printf("option3 given %s\n", optarg);
            break;
            case 'p':
                printf("%s [option1 (x)] "
                        "[option2 (y)] "
                        "[option3 (z)] "
                        "[usage (p)]\n",
                                argv[0]);
            break;
        }
    }

    return 0;
}
```

