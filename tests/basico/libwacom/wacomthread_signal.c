//////////////////////////////////////////////////////////////////
//  wacomthread.c			
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

#define GAP_WACOM	5000	// Sampling rate in microseconds

//Cabeceras de las librerias standar de C para io básica y llamadas al sistema
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     /* standard unix functions  */
#include <sys/types.h>  /* various type definitions */

//Cabeceras con definiciones para el sistema de timing en linux
#include <time.h>
#include <sys/time.h>

//Cabeceras con definiciones para el sistema de señales en linux
#include <signal.h>

//Cabeceras con definiciones para creacion de hilos
#include <pthread.h>

//Cabeceras con definiciones del API Wacom
#include "wactablet.h"

/*****************************************************************************
** Variables globales
*****************************************************************************/
const char* pszFile = "/dev/input/wacom";
const char* pszDeviceCls = "usb";
const char* pszDeviceType = NULL;

WACOMENGINE ghEngine;
WACOMTABLET ghTablet;

struct ABS_STATE gAbsState[WACOMFIELD_MAX];
struct KEY_STATE gKeyState[WACOMBUTTON_MAX];

pthread_t g_wacomthread;

/*****************************************************************************
** Custom API Implementation
*****************************************************************************/


//------------------------------------------------------------  WacomInit()
//
void WacomInit()
{
	int wacreturn=0;
	InitAll();
	wacreturn = pthread_create(&g_wacomthread,NULL,WacomThread,NULL);
    //pthread_join(g_wacomthread,NULL); Con esta funcion peta el temilla
}

//------------------------------------------------------------  WacomClose()
//
void WacomClose()
{
	pthread_kill(g_wacomthread, SIGKILL);
	WacomCloseTablet(ghTablet);
	WacomTermEngine(ghEngine);
}

//------------------------------------------------------------  WacomThread()
//
void *WacomThread()
{
	SetTimer();
	while(1)  {}
}


//------------------------------------------------------------  SetTimer()
//
int SetTimer()
{
	/* define a new mask set */
	sigset_t mask_set;
	
	/* first clear the set (i.e. make it contain no signal numbers);*/
	//sigemptyset(&mask_set)
	sigfillset(&mask_set);
	
	/* lets del the ALRM signal from our mask set */
	//sigaddset(&mask_set, SIGINT);
	sigdelset(&mask_set, SIGALRM);

	pthread_sigmask(SIG_BLOCK,&mask_set,NULL);  //Sets the process signal mask
	
	signal(SIGALRM,WacomTimer);			 //Sets the alarm handler
	
	/* defines a new timer interval and sets it*/
	
	struct itimerval timer, otimer;

	timer.it_interval.tv_usec=GAP_WACOM;
	timer.it_interval.tv_sec=0;
	timer.it_value.tv_usec=GAP_WACOM;
	timer.it_value.tv_sec=0;
	
	if(-setitimer(ITIMER_REAL,&timer,&otimer))
		return 0;
	else
		return 1;
}

//------------------------------------------------------------  UnSetTimer()
//
void UnSetTimer() 
{
	sigset_t mask_set;
	
	sigfillset(&mask_set);
	sigprocmask(SIG_BLOCK,&mask_set,NULL);
	signal(SIGALRM,SIG_IGN);
}


//------------------------------------------------------------  WacomTimer()
//
void WacomTimer(int value) {

	sigset_t mask_set;  /* used to set a signal masking set. */
    sigset_t old_set;   /* used to store the old mask set.   */
	//struct timeval tv;  
		
	/* re-set the signal handler again to catch_int, for next time */
    signal(SIGALRM, WacomTimer); 
    
    /* mask any further signals while we're inside the handler. */
    sigfillset(&mask_set);
    pthread_sigmask(SIG_SETMASK, &mask_set, &old_set);
    
   	while(!LeerTableta()) {}
   	
   	       
}



//------------------------------------------------------------  LeerTableta()
//
int LeerTableta() {

	unsigned char uchBuf[64];
	int i, nLength;//, nErrors=0;
	WACOMSTATE state = WACOMSTATE_INIT;

	//while(1)
	//{
		//Lectura de un paquete de datos binarios desde el puerto USB
		// getchar() != 'q'
		if ( ( nLength = WacomReadRaw(ghTablet,uchBuf,sizeof(uchBuf)) ) < 0)
		{
			// ERROR DE LECTURA DEL PAQUETE
			//nErrors++;
			return 0;
		}
		//Interpretacion del paquete de datos en un estado de la tableta: state
		if (WacomParseData(ghTablet,uchBuf,nLength,&state))
		{
			// ERROR DE INTERPRETACION DEL PAQUETE
			//nErrors++;
			return 0;
		}

		//Lectura secuencial de los distintos valores del estado actual de la tableta
		for (i=WACOMFIELD_TOOLTYPE; i<WACOMFIELD_MAX; ++i)
		{
			if (state.uValid & BIT(i))
			{
				if (i == WACOMFIELD_RELWHEEL)
					gAbsState[i].nValue += state.values[i].nValue;
				else
					gAbsState[i].nValue = state.values[i].nValue;
			}
		}
	/*
		//Lectura secuencial de los distintos botones
		for (i=WACOMBUTTON_LEFT; i<WACOMBUTTON_MAX; ++i)
		{
			gKeyState[i].nValue = state.values[WACOMFIELD_BUTTONS].nValue &
					(1 << i) ? 1 : 0;

		}
	*/	
	//}
	return 1;
}
	
//------------------------------------------------------------  InitAll()
//
int InitAll()
{
	WACOMMODEL model = { 0 };

	/* set device class, if provided */
	if (pszDeviceCls)
	{
		model.uClass = WacomGetClassFromName(pszDeviceCls);
		if (!model.uClass)
		{
			fprintf(stderr, "Unrecognized class: %s\n",pszDeviceCls);
			exit(1);
		}
	}

	/* set device type, if provided */
	if (pszDeviceType)
	{
		model.uDevice = WacomGetDeviceFromName(pszDeviceType,model.uClass);
		if (!model.uDevice)
		{
			fprintf(stderr, "Unrecognized device: %s\n",pszDeviceType);
			exit(1);
		}
	}

	/* open engine */
	ghEngine = WacomInitEngine();
	if (!ghEngine)
	{
		perror("failed to open tablet engine");
		exit(1);
	}

	/* open tablet */
	ghTablet = WacomOpenTablet(ghEngine,pszFile,&model);
	if (!ghTablet)
	{
		perror("WacomOpenTablet");
		exit(1);
	}

	/* get device capabilities */
	if (SetTablet(ghTablet))
	{
		perror("InitTablet");
		WacomCloseTablet(ghTablet);
		exit(1);
	}
	return 0;
}

//------------------------------------------------------------  SetTablet()
//
int SetTablet()
{
	int i, nCaps;
	int nMajor, nMinor, nRelease;

	WACOMMODEL model;
	WACOMSTATE ranges = WACOMSTATE_INIT;
	
	const char* pszName;
	const char* pszClass = "UNK_CLS";
	const char* pszVendor = "UNK_VNDR";
	const char* pszDevice = "UNK_DEV";
	const char* pszSub = "UNK_SUB";

	/* get model */
	model = WacomGetModel(ghTablet);
	pszVendor = WacomGetVendorName(ghTablet);
	pszClass = WacomGetClassName(ghTablet);
	pszDevice = WacomGetDeviceName(ghTablet);
	pszSub = WacomGetSubTypeName(ghTablet);
	pszName = WacomGetModelName(ghTablet);
	WacomGetROMVersion(ghTablet,&nMajor,&nMinor,&nRelease);

	/* get event types supported, ranges, and immediate values */
	nCaps = WacomGetCapabilities(ghTablet);
	WacomGetState(ghTablet,&ranges);

	for (i=WACOMFIELD_TOOLTYPE; i<WACOMFIELD_MAX; ++i)
	{
		if (nCaps & (1<<i))
		{
			gAbsState[i].bValid = 1;
			gAbsState[i].nValue = ranges.values[i].nValue;
			gAbsState[i].nMin = ranges.values[i].nMin;
			gAbsState[i].nMax = ranges.values[i].nMax;
		}
	}

	/* get key event types */
	for (i=0; i<WACOMBUTTON_MAX; ++i)
	{
		gKeyState[i].bValid = 1;
	}
	return 0;
}
