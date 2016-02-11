CXX=gcc
CFLAGS=-Wall -O3 -g -std=c99
SOURCE=main.c
HEADER=main.h
TARGET=Prog

default: compile

diff:
	${CXX} ${CFLAGS} differential.c -o differential -lm
	

compile:
	${CXX} ${CFLAGS} ${SOURCE} ${HEADER} -o ${TARGET} -lm

debug:
	${CXX} ${CFLAGS} ${SOURCE} ${HEADER} -o ${TARGET} -DEBUG -lm

rmlogs:
	rm -rf ./logs/*

clean:
	rm -f ${TARGET} differential *.o *.png *.dat
