//////////////////////////////////////////////////////////////////
//  main.c
//
//
// Copyleft (L)		Septiembre 2008		Jorge Ibáñez
//
//////////////////////////////////////////////////////////////////

#include "main.h"

//////////////////////////////////////////////////// INPUT/OUTPUT GLOBALS
extern 		SDL_Surface *screen;						// Screen to paint in, defined in sdlgl_control.c
const char*	g_conf_file_name="./configuracion.txt";		// Hardcoded name of config parameters file
CONFIG 		g_config;									// Config structure of actual experiment
CONFIG*		G_Config[MAX_CONF_NUMBER];					// Array of CONFIG structures, holds configuration from text file.
int 		g_config_number = 0;						// Counter, allow dynamic allocation of CONFIG struct in array G_Config
FILE*		g_GlobalFile=NULL;							// FILE pointer for summary results per individual
FILE*		g_DataFile=NULL;							// FILE pointer with detailed data on each trial.
TRIALDATA	g_TrialData[1000];
int 		g_DataCount = -1;
//////////////////////////////////////////////////// EXPERIMENT GLOBAL STATE VARIABLES
int 		g_trial_state = TRIAL_STATE_INTER_TRIAL;	// Actual state of trial, value from enum TRIAL_STATE
int 		g_trial_number = -1;						// Index of the actual number of trial
int 		g_trial_type = -1;							// Trial types as indexed in configuration file.
Uint32 		g_init_time = 0;							// Absolute (in ms from beggining) actual state's staRting time
Uint32 		g_timeofchange = 0;							// Time of change of the targets positions
const char* g_error_msg = "Generic Error";				// Error messages to be printed on screen
const char* g_bHitTarget1=NULL;							// Flag, sets if the user hited first target
const char* g_bHitTarget2=NULL;							// Flag, sets if the user hited second target

extern const char*	g_subject_name;
///////////////////////////////////////////////////  GLOBAL CONTROL FLAGS
//Uint8 	g_bDrawn = FALSE;							// Flag, sets if the current state has been drawn
//Uint8 	g_bKeyStroke = FALSE;						// Flag, sets the onset of a keyboard event
//Uint8 	g_bTabletEvent = FALSE;						// Flag, sets the onset of a tablet event

extern struct timeval now_tv, next_tv;
	
//------------------------------------------------------------  main()
//
int main(int argc, char **argv){
	OnInit();									//Inicializa recursos: sdlgl_control.c
	ExperimentSetup(argc,argv); 				//Configura el experimento
	ExperimentLoop();
	ExperimentEnd();							//Cierra archivos y libera recursos.
	return(0);									//No debería llegar nunca a este punto
}

//------------------------------------------------------------  ExperimentSetup()
//
static void ExperimentSetup(int argc, char **argv){
	// Parsea la linea de comandos y el archivo de configuracion: 	parser.c
	ParseCmdLine(argc, argv);
	ParseConfFile(g_conf_file_name);

	//Inicializamos la tableta con la función handler
	//WacomInit(WACOM_SCANMODE_NOCALLBACK, NULL);
	//pthread_t thread = WacomInit(NULL);
	WacomInit(NULL);
	
	//Crea la mascara de eventos y escondemos cursor
	ExperimentEventMask();
	SDL_ShowCursor(FALSE);

	//Generar nombres de archivos, abrirlos y escribir cabeceras
	ExperimentFiles();
	
	//Init timing variables
	InitTiming();

}

//------------------------------------------------------------  ExperimentHandler()
//
/*
void ExperimentHandler(int signal){
	if (g_trial_state > TRIAL_STATE_INTER_TRIAL)
		ExperimentSaveData(SDL_GetTicks()-g_init_time);
	g_bTabletEvent = True;
}*/

//------------------------------------------------------------  EndExperiment()
//
static void ExperimentEnd(void){
	printf("Cerrando el programa y liberando recursos\n");
	fflush(stdout);
	ShowBackground(RGB_Blue);
	UpdateScreen();
	WacomClose();
	if(g_DataFile) {
		fflush(g_DataFile);
		fclose(g_DataFile);
	}
	if(g_GlobalFile) {
		fflush(g_GlobalFile);
		fclose(g_GlobalFile);
	}
	exit(APPSUCCESS);
}



//------------------------------------------------------------  MainLoop()
//
static void ExperimentLoop()
{
    SDL_Event event;    
	
	//Bucle indefinido que lleva a cabo el experimento
    while(TRUE) {
        //Implementacion del timer sin usar señales
		//SDL_Delay(ExperimentTimeLeft());
		SetDelay(ExperimentTimeLeft());

        //Funcion principal para regular los estados del experimento
        //Contiene los condicionales asociados a la salida de WACOMlib
    	if (!ExperimentFSA())
    	 	break;

        //Bucle principal de eventos SDL
        //Si no se va a obtener ningún evento útil, mejor desecharlos...
        //Ahora solo procesa el escape antes de tocar la pantalla
        while( SDL_PollEvent(&event))
       		if (g_trial_state == TRIAL_STATE_PRECONTACT)
				if (event.key.keysym.sym == SDLK_ESCAPE || event.type == SDL_QUIT)
					return;
    }
}



//------------------------------------------------------------  ExperimentTimer()
//
static int ExperimentFSA(void){
	Uint32 now,time_inc;
	static float x,y,x1,y1;
	Uint16 press;
	static Uint8 bRepeat = FALSE;

	//Actualizamos variables temporales
	now = SDL_GetTicks();
	time_inc = now - g_init_time;


	//Actualizamos los variables de la tableta
	x1    = gAbsState[WACOMFIELD_POSITION_X].nValue;
	y1    = gAbsState[WACOMFIELD_POSITION_Y].nValue;
	x     = WAC_SCALE_X(gAbsState[WACOMFIELD_POSITION_X].nValue);
	y     = WAC_SCALE_Y(gAbsState[WACOMFIELD_POSITION_Y].nValue);
	press = gAbsState[WACOMFIELD_PRESSURE].nValue;

	// Guardamos estado detallado del movimiento si procede
	/*
	if (g_trial_state > TRIAL_STATE_INIT_CONTACT){
		g_DataCount++;	
		g_TrialData[g_DataCount].x = x;
		g_TrialData[g_DataCount].y = y;
		g_TrialData[g_DataCount].time = time_inc;
		g_TrialData[g_DataCount].press = press;
			
	}*/
	
	if (g_trial_state > TRIAL_STATE_INIT_CONTACT)
		ExperimentSaveData(x1,y1,press,time_inc);
		
	//Automata de estados, bifurca el flujo del programa
	switch(g_trial_state)
	{
		case TRIAL_STATE_REC_ERROR:///////////////////////////////////////////
		case TRIAL_STATE_NOREC_ERROR:///////////////////////////////////////////
			if (time_inc >= ERROR_SCREEN_TIME )
			{
				g_trial_state = TRIAL_STATE_INTER_TRIAL;
				ExperimentGraphics();
			}
			break;
			
		case TRIAL_STATE_INTER_TRIAL://///////////////////////////////////
			if (!bRepeat)
			{
				if (!ExperimentSelect())
					return FALSE;
				ExperimentGraphics();
				bRepeat = TRUE;				
				g_init_time = now;
			}
			else if (time_inc >= INTER_TRIAL_TIME)
			{
				g_trial_state = TRIAL_STATE_PRECONTACT;
				ExperimentGraphics();
			}
			break;
			
		case TRIAL_STATE_PRECONTACT://///////////////////////////////////////
			if ( (press > MIN_PRESSURE) \
				&& (x < INIT_CIRCLE_AREA) && (x > -INIT_CIRCLE_AREA) \
				&& (y < INIT_CIRCLE_AREA) && (y > -INIT_CIRCLE_AREA) )
			{
				g_trial_state = TRIAL_STATE_INIT_CONTACT;
				ExperimentGraphics();
				g_init_time = 0;
			}
			break;
			
		case TRIAL_STATE_INIT_CONTACT://///////////////////////////////////////
			if ( press < MIN_PRESSURE )
			{
				g_error_msg = "Has levantado el lapiz demasiado";
				g_trial_state = TRIAL_STATE_REC_ERROR;
				bRepeat = TRUE;
				g_bHitTarget1 = g_bHitTarget2 = "no";
				//ExperimentSaveConfig();
				ExperimentGraphics();
				g_init_time = now;
			}
			else if ( (x > INIT_CIRCLE_AREA) || (y > INIT_CIRCLE_AREA) || (y < -INIT_CIRCLE_AREA))
			{
				g_trial_state = TRIAL_STATE_PRECHANGE;
				ExperimentGraphics();
				g_init_time = now;
			}
			break;
		case TRIAL_STATE_PRECHANGE://////////////////////////////////////////////
			if (press < MIN_PRESSURE)
			{
				g_error_msg = "Has levantado el lapiz demasiado";
				g_trial_state = TRIAL_STATE_REC_ERROR;
				bRepeat = TRUE;
				g_bHitTarget1 = g_bHitTarget2 = "no";
				//ExperimentSaveConfig();
				ExperimentGraphics();
				g_init_time = now;
			}
			else if (g_init_time >= g_config.time_of_jump)
			{
				g_trial_state = TRIAL_STATE_CONTACT1;
				ExperimentGraphics();
			}
			break;
		case TRIAL_STATE_CONTACT1://///////////////////////////////////////
			if (press < MIN_PRESSURE)
			{
				g_error_msg = "Has levantado el lapiz demasiado";
				g_trial_state = TRIAL_STATE_REC_ERROR;
				bRepeat = TRUE;
				g_bHitTarget1 = g_bHitTarget2 = "no";
				//ExperimentSaveConfig();
				ExperimentGraphics();
				g_init_time = now;
			}
			else if (time_inc > g_config.time_limit)
			{
				g_error_msg = "Te has pasado del tiempo total para responder";
				g_trial_state = TRIAL_STATE_REC_ERROR;
				bRepeat = TRUE;
				ExperimentGraphics();
				g_init_time = now;
			}
			else if ( x > (g_config.x2_target1 - TARGET1_WIDTH/2))
			{
				if  ( x > (g_config.x2_target1 + TARGET1_WIDTH/2))
				{
					//Tas pasao sin tocarlo!!!!
					g_trial_state = TRIAL_STATE_CONTACT2;
					g_bHitTarget1 = "no";
				}
				else if ((y > g_config.y2_target1 - g_config.h_target1/2) &&\
					     (y < g_config.y2_target1 + g_config.h_target1/2) )
				{
					g_trial_state = TRIAL_STATE_CONTACT2;
					g_bHitTarget1 = "yes";
				}
			}
			break;
		case TRIAL_STATE_CONTACT2:///////////////////////////////////////
			if (press < MIN_PRESSURE)
			{
				g_error_msg = "Has levantado el lapiz demasiado";
				g_trial_state = TRIAL_STATE_REC_ERROR;
				bRepeat = TRUE;
				g_bHitTarget1 = g_bHitTarget2 = "no";
				//ExperimentSave();
				ExperimentGraphics();
				g_init_time = now;
			}
			else if (time_inc > g_config.time_limit)
			{
				//Se te ha pasado el tiempo total!!
				g_error_msg = "Te has pasado del tiempo total para responder";
				g_trial_state = TRIAL_STATE_REC_ERROR;
				bRepeat = TRUE;
				g_bHitTarget1 = g_bHitTarget2 = "no";
				//ExperimentSaveConfig();
				ExperimentGraphics();
				g_init_time = now;
			}
			else if (x > g_config.x2_target2 - TARGET2_WIDTH/2)
			{
				if  (x > g_config.x2_target2 + TARGET2_WIDTH/2)
				{
					//Tas pasao sin tocarlo!!!!
					g_trial_state = TRIAL_STATE_NOREC_ERROR;
					g_bHitTarget2 = "no";
					ExperimentGraphics();
					ExperimentSaveConfig();
					fflush(g_GlobalFile);
					bRepeat = FALSE;
				}
				else if ((y > g_config.y2_target2 - g_config.h_target2/2) &&\
				         (y < g_config.y2_target2 + g_config.h_target2/2) )
				{
					if (g_bHitTarget1 == "no")
					{
						g_trial_state = TRIAL_STATE_NOREC_ERROR;
						ExperimentGraphics();
						ExperimentSaveConfig();
						fflush(g_GlobalFile);
						bRepeat = FALSE;
					}
					else
					{
						g_trial_state = TRIAL_STATE_INTER_TRIAL;
						g_bHitTarget2 = "yes";
						ExperimentSaveConfig();
						fflush(g_GlobalFile);
						bRepeat = FALSE;
					}
				}
			}
			break;
		default:
			ExperimentEnd();
			break;
	} //end_switch
	return TRUE;
}	//end_function


//------------------------------------------------------------  ExperimentGraphics()
//
static void ExperimentGraphics() {
	switch(g_trial_state)
	{
		case TRIAL_STATE_INTER_TRIAL:///////////////////////////////////////////
			ShowBackground(RGB_Gray);
			//ShowMessage();
			break;

		case TRIAL_STATE_PRECONTACT:///////////////////////////////////////////
			ShowBackground(RGB_Black);
			ShowInitPoint();
			break;

		case TRIAL_STATE_INIT_CONTACT:
		case TRIAL_STATE_PRECHANGE: //No es necesario pintar en este estado
		case TRIAL_STATE_CONTACT1:
		case TRIAL_STATE_CONTACT2:  //No es necesario pintar en este estado
			ShowBackground(RGB_Black);
			ShowTargets();
			break;

		case TRIAL_STATE_REC_ERROR://///////////////////////////////////////
			ShowBackground(RGB_Green);
			//ShowMessage(g_error_msg);
			break;
			
		case TRIAL_STATE_NOREC_ERROR://///////////////////////////////////////
			ShowBackground(RGB_Red);
			//ShowMessage(g_error_msg);
			break;
			
		default://////////////////////////////////////////////////////////////
			exit(APPERROR);
			break;
	}
	UpdateScreen();

}

//------------------------------------------------------------  ExperimentSaveConfig()
//
void ExperimentSaveConfig(){
	int i;
	fprintf( g_GlobalFile,\
			" %d\t %d\t %s\t %s\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t  %f\t %f ",\
			 g_trial_number, g_trial_type,\
			 g_bHitTarget1, g_bHitTarget2,\
			 g_config.x_start, g_config.y_start, g_config.r_start,\
			 g_config.x1_target1,g_config.y1_target1,g_config.x2_target1,g_config.y2_target1,g_config.h_target1,\
			 g_config.x1_target2,g_config.y1_target2,g_config.x2_target2,g_config.y2_target2,g_config.h_target2,\
			 g_config.time_of_jump,g_config.time_limit);
	for (i=0;i<g_config_number;i++)
		fprintf( g_GlobalFile,"%d ", (*G_Config[i]).replications);
	fprintf( g_GlobalFile,"\n");
	//for (i=0;i<g_DataCount;i++)
	//	fprintf( g_DataFile, " %f\t %f \t %d\t %f \n", g_TrialData[i].x, g_TrialData[i].y, g_TrialData[i].press, g_TrialData[i].time );
	//g_DataCount = -1;
}

//------------------------------------------------------------  ExperimentSaveData()
//
void ExperimentSaveData(float x, float y, int press, int time_inc){
	static int enter_times = 0;
	enter_times++;
	fprintf(g_DataFile, "state: %d looped_times: %d", g_trial_state, enter_times);
	fprintf( g_DataFile, " %f\t %f \t %d\t %ld \n", x,y,press,getns());
}
	


//------------------------------------------------------------  ExperimentEventMask()
//
static void ExperimentEventMask(){
	// Ignorar todos los eventos que no van a usarse
	SDL_EventState(SDL_KEYUP, SDL_IGNORE);
	SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
	SDL_EventState(SDL_JOYAXISMOTION, SDL_IGNORE);
	SDL_EventState(SDL_JOYBALLMOTION, SDL_IGNORE);
	SDL_EventState(SDL_JOYHATMOTION, SDL_IGNORE);
	SDL_EventState(SDL_JOYBUTTONDOWN, SDL_IGNORE);
	SDL_EventState(SDL_JOYBUTTONUP, SDL_IGNORE);
	SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
	SDL_EventState(SDL_VIDEORESIZE, SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

}
