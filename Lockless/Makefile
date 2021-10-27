CC=g++
CXXFLAGS=-Wall -Wextra -pedantic -std=c++11 -pthread -O3 -Wl,-rpath,/opt/gcc/4.9.2/lib64 -fsanitize=thread
BUILDDIR=bin
TEST_SRCS=src/single_threaded_tests.cpp src/main.cpp

all: directory lockfree

directory:
	mkdir -p $(BUILDDIR)

LF_FILES = src/lock_free_bst.cpp
lockfree:
	$(CC) $(CXXFLAGS) -o $@ $(LF_FILES) $(TEST_SRCS) -D LOCKFREE
	mv $@ $(BUILDDIR)

lf: lockfree

clean:
	rm -rf $(BUILDDIR)
