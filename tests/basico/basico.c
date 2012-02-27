

#include "basico.h"
SDL_Surface *screen;

//------------------------------------------------------------  main()
//
int main(int argc, char **argv){
	OnInit();						
	WacomInit(WACOM_SCANMODE_NOCALLBACK, NULL);
	ExperimentLoop();				
	WacomClose();
	return(APPSUCCESS);				
}

//------------------------------------------------------------  MainLoop()
//
static void ExperimentLoop()
{
    SDL_Event event;

	//Bucle indefinido que lleva a cabo el experimento
    while(TRUE)
    {
        //Implementacion del timer sin usar señales
		SDL_Delay(ExperimentTimeLeft());

        //Funcion principal para regular los estados del experimento
        //Contiene los condicionales asociados a la salida de WACOMlib
    	if (!ExperimentTimer())
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
int ExperimentTimer(void){

	Uint32 now,time_inc;
	float x,y;
	Uint16 press;
	static Uint8 bRepeat = FALSE;

	//Actualizamos variables temporales
	now = SDL_GetTicks();
	time_inc = now - g_init_time;

	//Actualizamos los variables de la tableta
	x     = WAC_SCALE_X(gAbsState[WACOMFIELD_POSITION_X].nValue);
	y     = WAC_SCALE_Y(gAbsState[WACOMFIELD_POSITION_Y].nValue);
	press = gAbsState[WACOMFIELD_PRESSURE].nValue;
	
	//Salvamos datos
	g_TrialData[g_DataCount].x = x;
	g_TrialData[g_DataCount].y = y;
	g_TrialData[g_DataCount].time = time_inc;
	g_TrialData[g_DataCount].press = press;
	
	//pintamos
	ShowPoint(x,y,press/50)	
}

//------------------------------------------------------------  ExperimentTimeLeft()
//
static int ExperimentTimeLeft()
{
	static Uint32 now, next_time=0;

	if( next_time <= (now = SDL_GetTicks()) )
	{
		next_time = now + TICK_INTERVAL;
        return 0;
	}
    else
		return(next_time - now);
}



////////////////////////////////////////////////////////////////////////
//  AUXILIAR GRAPHICAL FUNCTIONS
////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------  ShowFixation()
//
 void ShowPoint(x,y,r){
 	xt = (Sint16) ABS_SCALE_X(x);
 	yt = (Sint16) ABS_SCALE_Y(y);
 	rt = (Uint16) REL_SCALE_X(r);
	filledCircleColor(screen,xt,yt,rt,RGBA_Cyan);
}

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

//------------------------------------------------------------  OnInit()
// Inicializa la libreria gráfica, el logging system y carga imágenes
void OnInit() {

    if (!(screen = setup_sdl()) )
	{
		perror("Couldn't initialize video");
		exit(APPERROR);
	}
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
	SDL_ShowCursor(FALSE);
	//WacomInit(WACOM_SCANMODE_NOCALLBACK, NULL);
	//WacomInit(WACOM_SCANMODE_CALLBACK, ExperimentHandler);
}

//------------------------------------------------------------  OnExit()
// Salir limpiamente de SDL
void OnExit() {
	SDL_Quit();
}

//------------------------------------------------------------  OnDraw()
//Refresca la pantalla cuando se recibe un evento de repintado del WM
void OnDraw() {
	SDL_FillRect(screen, NULL, 0);
	SDL_Flip(screen);
}

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
    //if( (screen = SDL_SetVideoMode( WIDTH, HEIGHT, video->vfmt->BitsPerPixel, SDL_DOUBLEBUF )) == NULL )
    if( (screen = SDL_SetVideoMode( WIDTH, HEIGHT, video->vfmt->BitsPerPixel, SDL_DOUBLEBUF | SDL_FULLSCREEN )) == NULL )
	{
        fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
        return(ERROR);
    }
    return screen;

}
