CXX=gcc
CFLAGS=-Wall -O3 -g -std=c99
SOURCE=
HEADER=
TARGET=

default: compile

compile: new.c
	${CXX} ${CFLAGS} ${SOURCE} ${HEADER} -o ${TARGET} -lm

debug: new.c
	${CXX} ${CFLAGS} ${SOURCE} ${HEADER} -o ${TARGET} -DEBUG -lm

clean:
	rm -f ${TARGET} *.o
