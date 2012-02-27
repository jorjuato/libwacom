//////////////////////////////////////////////////////////////////
//  main.h
//
//
// Copyleft (L)		Septiembre 2008		Jorge Ibáñez
//
//////////////////////////////////////////////////////////////////


//Cabeceras estandar del SO
#include <stdlib.h>
#include <stdio.h>
//#include <string.h>
#include <unistd.h>
#include <math.h>

//Cabeceras de la libreria SDL
#ifndef __INCLUDESDL
#include "SDL/SDL.h"
#define __INCLUDESDL
#endif

//Cabeceras con definiciones del API Wacom
#ifndef __INCLUDEWACOM
#include "wacom.h"
#define __INCLUDEWACOM
#endif

//Cabeceras específicas para configurar este experimento
#include "pili.h"
#include "pili_draw.h"

//Cabeceras genéricas para todos los experimentos
#include "display.h"
#include "auxfuncs.h"


//////////////////////////////////////////////////////////////////
//		PROTOTIPOS DE LAS FUNCIONES								//
//////////////////////////////////////////////////////////////////

void OnExit();

void OnInit();

static void ExperimentSetup(int argc, char **argv);

static void ExperimentEnd(void);

static void ExperimentLoop();

static int ExperimentFSA(void);

static void ExperimentGraphics(void);

//static void ExperimentHandler(int);

static void ExperimentEventMask();

void ExperimentSaveConfig();

void ExperimentSaveData(float x, float y, int press, int time_inc);

//////////External defs

void ParseConfFile(const char* filename);

void ParseCmdLine(int argc, char** argv);

void InitTiming();

void SetDelay(long int ns);

long int getns();

long int ExperimentTimeLeft();
