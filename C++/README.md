C++ API Code
================

It is recommended that you compile any projects using the iSENSE C++ API code
using either Linux or Mac OS X.

The API code works in windows but it is not as easy to setup and requires using
Visual Studios.

The C++ API uses the libcurl library for C / C++.
Below is the setup guide for various operating systems.

##Linux
You can get libcurl on Linux (Ubuntu/Debian) by running the following commands:

```
sudo apt-get update
sudo apt-get install curl libcurl4-gnutls-dev
```

If you do not install libcurl, you will see the following error:

```
g++ -c tests.cpp -Wall -Werror -pedantic -std=c++0x -lcurl
In file included from tests.cpp:1:0:
include/API.h:10:23: fatal error: curl/curl.h: No such file or directory
 #include <curl/curl.h>
                       ^
compilation terminated.
make: *** [tests.o] Error 1
```

In order to run the makefile included in the API directory, you will need to
install the Boost unit testing library. You can either find this library at the
Boost website or in a Debian based Linux distribution (like Ubuntu) just run the
following command:

(note: requires ~100MB of available storage space)

```
sudo apt-get update
sudo apt-get install libboost-test-dev
```

If you do not install the boost unit testing library, you will see the following
error:

```
g++ -c tests.cpp -Wall -Werror -pedantic -std=c++0x -lcurl
tests.cpp:13:36: fatal error: boost/test/unit_test.hpp: No such file or directory
 #include <boost/test/unit_test.hpp>
                                    ^
compilation terminated.
make: *** [tests.o] Error 1
```

When compiling with curl, be sure to include "-lcurl" at the end of the gcc/g++ command.

Example:

```
g++ GET_basic_curl.cpp -lcurl
```

You should also use the provided Makefile for running the C++ tests. You can
modify this Makefile to run your own programs, or see the iSENSE/Teaching
repository for examples on using the C++ API.

##MacOS X
MacOS users should have curl / libcurl installed by default.
You will however need to add "-lcurl" to the Other Linker Flags in Xcode or when using GCC.
This can be found by going to build settings within an Xcode project, and then clicking on "All"
and scrolling down until you see an option for Other Linker Flags. Here you can type "-lcurl" and
add the API files (api.cpp as well as the "include" directory) to the Xcode project.

For more help with Xcode, [see the following site.](http://docs.millennialmedia.com/iOS-SDK/iOSAddingLinkerFlag.html)

You can also check out the example project in the "Mac" directory in the
[iSENSE Teaching Github repo](https://github.com/isenseDev/Teaching) (ExampleCode / C++ / Projects / Mac).

You should also be able to use the provided Makefile for Linux.

##Windows
To compile in Windows, you will need to use Visual Studios Express 2013 (Professional / Community versions should also work).
See the "Windows" directory for more information. An example project is provided - however, you will need to change
the paths of the curl directory and the lib directory.

To download Visual Studios 2013, [go to the following website.](http://www.visualstudio.com/)

##Testing
Confirmed working in Windows 7, 8.1 (x64), Mac OS X 10.10.1 Yosemite and Ubuntu 14.04LTS (x64).


##NOTES:
The C++ API wrapper also uses a JSON library called picojson to serialize and parse JSON.

[The latest PicoJSON can be found here.](https://raw.githubusercontent.com/kazuho/picojson/master/picojson.h)

It is also included it in the iSENSE API repository as a submodule, so if you
git clone the API repository it will automatically git  clone the picojson repository.

Picojson is contained in one header file called "picojson.h".
