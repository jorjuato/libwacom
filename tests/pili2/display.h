//////////////////////////////////////////////////////////////////
//  display.h
//
//
// Copyleft (L)		Febrero 2008		Jorge Ibáñez
//
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// SDL_WIDTH  = horizontal screen resolution in pixels
// SDL_HEIGHT = vertical screen resolution in pixels
///////////////////////////////////////////////////////////////////////////
// (0,0)														(0,WIDTH)//
//																		 //
//																		 //
//																		 //
//																		 //
//																		 //
//																		 //
//					SDL COORDINATE REFERENCE SYSTEM						 //
//								( x , y )								 //
//																		 //
//																		 //
//																		 //
//																		 //
//																		 //
//																		 //
// (HEIGHT,0)										       (HEIGHT,WIDTH)//
///////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////
// EXPERIMENT_WIDTH  = horizontal screen size in centimeters minus right offset
// EXPERIMENT_HEIGHT = vertical screen size in centimeters divided by two
/////////////////////////////////////////////////////////////////////////// (HEIGTH,-OFFSET) 
// (HEIGHT,WIDTH)											 (HEIGTH,0)	 //
//																  |		 //
//																  |	     //
//					EXPERIMENTS COORDINATE REFERENCE SYSTEM		  |	     //
//								( x , y )						  |	     //
//																  |		 //
//----------------------------------------------------------------+------// (0, -OFFSET)
//																(0,0)	 //
//																  |  	 //
//																  |		 //
//																  |		 //
//																  |		 //
// (-HEIGHT,WIDTH)										     (-HEIGTH,0) // 
/////////////////////////////////////////////////////////////////////////// (-HEIGHT,-OFFSET)

// main SDL-OpenGL headers
#ifndef __INCLUDESDL
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#define __INCLUDESDL
#endif

#define COLORKEY 0, 0, 0 //The Transparent colour

//--------------------------------------------------------------//
//  Simple list of SDL RBG Uint32 colors:						//
//--------------------------------------------------------------//

#define ColorRGB(X,Y,Z) (Uint32)(65536 * X + 256 * Y + Z)

#define RGB_Black		ColorRGB(0  , 0  , 0  )
#define RGB_Red			ColorRGB(255, 0  , 0  )
#define RGB_Green		ColorRGB(0  , 255, 0  )
#define RGB_Blue		ColorRGB(0  , 0  , 255)
#define RGB_Cyan		ColorRGB(0  , 255, 255)
#define RGB_Magenta		ColorRGB(255, 0  , 255)
#define RGB_Yellow		ColorRGB(255, 255, 0  )
#define RGB_White		ColorRGB(255, 255, 255)

#define RGB_Maroon		ColorRGB(128, 0  , 0  )
#define RGB_Darkgreen	ColorRGB(0  , 128, 0  )
#define RGB_Navy		ColorRGB(0  , 0  , 128)
#define RGB_Teal		ColorRGB(0  , 128, 128)
#define RGB_Purple		ColorRGB(128, 0  , 128)
#define RGB_Olive		ColorRGB(128, 128, 0  )
#define RGB_Gray		ColorRGB(128, 128, 128)
#define RGB_Grey		ColorRGB(192, 192, 192)

//--------------------------------------------------------------//
//  Simple list of SDL RBGA Uint32 colors (con transparencias):	//
//--------------------------------------------------------------//

#define ColorRGBA(X , Y , Z , A) (Uint32)(16777216 * X + 65536 * Y + 256 * Z + A)

#define RGBA_Black		0x000000FF
#define RGBA_Red		0xFF0000FF
#define RGBA_Green		0x00FF00FF
#define RGBA_Blue		0x0000FFFF
#define RGBA_Cyan		0x00FFFFFF
#define RGBA_Magenta	0xFF00FFFF
#define RGBA_Yellow		0xFFFF00FF
#define RGBA_White		0xFFFFFFFF
/*
#define RGBA_Black		ColorRGBA(0  , 0  , 0  , 255)
#define RGBA_Red		ColorRGBA(255, 0  , 0  , 255)
#define RGBA_Green		ColorRGBA(0  , 255, 0  , 255)
#define RGBA_Blue		ColorRGBA(0  , 0  , 255, 255)
#define RGBA_Cyan		ColorRGBA(0  , 255, 255, 255)
#define RGBA_Magenta	ColorRGBA(255, 0  , 255, 255)
#define RGBA_Yellow		ColorRGBA(255, 255, 0  , 255)
#define RGBA_White		ColorRGBA(255, 255, 255, 255)
*/
#define RGBA_Maroon		ColorRGBA(128, 0  , 0  , 255)
#define RGBA_Darkgreen	ColorRGBA(0  , 128, 0  , 255)
#define RGBA_Navy		ColorRGBA(0  , 0  , 128, 255)
#define RGBA_Teal		ColorRGBA(0  , 128, 128, 255)
#define RGBA_Purple		ColorRGBA(128, 0  , 128, 255)
#define RGBA_Olive		ColorRGBA(128, 128, 0  , 255)
#define RGBA_Gray		ColorRGBA(128, 128, 128, 255)
#define RGBA_Grey		ColorRGBA(192, 192, 192, 255)

//--------------------------------------------------------------//
// Definiciones de valores de la configuracion física de la tableta
//--------------------------------------------------------------//

#define X_RES			1600.00
#define Y_RES			1200.00
//#define X_RES			1280.00
//#define Y_RES			800.00
//#define X_RES			1024
//#define Y_RES			768
#define WIDTH			X_RES
#define HEIGHT			Y_RES
#define WAC_X_MAX		87200.00
#define WAC_Y_MAX		65600.00
//#define WAC_X_MAX		10208.00
//#define WAC_Y_MAX		7424.00
#define PRESS_FACTOR 	1024.00
//#define PRESS_FACTOR 	512.00
//#define X_FACTOR		( (X_RES) / (X_AXE_RANG) )
//#define Y_FACTOR		( (Y_RES) / (Y_AXE_RANG) )
#define ASPECT_RATIO 	(WIDTH / HEIGHT )
//#define GAMEMODESTRING 	"1600x1200:24@60"
#define MAX_X			38.0
#define MIN_X			-5.0
#define MAX_Y			16.25
#define MIN_Y			-16.25
#define RANGE_X 		(MAX_X-MIN_X)
#define RANGE_Y			(MAX_Y-MIN_Y)
#define ABS_SCALE_X(x)	((MAX_X - x) *(X_RES/RANGE_X))
#define ABS_SCALE_Y(x)	((MAX_Y - x) *(Y_RES/RANGE_Y))
#define REL_SCALE_X(r)	(r*(X_RES/RANGE_X))
#define REL_SCALE_Y(r)	(r*(Y_RES/RANGE_Y))
#define WAC_SCALE_X(x)	(MAX_X - (((float)x) *(RANGE_X/WAC_X_MAX)))
#define WAC_SCALE_Y(x)	(MAX_Y - (((float)x) *(RANGE_Y/WAC_Y_MAX)))


