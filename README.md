# ThreadPool-and-Asynchronous-log
An implementation of threadpool and asynchronous log, each head file (.h) with a source file (.cpp). Because of the simple dependencies between files，we just use source file for test, the implementations of classes are all in the head files.

a) To Compile source files into binary executables

g++ -std=c++11 xxx.cpp -o xxx -lpthread

b) Run the program

./xxx

c) Debug the progma should add -g

Note that the std::cout can all be removed, they are used to debug only.
