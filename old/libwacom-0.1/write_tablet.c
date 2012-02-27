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
#include "wacom.h"

	
#define X_RES		1600.00
#define Y_RES		1200.00
#define X_AXE_RANG	87200.00
#define Y_AXE_RANG	65600.00
#define X_FACTOR	( (X_RES) / (X_AXE_RANG) )
#define Y_FACTOR	( (Y_RES) / (Y_AXE_RANG) )
#define PRESS_FACTOR 512

FILE* data;
struct timeval tv, tv0;

void wacom_callback(int i) {
	gettimeofday(&tv,NULL);
	//fprintf(data, "\t %d \t %d \t %d \t %ld \t %ld \n",
				//(int)(gAbsState[WACOMFIELD_POSITION_X].nValue * X_FACTOR),
				//(int)(gAbsState[WACOMFIELD_POSITION_Y].nValue * Y_FACTOR),
				//(int)(gAbsState[WACOMFIELD_PRESSURE].nValue),
				//tv.tv_sec, tv.tv_usec);
	printf("\t %d \t %d \t %d \t %ld \t %ld \n",
			(int)(gAbsState[WACOMFIELD_POSITION_X].nValue * X_FACTOR),
			(int)(gAbsState[WACOMFIELD_POSITION_Y].nValue * Y_FACTOR),
			(int)(gAbsState[WACOMFIELD_PRESSURE].nValue),
			tv.tv_sec, tv.tv_usec);
	//printf("que pasa\n");
	fflush(stdout);
}

int main(void){
	// Initialize all
	WacomInit(WACOM_SCANMODE_POLL, &wacom_callback);
	//data = fopen("./datafile", "w");
	//fprintf(data, "\tX \tY \tPress \tSec \tMicrosec \n");
	gettimeofday(&tv0,NULL);
	
	// Infinite loop
	while(1) {}
	
	// Free resources and quit
	//fclose(data);
	exit(0);
}
