/************************************************************************##
** 							libparser						 			 ##
**																		 ##
**			Jorge Ibáñez				Diciembre 2006					 ##
**																		 ##
**																		 ##
**																		 ##
**  Libreria que contiene funciones para parsear la linea de comando y	 ##
**  responder apropiadamente al usuario. Muy básico, sirve de referencia ##
**																		 ##
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "pili.h"
#include "parser.h"
#define PILI_VERSION "0.0.1"

extern const char*	g_subject_name;
extern CONFIG*		G_Config[MAX_CONF_NUMBER];
extern int g_config_number;
//------------------------------------------------------------   int ParseCmdLine()
//

void ParseCmdLine(int argc, char** argv) {
	const char* pa;

	++argv;
	
	if (argc == 1)
		Usage(0);
	

	while ((pa = *(argv++)) != NULL)
	{
		if (pa[0] == '-')
		{
			if ((strcmp(pa,"-h") == 0) || (strcmp(pa,"--help") == 0))
				Usage(0);
			else if ((strcmp(pa,"-V") == 0) || (strcmp(pa,"-v") == 0) || (strcmp(pa,"--version") == 0))
				{ Version(); Usage(0); exit(0); }
			else
				{ Usage(0); Fatal("Unknown option %s\n",pa);}

		}
		else if (!g_subject_name)
			g_subject_name = pa;
		else
			{ Usage(0); Fatal("Unknown argument %s\n",pa);}
	}
}

//------------------------------------------------------------   int ParseConfFile()
//
void ParseConfFile(const char* filename){
	char line[256];
	int line_number = -1;
	FILE *input_file = NULL;
	const char* rptr = NULL;


	input_file = fopen(filename, "r");
	if (!input_file)
		exit(0);

	while( (rptr = fgets(line, sizeof(line), input_file)) )
	{
		if (line_number>-1)
			G_Config[line_number] = read_config(rptr);
		line_number++;
	}
	printf("Leido archivo de configuracion\n");
	fclose(input_file);
}


CONFIG *read_config(const char* rptr)
{
	CONFIG *conf = NULL;
	conf = (CONFIG *) malloc(sizeof(CONFIG));
	g_config_number++;
	sscanf( rptr, "%f %f %f %f %f   %f %f %f %f %f    %f %f %f %f  %f %d",\
				 &((*conf).x_start),\
				 &((*conf).y_start),\
				 &((*conf).r_start),\
				 &((*conf).x1_target1),\
				 &((*conf).y1_target1),\
				 &((*conf).x2_target1),\
				 &((*conf).y2_target1),\
				 &((*conf).h_target1),\
				 &((*conf).x1_target2),\
				 &((*conf).y1_target2),\
				 &((*conf).x2_target2),\
				 &((*conf).y2_target2),\
				 &((*conf).h_target2),\
				 &((*conf).time_of_jump),\
				 &((*conf).time_limit),\
				 &((*conf).replications));
	printf( "%f %f %f %f %f   %f %f %f %f %f    %f %f %f %f  %f %d\n",\
				 ((*conf).x_start),\
				 ((*conf).y_start),\
				 ((*conf).r_start),\
				 ((*conf).x1_target1),\
				 ((*conf).y1_target1),\
				 ((*conf).x2_target1),\
				 ((*conf).y2_target1),\
				 ((*conf).h_target1),\
				 ((*conf).x1_target2),\
				 ((*conf).y1_target2),\
				 ((*conf).x2_target2),\
				 ((*conf).y2_target2),\
				 ((*conf).h_target2),\
				 ((*conf).time_of_jump),\
				 ((*conf).time_limit),\
				 ((*conf).replications));
	fflush(stdout);
	return conf;
}

//------------------------------------------------------------   void Usage(int rtn)
//
static void Usage(int rtn)
{
	FILE* f = rtn ? stderr : stdout;

	fprintf(f, "Usage: pili [options] subject_name\n"
			"  -h, --help                - usage\n"
			"  -V, --version             - version\n"
			"\n"
			"Use --list option for input_device choices\n");
	exit(rtn);
}

//------------------------------------------------------------   void Version(void)
//
static void Version(void)
{
	fprintf(stdout,"%s\n",PILI_VERSION);
}

//------------------------------------------------------------   void Fatal(const char*)
//
static void Fatal(const char* pszFmt, ...)
{
	va_list args;
	va_start(args,pszFmt);
	vfprintf(stderr,pszFmt,args);
	va_end(args);
	exit(1);
}


