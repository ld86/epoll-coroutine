CXXFLAGS=-std=c++11
LDFLAGS=-lboost_context
CC=${CXX}
all: main
main: main.o io_stuff.o
