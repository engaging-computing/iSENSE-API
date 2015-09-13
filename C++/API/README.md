C++ API Source
==========
API source code for iSENSE related projects in C++.

##Linux & Mac OS X command line setup

1. API.cpp: This file contains the API class functions. Look through it and read the
comments to understand what they do and how they can be used.

2. The include directory: This should contain API.h, memfile.h and a submodule (directory) named picojson.
The memfile.h and the picojson library are used along with libcurl to make HTTP GET / POST requests to iSENSE's
REST API. I suggest looking through API.h for the iSENSE class declaration.
It provides a simple overview - more detail can be found in the API.cpp file.

3. A main file: You can check out some of the example mains (GET_search.cpp, POST_email.cpp, etc) in the
[iSENSE Teaching Github repo](https://github.com/isenseDev/Teaching)
so that you can make your own program that uses the API.

4. A makefile: one is provided for testing. Simply add on to it to compile your program.

##Windows  & Mac OS X IDE Setup
See the example projects in the [iSENSE Teaching Github repo](https://github.com/isenseDev/Teaching).
(Teaching / ExampleCode / C++ / Projects)

##API Testing
To test the API, run the provided Makefile. You can use the following commands:

```
make
./tests.out > stdout.txt 2> stderr.txt
```

This will place all the standard IO into a text file called stdout.txt, and all
of the standard errors will end up in a text file called stderr.txt.
