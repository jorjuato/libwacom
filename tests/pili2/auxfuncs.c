/************************************************************************##
** 	auxfuncs.h										 			 		 ##
**																		 ##
**			Jorge Ibáñez				Diciembre 2006					 ##
**																		 ##
**																		 ##
**																		 ##
**  Conjunto de funciones auxiliares para el programa principal del 	 ##
**  experimento. Controlan entrada y salida de ficheros y cuestiones	 ##
**  secundarias del algoritmo que complican la lectura de main.c		 ##
**																		 ##
**************************************************************************/

//Cabeceras de la libreria SDL
#ifndef __INCLUDESDL
#include "SDL/SDL.h"
#define __INCLUDESDL
#endif

//Cabeceras de la libreria Wacom
#ifndef __INCLUDEWACOM
#include "wacom.h"
#define __INCLUDEWACOM
#endif

//Cabeceras estandar del SO
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
//#include <unistd.h>
//#include <math.h>
//#include <sys/time.h>


//Cabeceras principales del experimento
#include "display.h"
#include "pili.h"
#include "auxfuncs.h"

//////////////////////////////////////////////////// INPUT/OUTPUT GLOBALS
const char*	g_subject_name;
const char*	g_data_base="./data";
char		g_data_path[128];
char		g_global_path[128];

//////////////////////////////////////////////////// CONFIGURATION GLOBAL STRUCTURES (main.c)
extern CONFIG 		g_config;
extern CONFIG*		G_Config[MAX_CONF_NUMBER];
extern FILE*		g_GlobalFile;
extern FILE* 		g_DataFile;
extern int 			g_config_number;

//////////////////////////////////////////////////// EXPERIMENT GLOBAL STATE VARIABLES (main.c)
extern const char*	g_bHitTarget1;
extern const char*	g_bHitTarget2;
extern int 			g_trial_number;
extern int 			g_trial_type;


//------------------------------------------------------------  ExperimentFiles()
//
void ExperimentFiles(){
	char directory[128];
	char globalfilename[256];
	char datafilename[128];
	int i;

	//Generamos el  nombre del directorio donde guardaremos los datos.
	strcpy(directory,g_data_base);
	strcat(directory,"/");
	strcat(directory,g_subject_name);
	mkdir(directory, 0777);
	strcat(directory,"/");

	//Ahora, generamos los nombres de los ficheros
	strcpy(globalfilename,directory);
	strcat(globalfilename,g_subject_name);
	strcat(globalfilename,"_resultados.dat");

	strcpy(datafilename,directory);
	strcat(datafilename,g_subject_name);
	strcat(datafilename,"_");

	//Guardamos el path de los archivos de datos y creamos archivo global
	strcpy(g_data_path,datafilename);
	strcpy(g_global_path,globalfilename);

	g_GlobalFile = fopen(g_global_path,"w");
	//printf("Abriendo fd %d a partir del archivo %s\n",fileno(g_GlobalFile),g_global_path);
	//fflush(stdout);

	//Ahora, pintamos la cabecera del archivo de resultados
	fprintf( g_GlobalFile, "TrNo\tTrTy\thit1\thit2\tx_start\t  y_start\t  r_start\t  x1_target1\t  y1_target1\t  x2_target1\t  y2_target1\t  h_target1\t   time_of_jump\t  time_limit\t ");
	for (i=0;i<g_config_number;i++)
		fprintf( g_GlobalFile,"Left_%d ", i);
	fprintf( g_GlobalFile,"\n");
	fflush(g_GlobalFile);
	printf("Escrita cabecera en archivo global\n");
}

//------------------------------------------------------------  ExperimentSelect()
//
int ExperimentSelect(){
	double proporciones[g_config_number+1],rnd_num;
	int i 					= 0,
		acumulated_trials 	= 0,
		total_trials 		= 0,
		charlen 			= 0;
	char trial_number[8];
	char filename[128];

	//Inicializamos la semilla del generador de numeros aleatorios
	srand(time(0));

	//Sumamos el numero total de trials restantes
	for (i=0;i<g_config_number;i++)
	{
		total_trials += (*G_Config[i]).replications;
		proporciones[i] = 0;
	}
	//Comprobamos que quedan trials por realizar
	printf("Number of trials left: %d\n", total_trials);
	fflush(stdout);
	if (total_trials == 0)
		return FALSE;
	//Obtenemos las proporciones de cada trial_type
	for (i=1;i<g_config_number+1;i++)
	{
		acumulated_trials += (*G_Config[i-1]).replications;
		proporciones[i] = (float) acumulated_trials / (float)total_trials; //(float)
		printf("Trial proportions: %f\n", proporciones[i]);
		fflush(stdout);
	}
	//Ahora, elegimos aleatoriamente el tipo de trial
	g_trial_type = -1;
	do
	{
		rnd_num = (float)rand() / (float)(RAND_MAX+1.0);
		printf("el número aleatorio obtenido es...: %f\n", rnd_num);
		for (i=0;i<g_config_number;i++)
			if( rnd_num > proporciones[i] && rnd_num<proporciones[i+1])
				if ((*G_Config[i]).replications>0)
					g_trial_type = i;
	} while(g_trial_type == -1);
	printf("el tipo de trial obtenido es...: %d\n", g_trial_type);
	(*G_Config[g_trial_type]).replications -= 1;
	g_trial_number++;
	itoa(g_trial_number,trial_number,10);
	charlen = strlen(trial_number);
	strcpy(filename,g_data_path);
	for (i=0;i<4-charlen;i++)
		strcat(filename,"0");
	strcat(filename,trial_number);
	strcat(filename,".dat");


	//Cerramos anterior archivo y reiniciamos puntero
	if (g_DataFile)
	{
		fflush(g_DataFile);
		fclose(g_DataFile);
	}
	g_DataFile = fopen(filename,"w");
	g_config = (*G_Config[g_trial_type]);
	fprintf( g_DataFile, "x_pos\t\t y_pos\t pressure \t time from beggining (ms)\n");
	return TRUE;
}
/*
//------------------------------------------------------------  ExperimentSaveConfig()
//
void ExperimentSaveConfig(CONFIG Config){
	int i;

	fprintf( g_GlobalFile,\
			"%d %d %s %s %f %f %f %f   %f %f %f %f %f    %f %f %f %f  %f %f ",\
			 g_trial_number, g_trial_type,\
			 g_bHitTarget1, g_bHitTarget2,\
			 Config.x_start, Config.y_start, Config.r_start,\
			 Config.x1_target1,Config.y1_target1,Config.x2_target1,Config.y2_target1,Config.h_target1,\
			 Config.x1_target2,Config.y1_target2,Config.x2_target2,Config.y2_target2,Config.h_target2,\
			 Config.time_of_jump,Config.time_limit);
	for (i=0;i<g_config_number;i++)
		fprintf( g_GlobalFile,"%d ", (*G_Config[i]).replications);
	fprintf( g_GlobalFile,"\n");
	fflush(g_GlobalFile);
}


//------------------------------------------------------------  ExperimentSaveData()
//
void ExperimentSaveData(int time)
{
fprintf(g_DataFile, "%f\t%f\t%d\t%d\n",		\
		WAC_SCALE_X(gAbsState[WACOMFIELD_POSITION_X].nValue),	\
		WAC_SCALE_Y(gAbsState[WACOMFIELD_POSITION_Y].nValue),	\
		gAbsState[WACOMFIELD_PRESSURE].nValue,		\
		time);
}

*/

/*

 * Ansi C "itoa" based on Kernighan & Ritchie's "Ansi C":

 */

void strreverse(char* begin, char* end)
{
	char aux;
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
}

void itoa(int value, char* str, int base) {

	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* wstr=str;
	int sign;

	// Validate base
	if (base<2 || base>35){ *wstr='\0'; return; }

	// Take care of sign
	if ((sign=value) < 0) value = -value;

	// Conversion. Number is reversed.
	do *wstr++ = num[value%base]; while(value/=base);
	if(sign<0) *wstr++='-';
	*wstr='\0';

	// Reverse string

	strreverse(str,wstr-1);

}
