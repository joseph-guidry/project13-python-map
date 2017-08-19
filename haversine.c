

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define R 6371
#define TO_RAD (3.1415926536 / 180)


/* Found on rosettacode.org/wiki/Haversine_formula#C */
double haversine_dist(double lat1, double long1, double alt1, double lat2, double long2, double alt2)
{
	double results;
	/*
	printf("lat1 %f \n", lat1);
	printf("lat2 %f \n", long1);
	printf("lat2 %f \n", lat2);
	printf("long2 %f \n", long2);
	*/
	
	double dx, dy, dz;
	long1 -= long2;
	long1 *= TO_RAD, lat1 *= TO_RAD, lat2 *= TO_RAD;
	
	/*
	printf("lat1 %f \n", lat1);
	printf("lat2 %f \n", long1);
	printf("lat2 %f \n", lat2);
	printf("long2 %f \n", long2);
	*/
	
	dz = sin(lat1) - sin(lat2);
	dx = cos(long1) * cos(lat1) - cos(lat2);
	dy = sin(long1) * cos(lat1);
	/*
	printf("dz = %f \n", dz);
	printf("dx = %f \n", dx);
	printf("dy = %f \n", dy);
	*/
	double height = alt1 - alt2;
	double distance = asin(sqrt((dx * dx) + (dy * dy) + (dz * dz)) / 2) * 2 * R;
	return sqrt((distance * distance) + (height * height));
}	


/*

usage statement:  double d = dist(double, double, double, double);

*/

