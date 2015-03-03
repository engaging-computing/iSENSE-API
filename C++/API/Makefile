# Makefile for the iSENSE projects involving C++
# Designed to quickly compile the C++ code (only if files are changed)
CC = g++

# NOTES: -lcurl is required. std=c++11 is also needed for to_string. -ansci
CFLAGS = -Wall -Werror -pedantic -std=c++11 -lcurl

# In the future, should try compiling with these flags (change to -ansi)
# CFLAGS= -Wall -Werror -ansi -pedantic
Boost= -lboost_unit_test_framework

# Makes all of the C++ projects, appends a ".out" for easy removal in make clean
all: 	tests.out

# Unit tests for the iSENSE code.
tests.out:	tests.o API.o
	$(CC) tests.o API.o -o tests.out $(CFLAGS) $(Boost)

tests.o: tests.cpp include/API.h include/memfile.h include/picojson.h
	$(CC) -c tests.cpp $(CFLAGS)

# API code
API.o:	API.cpp include/API.h include/memfile.h include/picojson.h
	$(CC) -c API.cpp $(CFLAGS)

clean:
	rm *.out
	rm *.o
