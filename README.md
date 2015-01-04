qre -- Quick Regular Expression
===============================
A regular expression engine which supports compiling regular expressions
into virtual machine bytecode for pattern matching at fast speed.

Features
--------
1. Plain Text
2. Branch
3. Group
4. Capture by index
5. Capture by lable
6. Unicode Internal & UTF-8 Support


Building
--------
Python2 is required to generate the Makefile. 
(Python3 is not supported yet)

Use the following command to generate the Makefile:
```
$ ./configure
```

And the following commands to build the project:
```
$ make # for building the demo program
$ make static # for building the static library
$ make shared # for building the shared (dynamic) library
```


Usage
-----
```
Usage : qre <testname> <arguments>

analysis  <pattern>
match     <pattern> <haystack>
```


License
-------
BSD-3

