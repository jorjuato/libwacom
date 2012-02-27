//////////////////////////////////////////////////////////////////
//  pili_draw.c
//
//
// Copyleft (L)		Septiembre 2008		Jorge Ibáñez
//
//////////////////////////////////////////////////////////////////

//Cabeceras con definiciones del API Wacom
//#include "wacom.h"

//Cabecera principal
#include "SDL/SDL.h"
//#include "SDL/SDL_opengl.h"
//#include "SDL/SDL_framerate.h"
#include "SDL/SDL_gfxPrimitives.h"
//#include "./font/nfontc.h"
//#include "SDL/SDL_gfxBlitFunc.h"
#define __INCLUDESDL

//Cabeceras con definiciones para el experimento
#include "display.h"
#include "pili.h"
#include "pili_draw.h"


//////////////////////////////////////////////////// MAIN GLOBALS
extern SDL_Surface *screen;					// Puntero a la pantalla completa. Reside en sdlgl_control.c

//////////////////////////////////////////////////// IOR GLOBAL STATE VARIABLES
extern char* g_trial_type;							// Actual type of trial, value from enum TRIAL_TYPE
extern int g_trial_number;						// Index of the actual number of trial in matrix Resultados
extern int g_trial_state;
extern CONFIG g_config;

//------------------------------------------------------------  UpdateScreen()
//
void UpdateScreen(){
	SDL_Flip(screen);
}

//------------------------------------------------------------  ShowBackground()
//
 void ShowBackground(Uint32 color){
	SDL_FillRect(screen, NULL, color);
}

//------------------------------------------------------------ ShowMesagge()
//
void ShowMessage(const char* msg){
	//SDL_Surface *text;
	//SDL_Color color = {255, 255, 255};

	//stringColor(screen, Sint16 x, Sint16 y, g_message,Uint32 color);
	//gfxPrimitivesSetFont(unsigned char *fontdata, int cw, int ch);

	/* Creates an SDL surface containing the specified text */
	//text = FNT_Render(msg, color);

	//SDL_BlitSurface(text, NULL, screen, NULL);
	//SDL_FreeSurface(text);

}

//------------------------------------------------------------  ShowFixation()
//
 void ShowInitPoint(){
 	Sint16 x,y,r;
 	x = (Sint16) ABS_SCALE_X(g_config.x_start);
 	y = (Sint16) ABS_SCALE_Y(g_config.y_start);
 	r = (Uint16) REL_SCALE_X(g_config.r_start);
	filledCircleColor(screen,x,y,r,RGBA_Cyan);
}

//------------------------------------------------------------  ShowCue()
//
 void ShowTargets()
{
	SDL_Rect target1, target2;

	if (g_trial_state < TRIAL_STATE_CONTACT1)
	{
		target1.x = (Sint16)ABS_SCALE_X(g_config.x1_target1 +TARGET1_WIDTH/2);
		target1.y = (Sint16)ABS_SCALE_Y(g_config.y1_target1- g_config.h_target1/2);
		target1.w = (Uint16)REL_SCALE_X(TARGET1_WIDTH);
		target1.h = (Uint16)REL_SCALE_Y(g_config.h_target1);

		target2.x = (Sint16)ABS_SCALE_X(g_config.x1_target2 + TARGET2_WIDTH/2);
		target2.y = (Sint16)ABS_SCALE_Y(g_config.y1_target2 - g_config.h_target2/2);
		target2.w = (Uint16)REL_SCALE_X(TARGET2_WIDTH);
		target2.h = (Uint16)REL_SCALE_Y(g_config.h_target2);
	}
	else
	{
		target1.x = (Sint16)ABS_SCALE_X(g_config.x2_target1 +TARGET1_WIDTH/2);
		target1.y = (Sint16)ABS_SCALE_Y(g_config.y2_target1- g_config.h_target1/2);
		target1.w = (Uint16)REL_SCALE_X(TARGET1_WIDTH);
		target1.h = (Uint16)REL_SCALE_Y(g_config.h_target1);

		target2.x = (Sint16)ABS_SCALE_X(g_config.x2_target2 + TARGET2_WIDTH/2);
		target2.y = (Sint16)ABS_SCALE_Y(g_config.y2_target2 - g_config.h_target2/2);
		target2.w = (Uint16)REL_SCALE_X(TARGET2_WIDTH);
		target2.h = (Uint16)REL_SCALE_Y(g_config.h_target2);
	}
	SDL_FillRect(screen, &target1, RGB_White);
	SDL_FillRect(screen, &target2, RGB_White);
}




