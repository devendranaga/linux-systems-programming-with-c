# Appendix. F Shell


The `#!` is called **shebang**. the next followed by the shebang is the interpreter used. usually the interpreter used are the shells such as `/bin/sh` or `/bin/bash`.

printing on the shell is as easy as using `echo` command. 

The `echo` command prints text on the shell when given as

```
echo "hello"
```

prints "hello" on the shell.

Below is the shellscript. Download [here](https://github.com/DevNaga/gists/blob/master/echo.sh)

```shell
#!/bin/bash

echo "hello"

```

## conditional statements:

conditional statements such as `if` `elif` and `else` exist in shell.

```bash
if [ -f fork.c ] ; then
    echo "file present"
fi

```

## looping statements:

## switch:

## some of the useful commands:

Below are some of the useful commands when using the shell. 


| command | description |
|---------|-------------|
| `echo` | echo some text on the shell |
| `|` | pipe the output of one command to another command |
| `grep` | search for a keyword or text or pattern |
| `which` | locate the program in standard paths |
| `chmod` | change the file mode permissions |

