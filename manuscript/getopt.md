# getopt


*  parses arguments with options
*  example:

``` 
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
      gcc -Wall getopt_example.c -o getopt_example
      
      ./getopt_example -t 1
      
  The above code sleeps for 1 second and stops the execution

* include files : `<getopt.h>` and `<unistd.h>`.
* getopt defines a set of variable that can be used by the program.
  * **opterr**: If the value of this variable is nonzero, then getopt prints an error message to the standard error stream if it encounters an unknown option character or an option with a missing required argument. If you set this variable to zero, getopt does not print any messages, but it still returns the character ? to indicate an error.
    * For ex: if we run the above example as the following `./getopt_example -d `. It would print `./getopt_example: illegal option -- d` on to the screen.

  * **optopt**: When getopt encounters an unknown option character or an option with a missing required argument, it stores that option character in this variable. You can use this for providing your own diagnostic messages.

  * **optind**:  This variable is set by getopt to the index of the next element of the argv array to be processed. Once getopt has found all of the option arguments, you can use this variable to determine where the remaining non-option arguments begin. The initial value of this variable is 1.

  * **optarg**: This variable is set by getopt to point at the value of the option argument, for those options that accept arguments.

    * we get this option in our example of the getopt and use it to convert into an integer that gives us the time to sleep in the code. The `optarg` is the most commonly used argument in any basic to an intermediate level program.