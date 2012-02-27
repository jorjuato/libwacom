//////////////////////////////////////////////////////////////////
//  snake_threaded.c			
//							
// Contiene codigo que genera un thread para leer la tableta y configura un temporizador 
// bassado en señal de alarma del sistema de señales POSIX de Linux. 
// OpenGL. Esta version utiliza como temporizador el sistema de señales nativo 
// de Linux. Se basa en wacomglut2.c, que a su vez se basa en wacomnoglut.c, habiendole
// añadido las rutinas minimas para la presentación de graficos OpenGL.
//
// gcc -Wall  -lpthread -lGL -lGLU -lglut -lwacom -O3 snake_threaded.c -o snake
//  
// Copyleft (L)		Noviembre 2007		Jorge Ibáñez  						
//
//////////////////////////////////////////////////////////////////


//Cabeceras de las librerias standar de C para io básica y llamadas al sistema
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h> 

#include "wacom.h"


#define MAXPUNTOS			7
#define TICK_INTERVAL		10		// Tiempo de muestreo de la tableta en milisegundos
	
#define X_RES		1600.00
#define Y_RES		1200.00
//#define X_AXE_RANG	10208.00
//#define Y_AXE_RANG	7424.00

#define X_AXE_RANG	87200.00
#define Y_AXE_RANG	65600.00
#define X_FACTOR	( (X_RES) / (X_AXE_RANG) )
#define Y_FACTOR	( (Y_RES) / (Y_AXE_RANG) )
#define PRESS_FACTOR 512
//#define PRESS_FACTOR 1024
#define GAMEMODESTRING "1600x1200:32@60" 

////////////////////////////////////  GLUT GLOBALS
int window;
int contador=0, esperate=0;
int g_bufferx[MAXPUNTOS];
int g_buffery[MAXPUNTOS];
static int g_counter = 0;
double gInitTime;
int next_time;
FILE* archivo;



//------------------------------------------------------------  time_left()
//
int time_left(void)
{
	int now = glutGet(GLUT_ELAPSED_TIME);
	
    if(next_time <= now)
	{
		next_time = now + TICK_INTERVAL;
        return 0;
	}
    else    return(next_time - now);
}

//------------------------------------------------------------  WacomTimer()
//
void WacomTimer(int value) {

	int i,altura;	
	altura = glutGet( GLUT_WINDOW_HEIGHT );	
	
	if (gAbsState[WACOMFIELD_PRESSURE].nValue == 0) 
	{
		glutTimerFunc(time_left(), WacomTimer, g_counter);
		contador=0;
		return;
	}
	for (i=0; i<MAXPUNTOS-1; i++)
	{	
		g_bufferx[i]=g_bufferx[i+1]; 
		g_buffery[i]=g_buffery[i+1]; 
	}
	g_bufferx[contador]=gAbsState[WACOMFIELD_POSITION_X].nValue * X_FACTOR; 
	g_buffery[contador]=gAbsState[WACOMFIELD_POSITION_Y].nValue * Y_FACTOR;
	fprintf(archivo,"%d \t %d  \t %d \t %d \t %d \t %d \n", g_bufferx[contador], g_buffery[contador],gAbsState[WACOMFIELD_POSITION_X].nValue, gAbsState[WACOMFIELD_POSITION_Y].nValue, gAbsState[WACOMFIELD_PRESSURE].nValue, glutGet(GLUT_ELAPSED_TIME));
	
	if (contador<MAXPUNTOS-1) 
	{ 	
		contador++;
	}
	else //if ( !(g_bufferx[contador] == g_bufferx[contador-1] && g_buffery[contador] == g_buffery[contador-1]) )		
	{			
		//glMatrixMode( GL_MODELVIEW_MATRIX );
		//glLoadIdentity();
		
		glColor3f((double)gAbsState[WACOMFIELD_PRESSURE].nValue/1024, (double)gAbsState[WACOMFIELD_PRESSURE].nValue/1024, 1);  
		glLineWidth(5);
 		glBegin(GL_LINES);
 			for (i=0; i<MAXPUNTOS; i++)
 		 		glVertex2i(g_bufferx[i], g_buffery[i]);
		glEnd();
	 			
		if (esperate<20)
		{
			esperate++;
			glutSwapBuffers();
		}
		else 	
		{
			esperate=0; 
			OnDraw();
		} 
		
	}	
	g_counter = value + 1; 
	glutTimerFunc(time_left(), WacomTimer, g_counter);
}


//------------------------------------------------------------  OnReshape()
//
void OnReshape(int w, int h) {   
    
 	if (!h)
		return;
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();        
}

//------------------------------------------------------------  OnDraw()
//
void OnDraw(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode( GL_MODELVIEW_MATRIX );
	glLoadIdentity();
	glColor3f(0.0, 1.0, 0.5);  	
		glRecti(500, 0, 700, 100);
		glRecti(500, 768, 700, 668);
		glRecti(0, 250, 300, 550);

	/*glBegin(GL_LINES);
		glVertex2f(-40.0, 0.0);
		glVertex2f(700.0, 0.0);
		glVertex2f(0.0, -40.0);
		glVertex2f(0.0, 700.0);
	glEnd();*/
 	glutSwapBuffers();
}

//------------------------------------------------------------  OnInit()
//
void OnInit() {
    // enable depth testing
    glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	archivo = fopen("datos","w");
	fprintf(archivo,"X \t Y \t WacX \t WacY \t Press \t Time \n");
	WacomInit(WACOM_SCANMODE_NOCALLBACK, NULL);
	
	// set the function to handle timer event
	next_time = glutGet(GLUT_ELAPSED_TIME) + TICK_INTERVAL;
	glutTimerFunc(time_left(), WacomTimer, g_counter);
	glutSetCursor(GLUT_CURSOR_NONE);
}

//------------------------------------------------------------  OnExit()
//
void OnExit() {
	fclose(archivo);
	WacomClose();
}

//------------------------------------------------------------  OnKeyboad()
//
void OnKeyboard(unsigned char key, int x, int y){

	switch (key) 
	{ 
		case 27: 
			exit(0); 
		break;
	}
}

//------------------------------------------------------------  main()
//
int main(int argc, char **argv)
{
    
    // initialise glut
    glutInit(&argc,argv);

    // request a depth buffer, RGBA display mode, and we want double buffering
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );

    // set the initial window size
    glutInitWindowSize(X_RES,Y_RES);

    // create the window
    window = glutCreateWindow("Serpiente GLUT");
        
    // if we would want fullscreen:
    glutFullScreen();
    //glutGameModeString(GAMEMODESTRING);
    //glutEnterGameMode();

    // set the function to use to draw our scene
    glutDisplayFunc(OnDraw);

    // set the function to handle changes in screen size
    glutReshapeFunc(OnReshape);
	
	// set the function to handle events on keyboard
	glutKeyboardFunc(OnKeyboard);

    // set the function to be called when we exit
    atexit(OnExit);
  
    // run our custom initialisation
    OnInit();
    
    // this function runs a while loop to keep the program running.    
    glutMainLoop();
    
    return 0;	
	    
}
