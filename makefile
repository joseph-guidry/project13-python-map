#
#
#	makefile for zergmapper project
#
#

CFLAGS = -Wall -Wextra -Wpedantic -Wwrite-strings -Wstack-usage=1024 -Wfloat-equal -Waggregate-return -Winline

all: zergmapper

zergmapper: decode.o zergmapper.c decode.h splay.h
	gcc $(CFLAGS) -o zergmapper zergmapper.c decode.o 
	
	
zerg.o:zerg.c zerg.h
	gcc -c zerg.c -o zerg.o
	
#decode.o: decode.a decode.h
#	gcc -c buildData.c cmdPayload.c conversion.c gpsPayload.c msgPayload.c statusPayload.c
#	ar  r zerglib.a buildData.o cmdPayload.o conversion.o gpsPayload.o msgPayload.o statusPayload.o
	
splay.o: splay.c splay.h
	gcc -c splay.c -o splay.o
	
debug:

profile:

test:
	./decode pcaps/easy_3n0r/*
	
clean:
	rm -f *.o zergmapper decode
	
decode.o: decode.c decode.h splay.h buildData.c cmdPayload.c conversion.c gpsPayload.c msgPayload.c statusPayload.c
	gcc -c buildData.c cmdPayload.c conversion.c gpsPayload.c msgPayload.c statusPayload.c
	gcc -c splay.c -o splay.o
	gcc -Wall -Wextra -Wpedantic -Wwrite-strings -Wstack-usage=1024 -Wfloat-equal -Waggregate-return -Winline -o decode.o decode.c buildData.o cmdPayload.o conversion.o gpsPayload.o msgPayload.o statusPayload.o splay.o -lm

