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
** Custom API Implementation
*****************************************************************************/


//------------------------------------------------------------  WacomInit()
//
void WacomInit(int samplerate) {
	if ( !InitAll() )	
		exit(0);
	if ( pthread_create(&g_thread_id,NULL,WacomThread,samplerate) ) 
		exit(0);
}

//------------------------------------------------------------  WacomClose()
//
void WacomClose() {
	int fd;
	
	pthread_kill(g_thread_id, SIGKILL);
	fd = WacomGetFileDescriptor(ghTablet);
	ioctl(fd,EVIOCGRAB, NULL);
	WacomCloseTablet(ghTablet);
	WacomTermEngine(ghEngine);
}

//------------------------------------------------------------  WacomThread()
//
void* WacomThread(int samplerate) {
	fd_set rfds; 
	int fd;
	
	if ( !(fd = WacomGetFileDescriptor(ghTablet)) )
	{
		perror("GetFileDescriptor");
		WacomCloseTablet(ghTablet);
		WacomTermEngine(ghEngine);
		exit(0);
	}
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	
	while ( 1 ) {
		if ( FD_ISSET(fd, &rfds) ) 
		{
    		if(!LeerTableta())
				continue;    
  		} 
	}
	/*while(1)
	{
		if(!LeerTableta())
			continue;
		else
			usleep(samplerate); 
	}*/
}


//------------------------------------------------------------  LeerTableta()
//
int LeerTableta() {

	unsigned char uchBuf[64];
	int nLength;
	WACOMSTATE state = WACOMSTATE_INIT;

	if ( ( nLength = WacomReadRaw(ghTablet,uchBuf,sizeof(uchBuf)) ) < 0) 		
		return 0;												//Lectura de un paquete 
	if (WacomParseData(ghTablet,uchBuf,nLength,&state)) 		
		return 0;												//Interpretacion del paquete 
	
	//Lectura secuencial de los distintos valores del estado actual de la tableta
	if (state.uValid & BIT(WACOMFIELD_PROXIMITY))
		gAbsState[WACOMFIELD_PROXIMITY].nValue = state.values[WACOMFIELD_PROXIMITY].nValue;
	if (state.uValid & BIT(WACOMFIELD_POSITION_X))
		gAbsState[WACOMFIELD_POSITION_X].nValue = state.values[WACOMFIELD_POSITION_X].nValue;
	if (state.uValid & BIT(WACOMFIELD_POSITION_Y))
		gAbsState[WACOMFIELD_POSITION_Y].nValue = state.values[WACOMFIELD_POSITION_Y].nValue;
	if (state.uValid & BIT(WACOMFIELD_PRESSURE))
		gAbsState[WACOMFIELD_PRESSURE].nValue = state.values[WACOMFIELD_PRESSURE].nValue;
	
	return 1;
}
	
//------------------------------------------------------------  InitAll()
//
int InitAll()
{
	WACOMMODEL model = { 0 };
	int fd, grab = 1;

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
	/* Grab tablet to avoid X server interaction */
	if ( !(fd = WacomGetFileDescriptor(ghTablet)) )
	{
		perror("GetFileDescriptor");
		WacomCloseTablet(ghTablet);
		WacomTermEngine(ghEngine);
		return(0);
	}
	else if (ioctl(fd,EVIOCGRAB, &grab) == -1)
	{
		perror("ioctl grab");
		WacomCloseTablet(ghTablet);
		WacomTermEngine(ghEngine);
		return(0);		
	}
	return 1;
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
	return 1;
}
