#
#
#	makefile for zergmapper project
#
#

CFLAGS = -Wall -Wextra -Wpedantic -Wwrite-strings -Wstack-usage=1024 -Wfloat-equal -Waggregate-return -Winline

all: zergmap

zergmap: zergmapper.c decodelib.a lgraph.o haversine.o zerg_connect.o decode.h splay.h lgraph.h haversine.h zerg_connect.h
	gcc $(CFLAGS) -o zergmap zergmapper.c decodelib.a lgraph.o haversine.o zerg_connect.o -lm
	
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
	
debug: $(CFLAGS) += g
debug: all

profile: $(CFLAGS) += pg
profile: all
	
clean:
	rm -f *.o *.a zergmap
	

