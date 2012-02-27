//////////////////////////////////////////////////////////////////
//  timing.h
//
//
// Copyleft (L)		Febrero 2010		Jorge Ibáñez
//
//////////////////////////////////////////////////////////////////


#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#define ONE_MILLION				1000000
#define TICK_INTERVAL  			4

void InitTiming();

void SetDelay(long int ns);

long int ExperimentTimeLeft();

static int timeval_substract(struct timeval *result, struct timeval x, struct timeval y);

static void round_timeval(struct timeval* t);

long int getns();
