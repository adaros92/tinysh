# tinysh
A basic unix shell

## Build and run
```
make all
./smallsh
```

## Examples
### Basics
```
: # this is comment
:
: # list directory contents
: ls
: # redirect to file
: ls > some_file.txt
:
: # go home
: cd 
: # travel somewhere
: cd [some directory]
:
: # print working directory
: pwd
: 
```
Supports most other basic commands (cp, mv, cat, etc.)

### Signals and background processes
```
: # run some process
: sleep 10
: # CTRL + C sends SIGINT signal to kill bg process
terminated by signal 2
:
: # run same process but in the background
: sleep 10 &
background pid is [SOME PID]
: # get running processes
: ps
: # kill some background process
: kill -15 [SOME PID]
background pid [SOME PID] is done: terminated by signal 15
:
: # CTRL + Z enters foreground only mode
Entering foreground-only mode (& is now ignored)
```
