//////////////////////////////////////////////////////////////////
//  wacom.h			
//							
//	Cabeceras para acceso a las funciones del API contenido en
//	libwacom.a
//
//	Para linkar la libreria se debe llamar al compilador:
//
//				gcc archivosfuente.c  -L. -lwacom -lpthread -o binfile
//
//  en ese preciso orden.
//
//
// Copyleft (L)		Febrero 2008		Jorge Ibáñez  						
//
//////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Wacom Custom Public API
*****************************************************************************/

void WacomInit(int Mode, void (*WacomSignalHandler)(int));  
			// Inicializa y configura la tableta, lanza el thread wacom 
			// y configura la funcion que será llamada cuando se genere un evento.
			// Si no se desean retrollamadas, el segundo argumento ha ser NULL

void WacomClose(void); 
			// Libera recursos de la tableta y gráficos, termina el thread, 
			// y cierra los archivos abiertos para escritura. 

int WacomChangeMode(int Modo, void (*WacomSignalHandler)(int));
			// Cambia el modo de funcionamiento del thread que lee la tableta.
			// Los parámetros son idénticos a los de WacomInit() 
			// Devuelve 0 si hay exito, 1 si hay errores.

/*****************************************************************************
** Wacom Scanmode Enumeration
*****************************************************************************/

// Enumeracion de las constantes que definen los modos de funcionamiento del driver:
// 		WACOM_SCANMODE_CALLBACK: Cada evento produce una llamada a la funcion callback
//		WACOM_SCANMODE_NOCALLBACK: Los eventos no generan retrollamadas.

typedef enum
{
	WACOM_SCANMODE_CALLBACK,
	WACOM_SCANMODE_NOCALLBACK,
	WACOM_SCANMODE_MAX
} WACOM_SCANMODE;


			
/*****************************************************************************
** Definiciones del API para el driver wacom
*****************************************************************************/

#define WACOMVENDOR_UNKNOWN     0x0000
#define WACOMVENDOR_WACOM       0x056A
#define WACOMVENDOR_TPC         0xFFFFFF01

#define WACOMCLASS_SERIAL       0x0001
#define WACOMCLASS_USB          0x0002

#define WACOMDEVICE_UNKNOWN     0x0000
#define WACOMDEVICE_ARTPAD      0x0001
#define WACOMDEVICE_ARTPADII    0x0002
#define WACOMDEVICE_DIGITIZER   0x0003
#define WACOMDEVICE_DIGITIZERII 0x0004
#define WACOMDEVICE_PENPARTNER  0x0005
#define WACOMDEVICE_GRAPHIRE    0x0006
#define WACOMDEVICE_GRAPHIRE2   0x0007
#define WACOMDEVICE_GRAPHIRE3   0x0008
#define WACOMDEVICE_GRAPHIRE4   0x0009
#define WACOMDEVICE_INTUOS      0x000A
#define WACOMDEVICE_INTUOS2     0x000B
#define WACOMDEVICE_CINTIQ      0x000C
#define WACOMDEVICE_PTU         0x000D
#define WACOMDEVICE_VOLITO      0x000E
#define WACOMDEVICE_VOLITO2     0x000F
#define WACOMDEVICE_TPC         0x0010
#define WACOMDEVICE_INTUOS3     0x0011
#define WACOMDEVICE_CINTIQV5    0x0012

#define WACOMTOOLTYPE_NONE      0x00
#define WACOMTOOLTYPE_PEN       0x01
#define WACOMTOOLTYPE_PENCIL    0x02
#define WACOMTOOLTYPE_BRUSH     0x03
#define WACOMTOOLTYPE_ERASER    0x04
#define WACOMTOOLTYPE_AIRBRUSH  0x05
#define WACOMTOOLTYPE_MOUSE     0x06
#define WACOMTOOLTYPE_LENS      0x07
#define WACOMTOOLTYPE_PAD       0x08
#define WACOMTOOLTYPE_MAX       0x09

#define WACOMBUTTON_LEFT        0
#define WACOMBUTTON_MIDDLE      1
#define WACOMBUTTON_RIGHT       2
#define WACOMBUTTON_EXTRA       3
#define WACOMBUTTON_SIDE        4
#define WACOMBUTTON_TOUCH       5
#define WACOMBUTTON_STYLUS      6
#define WACOMBUTTON_STYLUS2     7
#define WACOMBUTTON_BT0         8
#define WACOMBUTTON_BT1         9
#define WACOMBUTTON_BT2         10
#define WACOMBUTTON_BT3         11
#define WACOMBUTTON_BT4         12
#define WACOMBUTTON_BT5         13
#define WACOMBUTTON_BT6         14
#define WACOMBUTTON_BT7         15
#define WACOMBUTTON_BT8         16
#define WACOMBUTTON_BT9         17
#define WACOMBUTTON_BT10        18
#define WACOMBUTTON_BT11        19
#define WACOMBUTTON_BT12        20
#define WACOMBUTTON_BT13        21
#define WACOMBUTTON_BT14        22
#define WACOMBUTTON_BT15        23
#define WACOMBUTTON_BT16        24
#define WACOMBUTTON_BT17        25
#define WACOMBUTTON_BT18        26
#define WACOMBUTTON_BT19        27
#define WACOMBUTTON_BT20        28
#define WACOMBUTTON_BT21        29
#define WACOMBUTTON_BT22        30
#define WACOMBUTTON_BT23        31
#define WACOMBUTTON_MAX         32

#define WACOMFIELD_TOOLTYPE     0
#define WACOMFIELD_SERIAL       1
#define WACOMFIELD_PROXIMITY    2
#define WACOMFIELD_BUTTONS      3
#define WACOMFIELD_POSITION_X   4
#define WACOMFIELD_POSITION_Y   5
#define WACOMFIELD_ROTATION_Z   6
#define WACOMFIELD_DISTANCE	7
#define WACOMFIELD_PRESSURE	8
#define WACOMFIELD_TILT_X       9
#define WACOMFIELD_TILT_Y       10
#define WACOMFIELD_ABSWHEEL     11
#define WACOMFIELD_RELWHEEL     12
#define WACOMFIELD_THROTTLE     13
#define WACOMFIELD_MAX          14


/*****************************************************************************
** Custom Wacom State structures
*****************************************************************************/

struct ABS_STATE
{
	int bValid;
	int nValue, nMin, nMax;
};

struct KEY_STATE	
{
	int bValid;
	int nValue;
};

// La version actual de la libreria solo actualiza el valor de los campos
// presion, x, y, proximity del array gAbsState...

extern struct ABS_STATE gAbsState[WACOMFIELD_MAX];
extern struct KEY_STATE gKeyState[WACOMBUTTON_MAX];




