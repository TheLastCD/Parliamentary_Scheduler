# Parlliamentary_Scheduler

## Overview
The Paliamentry Scheduler is a simple binary protocol and in future will be become:
* Mesh based hierarchical communication method designed for embedded devices
* Highly Fault tolerant and strongly typed
* Support for multiple RTOS's, but currently only designed to support linux.

# Building and Running
## build alias:

```
alias build="rm -rf build && cmake -B build && cmake --build build"
alias clean="rm -rf build"
alias test="cd build && ctest -V"
```
