//////////////////////////////////////////////////////////////////
//  sdlgl_control.h			
//	Funciones de inicializacion de la libreria SDL y del contexto
//	OpenGL para graficar en 3D.
// Cabeceras con definiciones para el acceso a las librerias gráficas
// SDL y OpenGL, así como definiciones de las caracteristicas físicas
// del dispositivo gráfico donde se ejecuta el programa
//
//
// Copyleft (L)		Febrero 2008		Jorge Ibáñez  						
//
//////////////////////////////////////////////////////////////////




//------------------------------------------------------------------------------//
//																				//
//					Graphic Engines' Setup Routines	Prototypes					//
//																				//
//------------------------------------------------------------------------------//

///////////////////////////////////////////////////		API Callback funtions

void OnExit();

void OnInit();

void OnDraw();

void OnReshape(int w, int h);

SDL_Surface* LoadImage(char *file);

///////////////////////////////////////////////////		Basic Initializing and Drawing SDL/GL API
static int setup_opengl();

static SDL_Surface* setup_sdlgl();

static SDL_Surface* setup_sdl(); 


///////////////////////////////////////////////////		Logging API

static int InitErrorLog(char* logfile);

static void CloseErrorLog(void);

int Log(char *szFormat, ...);

//--------------------------------------------------------------//
//Definicion de verdad y falsedad, jeje
//--------------------------------------------------------------//
#define true 		1
#define false 		0
#define TRUE		true
#define FALSE		false
#define APPERROR	true
#define APPSUCCESS	false
#define ERROR		false
#define SUCCESS 	true
