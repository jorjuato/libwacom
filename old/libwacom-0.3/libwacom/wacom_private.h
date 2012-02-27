//////////////////////////////////////////////////////////////////
//  wacomthread.h			
//							
// Contiene codigo que genera un thread para leer la tableta y configura un temporizador 
// bassado en señal de alarma del sistema de señales POSIX de Linux. 
// OpenGL. Esta version utiliza como temporizador el sistema de señales nativo 
// de Linux. Se basa en wacomglut2.c, que a su vez se basa en wacomnoglut.c, habiendole
// añadido las rutinas minimas para la presentación de graficos OpenGL.
//
// gcc -Wall  -lpthread -O3 wacomglut2.c wacusb.c wacserial.c wactablet.c  -o wacomglut2
//  
// Copyleft (L)		Noviembre 2007		Jorge Ibáñez  						
//
//////////////////////////////////////////////////////////////////


// from linux/input.h 
#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define BIT(x)  (1UL<<((x)%BITS_PER_LONG))
#define LONG(x) ((x)/BITS_PER_LONG)
#define ISBITSET(x,y) ((x)[LONG(y)] & BIT(y)) 

//Cabeceras de las librerias standar de C para io básica y llamadas al sistema
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     /* standard unix functions  */
#include <sys/types.h>  /* various type definitions */
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <linux/input.h>

//Cabeceras con definiciones del API Wacom
#include "wactablet.h"


//#include <sys/select.h>
//#include <sys/time.h>
//#include <sys/stat.h>

//#include <time.h>



/*****************************************************************************
**  Wacom Custom Private API
*****************************************************************************/

static int InitAll(); 

static int SetTablet();

static void LeerTableta(int i);
	
static void* WacomThread();


	

