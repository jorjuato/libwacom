//////////////////////////////////////////////////////////////////
//  wacomthread.c
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

#include "wacom_public.h"
#include "wacom_private.h"

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
void (*gCallback)(int) = NULL;

/*****************************************************************************
** Custom Public API Implementation
*****************************************************************************/

//------------------------------------------------------------  WacomInit()
//
pthread_t WacomInit(void (*WacomCallback)(int)) {
	if ( !InitAll() ) {
		perror("Init Wacom Engine");
		WacomCloseTablet(ghTablet);
		WacomTermEngine(ghEngine);
		exit(0);
	}
	if (WacomCallback)
		gCallback = WacomCallback;
		
	if ( pthread_create(&g_thread_id,NULL,WacomThread,NULL) )	{
			perror("Setting wacom scanning mode failed");
			WacomCloseTablet(ghTablet);
			WacomTermEngine(ghEngine);
			exit(0);
	}
	return g_thread_id;
}

//------------------------------------------------------------  WacomClose()
//
void WacomClose() {
	pthread_kill(g_thread_id, SIGTERM);
	sleep(1);
	ioctl(WacomGetFileDescriptor(ghTablet),EVIOCGRAB, (void *)0);
	WacomCloseTablet(ghTablet);
	WacomTermEngine(ghEngine);
}

/*****************************************************************************
** Custom Private API Implementation
*****************************************************************************/

//------------------------------------------------------------  WacomThread()
//
static void* WacomThread() {
	struct sigaction sighandler;
	int fd, oflags;
	
	/* Define the signal handler for SIGIO signal */
	memset(&sighandler, 0, sizeof(sighandler));
	sighandler.sa_handler = &LeerTableta;
	sigaction(SIGIO, &sighandler, NULL);
	
	/* Prepare the device to generate asinchronous signal on i/o */
	fd = WacomGetFileDescriptor(ghTablet);
	fcntl(fd, F_SETOWN, getpid(  ));
	oflags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, oflags | FASYNC);
	
	/* Stay waiting for signals...*/
	while(-pause()) {}
	
	return (void*) 0;
}

//------------------------------------------------------------  LeerTableta()
//
static void LeerTableta(int i) {

	static unsigned char uchBuf[64];
	static int nLength;
	static WACOMSTATE state = WACOMSTATE_INIT;

	//Lectura de un paquete
	if ( ( nLength = WacomReadRaw(ghTablet,uchBuf,sizeof(uchBuf)) ) < 0)
		return;

	//Interpretacion del paquete
	if (WacomParseData(ghTablet,uchBuf,nLength,&state))
		return;

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
	
	/* Call the user defined callback function */
	if (gCallback)
		gCallback(i);
}

//------------------------------------------------------------  InitAll()
//
static int InitAll() {
	WACOMMODEL model = { 0 };
	int fd;

	/* set device class, if provided */
	if (pszDeviceCls) {
		model.uClass = WacomGetClassFromName(pszDeviceCls);
		if (!model.uClass)	{
			fprintf(stderr, "Unrecognized class: %s\n",pszDeviceCls);
			exit(1);
		}
	}
	/* set device type, if provided */
	if (pszDeviceType)	{
		model.uDevice = WacomGetDeviceFromName(pszDeviceType,model.uClass);
		if (!model.uDevice)	{
			fprintf(stderr, "Unrecognized device: %s\n",pszDeviceType);
			exit(1);
		}
	}
	/* open engine */
	if (!(ghEngine = WacomInitEngine()) ){
		perror("failed to open tablet engine");
		return(0);
	}
	/* open tablet */
	if ( !(ghTablet = WacomOpenTablet(ghEngine,pszFile,&model)) )	{
		perror("WacomOpenTablet");
		WacomTermEngine(ghEngine);
		return(0);
	}
	/* get device capabilities */
	if (!SetTablet(ghTablet)){
		perror("InitTablet");
		WacomCloseTablet(ghTablet);
		WacomTermEngine(ghEngine);
		return(0);
	}
	/* get file descriptor of tablet to change access properties  */
	if ( !(fd = WacomGetFileDescriptor(ghTablet)) )	{
		perror("GetFileDescriptor");
		WacomCloseTablet(ghTablet);
		WacomTermEngine(ghEngine);
		return(0);
	}
	/* Grab tablet to avoid X server interaction */
	if (ioctl(fd,EVIOCGRAB, (void *)1) == -1)	{
		perror("Couln't ioctl grab Wacom device");
		WacomCloseTablet(ghTablet);
		WacomTermEngine(ghEngine);
		return(0);
	}
	return 1;
}

//------------------------------------------------------------  SetTablet()
//
static  int SetTablet() {
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

	/* get absolute state events*/
	for (i=WACOMFIELD_TOOLTYPE; i<WACOMFIELD_MAX; ++i)	{
		if (nCaps & (1<<i))	{
			gAbsState[i].bValid = 1;
			gAbsState[i].nValue = ranges.values[i].nValue;
			gAbsState[i].nMin = ranges.values[i].nMin;
			gAbsState[i].nMax = ranges.values[i].nMax;
		}
	}

	/* get key event types */
	for (i=0; i<WACOMBUTTON_MAX; ++i)
		gKeyState[i].bValid = 1;

	return 1;
}
