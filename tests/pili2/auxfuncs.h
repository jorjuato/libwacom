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




//void ExperimentSaveData(int time);

int ExperimentSelect();

void ExperimentFiles();

void itoa(int value, char* str, int base);

void strreverse(char* begin, char* end);

