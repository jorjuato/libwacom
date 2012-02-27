





//Cabeceras de las librerias standar de C para io básica y llamadas al sistema
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>     /* standard unix functions  */
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

//Cabeceras de la libreria SDL
#ifndef __INCLUDESDL
#include "SDL/SDL.h"
#include "SDL/SDL_gfxPrimitives.h"	
#define __INCLUDESDL
#endif

#include "display.h"

//Cabeceras de la libreria Wacom
#ifndef __INCLUDEWACOM
#include "wacom.h"
#define __INCLUDEWACOM
#endif

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

typedef struct {
	float x;
	float y;
	float time;
	int   press;
} TRIALDATA;

///////////////////////////////////////////////////		API Callback funtions

void OnExit();

void OnInit();

void OnDraw();

void OnReshape(int w, int h);

static SDL_Surface* setup_sdl(); 

