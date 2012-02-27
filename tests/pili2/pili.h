//////////////////////////////////////////////////////////////////
//  pili.h
//
//	Cabeceras para definir parámetros que caracterizan
//	el experimento de Pili
//
//
//
//
// Copyleft (L)		Septiembre 2008		Jorge Ibáñez
//
//////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//		CONSTANTES 	DEL EXPERIMENTO
//////////////////////////////////////////////////////////////////

//GENERAL CONSTANTS OF EXPERIMENT
#define MAX_CONF_NUMBER 		10
#define MIN_PRESSURE			50
#define INTER_TRIAL_TIME 		500
#define ERROR_SCREEN_TIME 		1000


//SPATIAL PROPERTIES OF STIMULI
#define TARGET1_WIDTH			1.0
//#define TARGET1_HEIGHT			4
#define TARGET2_WIDTH			1.0
//#define TARGET2_HEIGHT			4
//#define INIT_CIRCLE_RADIO		0.2
#define INIT_CIRCLE_AREA		0.2




//////////////////////////////////////////////////////////////////
//		ESTRUCTURA DE DATOS PARA ALMACENAR CONFIGURACIONES
//////////////////////////////////////////////////////////////////

typedef enum
{
	TRIAL_STATE_REC_ERROR,
	TRIAL_STATE_NOREC_ERROR,
	TRIAL_STATE_INTER_TRIAL,
	TRIAL_STATE_PRECONTACT,
	TRIAL_STATE_INIT_CONTACT,
	TRIAL_STATE_PRECHANGE,
	TRIAL_STATE_CONTACT1,
	TRIAL_STATE_CONTACT2,
	TRIAL_STATE_MAX
} TRIAL_STATE;



typedef struct {
	float x_start;
	float y_start;
	float r_start;
	float x1_target1;
	float y1_target1;
	float x2_target1;
	float y2_target1;
	float h_target1;
	float x1_target2;
	float y1_target2;
	float x2_target2;
	float y2_target2;
	float h_target2;
	float time_of_jump;
	float time_limit;
	int replications;
} CONFIG;

typedef struct {
	float x;
	float y;
	float time;
	int   press;
} TRIALDATA;

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



