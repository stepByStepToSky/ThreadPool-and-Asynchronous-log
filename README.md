## ThreadPool and Asynchronous log

## Introduction
An implementation of threadpool and asynchronous log. We use cmake to compile these files. Test files are all in the test folder.


a) To Compile source files into binary executables (in the root folder)
```cmd
mkdir build && cd build
cmake ..
make
```

b) Run the program (in the build folder)
```cmd
./xxx
```

## Example
```c++
//main.cpp
#include <iostream>
#include "log.h"

int main()
{
	ERRLOG("xiao cai");
	DEBUGLOG("xiao fan");
	
	ERRLOG("xiaocai %d%d%d%d", 0, 8, 1, 2);
	DEBUGLOG("xiaofan %d%d%d%d", 0, 9, 1, 4);
}
```
```cmd
g++ -g -std=c++11 main.cpp -o main ./libasynclogStaticLib.a  -lpthread
./main
```

## Log doc
[Asynchronous Log Readme](https://github.com/stepByStepToSky/ThreadPool-and-Asynchronous-log/blob/master/Log%20doc.md)


- Note that the async log can be used in multi process environment, however, they write into difference log files. And std::cout can all be removed, they are used to debug only.
