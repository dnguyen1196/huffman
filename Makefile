EXUCUTABLE = huffcode

CXX = g++
CXXFLAGS = -I. -I/usr/include/
FLAGS = -g -O2 -Wall -Wextra -Wfatal-errors -Werror -std=c++14 -pedantic


# -Wfatal-errors -Werror
############### Rules ###############

all: ${EXUCUTABLE}

clean:
	rm -f ${EXUCUTABLE} *.o
## Compile step (.c files -> .o files)

huffcode: main.o Hcompressor.o HuffmanTree.o Hdecompressor.o bitpack.h
	${CXX} ${FLAGS} ${CXXFLAGS} $^ -o $@

%.o: %.cpp
	${CXX} ${FLAGS} ${CXXFLAGS} -c $<