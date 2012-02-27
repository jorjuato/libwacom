//////////////////////////////////////////////////////////////////
//  sdlgl_control.c
//	Funciones de inicializacion de la libreria SDL y del contexto
//	OpenGL para graficar en 3D.
//
//
// Copyleft (L)		Febrero 2008		Jorge Ibáñez
//
//////////////////////////////////////////////////////////////////


//Cabeceras de las librerias standar de C para io básica y llamadas al sistema
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>     /* standard unix functions  */
#include <math.h>


//Cabecera principal
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#define __INCLUDESDL
#include "display.h"
#include "sdlgl_control.h"


#define APP_NAME	"SDL Pili Experiment"
#define APP_VERSION	"0.0.0.0.0"

SDL_Surface *screen;

char *logfile="pili.log";
FILE* ErrorLog;

//------------------------------------------------------------------------------//
//																				//
//							Graphic Engine Callback Functions					//
//																				//
//------------------------------------------------------------------------------//

//------------------------------------------------------------  OnInit()
// Inicializa la libreria gráfica, el logging system y carga imágenes
void OnInit() {

	// En principio solo usaremos respuestas desde el teclado
	//WacomInit(WACOM_SCANMODE_NOCALLBACK, NULL);
	//WacomInit(WACOM_SCANMODE_CALLBACK, ExperimentHandler);

 if ( !(InitErrorLog(logfile)))
	{
		perror("Couldn't open log file");
		exit(APPERROR);
	}
	else if (!(screen = setup_sdl()) )
	{
		perror("Couldn't initialize video");
		exit(APPERROR);
	}
/*	Si quisieramos usar opengl en el programa la secuencia de inicio de sdl
	debería ser la siguiente:

	else if (!(screen = setup_sdlgl()) )
	{
		perror("Couldn't initialize video");
		exit(APPERROR);
	}
	else if (!(screen = setup_opengl()) )
	{
		perror("Couldn't initialize video");
		exit(APPERROR);
	}
												*/
}

//------------------------------------------------------------  OnExit()
// Salir limpiamente de SDL
void OnExit() {
	CloseErrorLog();
	SDL_Quit();
}
//------------------------------------------------------------  OnDraw()
//Refresca la pantalla cuando se recibe un evento de repintado del WM
void OnDraw() {
	SDL_FillRect(screen, NULL, 0);
	SDL_Flip(screen);
}
//------------------------------------------------------------  LoadImage()
//
SDL_Surface* LoadImage(char *file) {
	SDL_Surface *tmp;
	tmp = SDL_LoadBMP(file);

	if (tmp == NULL)
	{
		fprintf(stderr, "Error: '%s' could not be opened: %s\n", file, SDL_GetError());
	}
	else
	{
		if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(tmp->format, COLORKEY)) == -1)
		fprintf(stderr, "Warning: colorkey will not be used, reason: %s\n", SDL_GetError());

	}
	return SDL_DisplayFormat(tmp);
}
//------------------------------------------------------------------------------//
//																				//
//					Graphic Engine Setup Routines								//
//																				//
//------------------------------------------------------------------------------//
//------------------------------------------------------------  setup_sdl()
//
static SDL_Surface* setup_sdl()
{
    const SDL_VideoInfo* video;
    SDL_Surface *screen;

    if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return(ERROR);
    }

    atexit(OnExit);

    video = SDL_GetVideoInfo();

    if( !video )
	{
        fprintf(stderr,"Couldn't get video information: %s\n", SDL_GetError());
        return(ERROR);
    }
    if( (screen = SDL_SetVideoMode( WIDTH, HEIGHT, video->vfmt->BitsPerPixel, SDL_DOUBLEBUF | SDL_FULLSCREEN )) == NULL )
    //if( (screen = SDL_SetVideoMode( WIDTH, HEIGHT, video->vfmt->BitsPerPixel, SDL_DOUBLEBUF )) == NULL )
	{
        fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
        return(ERROR);
    }
    return screen;

}
//------------------------------------------------------------  setup_sdlgl()
//
static SDL_Surface* setup_sdlgl()
{
    const SDL_VideoInfo* video;
	SDL_Surface *screen;

    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr,
                "Couldn't initialize SDL: %s\n", SDL_GetError());
        return(ERROR);
    }

    atexit(OnExit);

    video = SDL_GetVideoInfo();

    if( !video ) {
        fprintf(stderr,
                "Couldn't get video information: %s\n", SDL_GetError());
        return(ERROR);
    }

    /* Set the minimum requirements for the OpenGL window */
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    /* Note the SDL_DOUBLEBUF flag is not required to enable double
     * buffering when setting an OpenGL video mode.
     * Double buffering is enabled or disabled using the
     * SDL_GL_DOUBLEBUFFER attribute.
     */
    if( !(screen = SDL_SetVideoMode( WIDTH, HEIGHT, video->vfmt->BitsPerPixel, SDL_OPENGL | SDL_FULLSCREEN )) ) {
        fprintf(stderr,
                "Couldn't set video mode: %s\n", SDL_GetError());
        return(ERROR);
    }
    return screen;
}
//------------------------------------------------------------  setup_opengl()
//
static int setup_opengl()
{
    //float aspect = (float)WIDTH / (float)HEIGHT;

    /* Make the viewport cover the whole window */
    glViewport(0, 0, WIDTH, HEIGHT);

    /* Set the camera projection matrix:
     * field of view: 90 degrees
     * near clipping plane at 0.1
     * far clipping plane at 100.0
     */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, WIDTH, HEIGHT, 0.0f, -1.0f, 1.0f);
    //gluPerspective(60.0, aspect, 0.1, 100.0);
    /* We're done with the camera, now matrix operations
     * will affect the modelview matrix
     * */
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    /* set the clear color to gray */
    //glClearColor(0.5, 0.5 ,0.5, 0);
    glClearColor(0, 0 ,0, 0);
    /* We want z-buffer tests enabled*/
    glEnable(GL_DEPTH_TEST);

	//glFlush();
    /* Do draw back-facing polygons*/
   // glDisable(GL_CULL_FACE);
    return(SUCCESS);
}

//------------------------------------------------------------------------------//
//																				//
//							 Error Log System									//
//																				//
//------------------------------------------------------------------------------//
//------------------------------------------------------------  InitErrorLog()
//
static int InitErrorLog(char* logfile)								// Initializes Error Logging
{
	if(!(ErrorLog = fopen(logfile, "w")))			// If We Can't Open LOG_FILE For Writing
	{
		perror("Can't init Logfile!\n" );			// Report With perror() (Standard + Explains Cause Of The Error)
		return(ERROR);									// And Exit, This Is Critical, We Want Logging
	}

	Log("%s V%s -- Log Init...\n\n",
		APP_NAME, APP_VERSION);						// We Print The Name Of The App In The Log

	return TRUE;									// Otherwhise Return TRUE (Everything Went OK)
}
//------------------------------------------------------------  CloseErrorLog()
//
static void CloseErrorLog(void)							// Closes Error Logging
{
	Log("\n-- Closing Log...\n");					// Print The End Mark

	if(ErrorLog)									// If The File Is Open
	{
		fclose(ErrorLog);							// Close It
	}

	return;											// And Return, Quite Plain Huh? :)
}
//------------------------------------------------------------  Log()
//
int Log(char *szFormat, ...)						// Add A Line To The Log
{
	va_list Arg;									// We're Using The Same As The printf() Family, A va_list
													// To Substitute The Tokens Like %s With Their Value In The Output

	va_start(Arg,szFormat);							// We Start The List

	if(ErrorLog)									// If The Log Is Open
	{
		vfprintf(ErrorLog, szFormat, Arg);			// We Use vprintf To Perform Substituctions
		fflush(ErrorLog);							// And Ensure The Line Is Written, The Log Must Be Quick
	}

	va_end(Arg);									// We End The List

	return 0;										// And Return A Ok
}

