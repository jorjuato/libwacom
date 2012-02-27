//////////////////////////////////////////////////////////////////
//  write_tablet.c	
//							


//
//  gcc -Wall write_tablet.c -L. -lwacom -lpthread -o write_tablet
//  
// Copyleft (L)		Noviembre 2007		Jorge Ibáñez  						
//
//////////////////////////////////////////////////////////////////


//Cabeceras de las librerias standar de C para io básica y llamadas al sistema
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <pthread.h>
#include "wacom.h"

	
#define X_RES		1600.00
#define Y_RES		1200.00
#define X_AXE_RANG	87200.00
#define Y_AXE_RANG	65600.00
#define X_FACTOR	( (X_RES) / (X_AXE_RANG) )
#define Y_FACTOR	( (Y_RES) / (Y_AXE_RANG) )
#define PRESS_FACTOR 1024

FILE* data;
struct timeval tv, tv0, tvdiff;


int timeval_substract (struct timeval *result, struct timeval *x, struct timeval *y) {
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait. tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

void wacom_callback(int i) {	
	gettimeofday(&tv,NULL);
	timeval_substract(&tvdiff,&tv,&tv0);
	fprintf(data,"\t %d \t %d \t %d \t %d \t %ld \t %d \n", i,
			(int)(gAbsState[WACOMFIELD_POSITION_X].nValue * X_FACTOR),
			(int)(gAbsState[WACOMFIELD_POSITION_Y].nValue * Y_FACTOR),
			(int)(gAbsState[WACOMFIELD_PRESSURE].nValue),
			tvdiff.tv_sec, (int)(tvdiff.tv_usec / 1000));
	//fflush(stdout);
}

int main(void){
	// Initialize library and timer
	pthread_t thread = WacomInit(&wacom_callback);
	gettimeofday(&tv0,NULL);
	data = fopen("./datafile", "w");
	fprintf(data, "\tX \tY \tPress \tSec \tms \n");
	
	// Wait till thread ends
	pthread_join(thread, NULL);
	
	// Free resources and quit
	WacomClose();
	fclose(data);
	exit(0);
}
