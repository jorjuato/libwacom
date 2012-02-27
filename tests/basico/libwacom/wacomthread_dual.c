//////////////////////////////////////////////////////////////////
//  wacomthread_dual.c
//
// Contiene codigo que genera un thread para leer la tableta de forma asíncrona
// por medio del sistema de llamadas ioctl y select. Así se consigue la lectura
// no bloqueante y que el thread no haga nada hasta la llegada de un paquete de
// la tableta. Este thread también configura, si así lo solicita el usuario, una
// retrollamada que será ejecutada cada vez que entre un paquete.
// En el archivo de cabeceras se explicita el prototipo de la función WacomInit()
// que es la encargada de controlar todo el proceso de inicialización y configuración
// de la tableta.
//
//
// Copyleft (L)		Febrero 2008		Jorge Ibáñez
//
//////////////////////////////////////////////////////////////////

#include "wacomthread.h"

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

pthread_t g_thread_id;

/*****************************************************************************
** Custom Public API Implementation
*****************************************************************************/


//------------------------------------------------------------  WacomInit()
//
void WacomInit(int mode, void (*WacomSignalHandler)(int)) {
	if ( !InitAll() )
	{
		perror("Init Wacom Engine");
		WacomCloseTablet(ghTablet);
		WacomTermEngine(ghEngine);
		exit(0);
	}
	else if ( pthread_create(&g_thread_id,NULL,WacomThread,NULL) )
	{
		perror("Setting wacom scanning mode failed");
		WacomCloseTablet(ghTablet);
		WacomTermEngine(ghEngine);
		exit(0);
	}
	else if(mode == WACOM_SCANMODE_CALLBACK)
	{
		if (!WacomSignalHandler)
			perror("Wacom Callback not specified, necesary for queried scanmode");
		else
		{
			struct sigaction sighandler;
			memset(&sighandler, 0, sizeof(sighandler));
			sighandler.sa_handler = WacomSignalHandler;
			sigaction(SIGIO, &sighandler, NULL);
		}
	}
	else if(mode != WACOM_SCANMODE_NOCALLBACK)
	{
		perror("Wacom Scanning mode specified unknown");
		WacomCloseTablet(ghTablet);
		WacomTermEngine(ghEngine);
		exit(0);
	}
}

//------------------------------------------------------------  WacomClose()
//
void WacomClose() {
	pthread_kill(g_thread_id, SIGTERM);
	sleep(1);
	ioctl(WacomGetFileDescriptor(ghTablet),EVIOCGRAB, (void *)0);
	WacomCloseTablet(ghTablet);
	WacomTermEngine(ghEngine);
	UnSetSignalHandler();
}

//------------------------------------------------------------  WacomChangeMode()
//
int WacomChangeMode(int mode, void (*WacomSignalHandler)(int)) {

	if(mode == WACOM_SCANMODE_CALLBACK)
	{
		if (!WacomSignalHandler)
			perror("Wacom Callback not specified, necesary for queried scanmode");
		else
		{
			struct sigaction sighandler;
			memset(&sighandler, 0, sizeof(sighandler));
			sighandler.sa_handler = WacomSignalHandler;
			if (!sigaction(SIGIO, &sighandler, NULL));
				return 0;
		}
	}
	else if(mode == WACOM_SCANMODE_NOCALLBACK)
	{
		if (!sigaction(SIGIO, NULL, NULL));
			return 0;
	}
	else
		return 0;

	return 1;
}

/*****************************************************************************
** Custom Private API Implementation
*****************************************************************************/
//------------------------------------------------------------  WacomThread()
//
static void* WacomThread() {
	fd_set rfds;
	int fd, res;
	//struct timeval tv;

	if ( !(fd = WacomGetFileDescriptor(ghTablet)) )
	{
		perror("GetFileDescriptor");
		WacomCloseTablet(ghTablet);
		WacomTermEngine(ghEngine);
		exit(0);
	}
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	while ( 1 )
	{
		/* Select wait up to one hundred miliseconds. */
    	//tv.tv_sec = 0;
    	//tv.tv_usec = 100000;	
		res = select(fd + 1, &rfds, NULL, NULL, NULL);

	    if ( -1 == res && EINTR == errno )
	      continue; 				//System call interrupted by a signal received during call.
	    else if ( -1 == res )
	      perror("select() failed");//Undefined Error on select system call 
		else
	    	LeerTableta();
	}
}

//------------------------------------------------------------  LeerTableta()
//
static int LeerTableta() {

	static unsigned char uchBuf[64];
	static int nLength;
	static WACOMSTATE state = WACOMSTATE_INIT;
	//static int i;

	//Lectura de un paquete
	if ( ( nLength = WacomReadRaw(ghTablet,uchBuf,sizeof(uchBuf)) ) < 0)
		return 0;

	//Interpretacion del paquete
	if (WacomParseData(ghTablet,uchBuf,nLength,&state))
		return 0;

	//Lectura secuencial de los distintos valores del estado actual de la tableta
	if (state.uValid & BIT(WACOMFIELD_PROXIMITY))
		gAbsState[WACOMFIELD_PROXIMITY].nValue = state.values[WACOMFIELD_PROXIMITY].nValue;
	if (state.uValid & BIT(WACOMFIELD_POSITION_X))
		gAbsState[WACOMFIELD_POSITION_X].nValue = state.values[WACOMFIELD_POSITION_X].nValue;
	if (state.uValid & BIT(WACOMFIELD_POSITION_Y))
		gAbsState[WACOMFIELD_POSITION_Y].nValue = state.values[WACOMFIELD_POSITION_Y].nValue;
	if (state.uValid & BIT(WACOMFIELD_PRESSURE))
		gAbsState[WACOMFIELD_PRESSURE].nValue = state.values[WACOMFIELD_PRESSURE].nValue;

/*	for (i=WACOMFIELD_TOOLTYPE; i<WACOMFIELD_MAX; ++i)
		if (state.uValid & BIT(i))
			gAbsState[i].nValue = state.values[i].nValue;
	} Para leer todos los campos*/
	return 1;
}

//------------------------------------------------------------  UnSetTimer()
//
static void UnSetSignalHandler() {
	sigset_t mask_set;

	sigfillset(&mask_set);
	sigprocmask(SIG_BLOCK,&mask_set,NULL);
	signal(SIGIO,SIG_IGN);
}


//------------------------------------------------------------  InitAll()
//
static int InitAll()
{
	WACOMMODEL model = { 0 };
	int fd;

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
	if (!(ghEngine = WacomInitEngine()) )
	{
		perror("failed to open tablet engine");
		return(0);
	}
	/* open tablet */
	if ( !(ghTablet = WacomOpenTablet(ghEngine,pszFile,&model)) )
	{
		perror("WacomOpenTablet");
		WacomTermEngine(ghEngine);
		return(0);
	}
	/* get device capabilities */
	if (!SetTablet(ghTablet))
	{
		perror("InitTablet");
		WacomCloseTablet(ghTablet);
		WacomTermEngine(ghEngine);
		return(0);
	}
	/* get file descriptor of tablet to change access properties  */
	if ( !(fd = WacomGetFileDescriptor(ghTablet)) )
	{
		perror("GetFileDescriptor");
		WacomCloseTablet(ghTablet);
		WacomTermEngine(ghEngine);
		return(0);
	}
	/* Grab tablet to avoid X server interaction */
	if (ioctl(fd,EVIOCGRAB, (void *)1) == -1)
	{
		perror("Couln't ioctl grab Wacom device");
		WacomCloseTablet(ghTablet);
		WacomTermEngine(ghEngine);
		return(0);
	}
	return 1;
}

//------------------------------------------------------------  SetTablet()
//
static  int SetTablet()
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
	return 1;
}
