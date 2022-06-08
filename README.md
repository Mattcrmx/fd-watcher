# fd-watcher
A watchdog program that allows to monitor the number of file descriptors open by a process. At the moment it only accepts keyworded options and no arguments but it's likely subject to changes.

## Compiling the code
Simply run the Makefile:
```bash
make
```

The usage is as follows :
```bash
./fd-watcher --pid=42069
```
or 

```bash
./fd-watcher --name=mylittleprocess
```

Options can be specified:
```bash
-p --pid: the pid of the process to monitor
-n --name: the name of the process to monitor
-t --time: the time limit of the watchdog
-i --interval: the interval at which the watchdog will watch the descriptors
```
