# Ninth Chapter

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

```
git clone https://github.com/DevNaga/gists.git
```

#### check the status of the branch

```
git status
```

#### configure the git username

```
git config --add user.name "devnaga"
```

#### confiure the git user email address

```
git config --add user.email "devendra.aaru@gmail.com"
```

#### configure the color screen for branch and status

```
git config --add color.diff auto
git config --add color.branch auto
git config --add color.status auto
```

#### configure the difftool

```
git config --add diff.tool meld
```

#### configure the favourite editor for the commit

```
git config --add core.editor subl
```

#### pull the code in the repository:

```
git pull
```

#### launch a difftool

```
git difftool
```

#### create a branch:

```
git branch new
```

#### delete a branch:

```
git branch -D new
```

#### stash local changes

```
git stash
```

#### list the stashed items

```
git stash list
```

#### show the stashed change

```
git stash show -p stash@{0}
```

where `0` is the number of the stash.

#### diff the local changes

```
git diff
```

#### counters of the diff

```
git diff --stat
```

#### view the changes in the staging

```
git diff --cached
```

#### view the counters of the changes in the staging

```
git diff --stat --cached
```

#### commit the changes

```
git commit
```

#### push the changes

```
git push origin <branch-name>
```

#### merge a branch

```
git merge origin/<branch-name>
```

#### rebase the branch

```
git rebase -i
```

where i is for the interactive rebase.


## Git clients
1. https://www.gitkraken.com/
2. https://desktop.github.com/

More on the git is at the below links:

### Links:

https://git-scm.com/book/en/v2/Getting-Started-Git-Basics

