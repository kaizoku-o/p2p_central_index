# This is a GNU Makefile.

# Creating a static library
TARGET = sock

# Libraries to use, objects to compile
SRCS = sock.cpp
SRCS_FILES = $(foreach F, $(SRCS), ./$(F))
CXX_FLAGS = -pthread --std=c++11

# Make it all!
all : sock.o
	g++ $(CXX_FLAGS) sock.o -o $(TARGET)

sock.o: sock.cpp
	g++ $(CXX_FLAGS) sock.cpp -c

# Standard make targets
clean :
	@rm -f *.o $(TARGET)
