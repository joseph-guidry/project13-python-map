#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#ifndef _ZERG_H_
#define _ZERG_H_

typedef union {						//Store floats and convert to binary
	double value;
	uint64_t value64;
} convertDouble;

typedef union {						//Store floats and convert to binary 
	float value;
	uint32_t value32;
} convertFloat;
/*
struct zerg
{
	uint16_t destID;
	char * zerg_name;				
	struct health
	{
		unsigned int hit_points;
		unsigned int max_points;
	};
	struct position 
	{
		convertDouble latitude;
		convertDouble longitude;
		convertFloat alititude;
	};
};
*/
#endif

