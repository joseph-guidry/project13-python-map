#
#
#	makefile for zergmapper project
#
#

CFLAGS = -Wall -Wextra -Wpedantic -Wwrite-strings -Wstack-usage=1024 -Wfloat-equal -Waggregate-return -Winline

all: zergmapper

zergmapper: zergmapper.c decodelib.a lgraph.o haversine.o min_heap.o zerg_connect.o decode.h splay.h lgraph.h haversine.h min_heap.h zerg_connect.h
	gcc $(CFLAGS) -o zergmapper zergmapper.c decodelib.a lgraph.o haversine.o zerg_connect.o min_heap.o -lm

min_heap.o: min_heap.c min_heap.h
	gcc -c min_heap.c -o min_heap.o
	
lgraph.o: lgraph.c lgraph.h
	gcc -c lgraph.c -o lgraph.o
	
haversine.o: haversine.c haversine.h
	gcc -c haversine.c -o haversine.o
	
zerg_connect.o: zerg_connect.c zerg_connect.h
	gcc -c zerg_connect.c -o zerg_connect.o

decodelib.a: decode.c splay.c buildData.c cmdPayload.c conversion.c gpsPayload.c msgPayload.c statusPayload.c decode.h splay.h
	gcc -c buildData.c cmdPayload.c conversion.c gpsPayload.c msgPayload.c statusPayload.c decode.c splay.c
	gcc -c decode.c -o decode.o 
	gcc -c splay.c -o splay.o
	ar r decodelib.a buildData.o cmdPayload.o conversion.o gpsPayload.o msgPayload.o statusPayload.o decode.o splay.o	
	
debug: zergmapper.c  decodelib.a lgraph.o haversine.o min_heap.o decode.h splay.h lgraph.h haversine.h min_heap.h
	gcc $(CFLAGS) -g -o zergmapper zergmapper.c decodelib.a lgraph.o haversine.o min_heap.o -lm

profile:

driver: graphdriver.c zerg_connect.o min_heap.o lgraph.o
	gcc $(CFLAGS) -o driver graphdriver.c lgraph.o min_heap.o zerg_connect.o

test:
	./decode pcaps/easy_3n0r/*
	
clean:
	rm -f *.o zergmapper decode
	

