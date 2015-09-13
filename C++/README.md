C++ API Code
================

It is recommended that you compile any projects using the iSENSE C++ API code using either Linux or Mac OS X. The API code works in windows but it is not as easy to setup and requires using Visual Studios.

Some of these examples use cURL through the libcURL library for C. Below is the setup guide for various operating systems.

##Linux
You can get libcurl on Linux (Ubuntu/Debian) by running the following commands:

```
sudo apt-get update
sudo apt-get install curl libcurl4-gnutls-dev
```

In order to run the makefile included in the API directory, you will need to install the Boost unit testing library. You can either find this library at the Boost website or in a Debian based Linux distribution just run the following command:

(note: requires 100MB of available storage space)

```
sudo apt-get install libboost-test-dev
```

If those prerequisites are not installed, you may see the following error:

```
g++ -O0 -g -Wall -Wextra -pedantic-errors  -w -std=c++11 POST_simple.cpp -o simple.out -lcurl
POST_simple.cpp:1:65: fatal error: curl/curl.h: No such file or directory
#include <curl/curl.h>
```

When compiling with curl, be sure to include "-lcurl" at the end of the gcc/g++ command.

Example:

```
g++ GET_basic_curl.cpp -lcurl
```

##MacOS X
MacOS users should have curl / libcurl installed by default.
You will however need to add "-lcurl" to the Other Linker Flags in Xcode or when using GCC.
This can be found by going to build settings within an Xcode project, and then clicking on "All"
and scrolling down until you see an option for Other Linker Flags. Here you can type "-lcurl" and
add the API files (api.cpp as well as the "include" directory) to the Xcode project.

For more help with Xcode, [see the following site.](http://docs.millennialmedia.com/iOS-SDK/iOSAddingLinkerFlag.html)

You can also check out the example project in the "Mac" directory in the [Teaching Github repo](https://github.com/isenseDev/Teaching) (ExampleCode / C++ / Projects / Mac).

You should also be able to use the provided Makefile for Linux.

##Windows
To compile in Windows, you will need to use Visual Studios Express 2013 (Professional / Community versions should also work).
See the "Windows" directory for more information. An example project is provided - however, you will need to change
the paths of the curl directory and the lib directory.

To download Visual Studios 2013, [go to the following website.](http://www.visualstudio.com/)

##Testing
Confirmed working in Windows 7, 8.1 (x64), Mac OS X 10.10.1 Yosemite and Ubuntu 14.04LTS (x64).


##NOTES:
For some of these programs, a JSON library called picojson is used to serialize and parse JSON.
[You can grab the code for that off the following github repo.](https://github.com/kazuho/picojson)

It is also included it in the iSENSE Teaching GitHub Repository, inside the Projects directory in a
zip called "include.zip". Unzip that zip and you will have picojson, as well as memfile.h and API.h.

[The latest PicoJSON can be found here.](https://raw.githubusercontent.com/kazuho/picojson/master/picojson.h)

You can also just git clone the repository using the following command:

```
git clone https://github.com/kazuho/picojson.git
```

Picojson is contained in one header file called "picojson.h".