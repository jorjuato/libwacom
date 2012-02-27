//////////////////////////////////////////////////////////////////
//  timing.c
//
//
// Copyleft (L)		Febrero 2010		Jorge Ibáñez
//
//////////////////////////////////////////////////////////////////

#include "timing.h"

struct timeval now_tv, next_tv;

void InitTiming(){
	gettimeofday(&now_tv,NULL);
	next_tv.tv_sec = now_tv.tv_sec;
	next_tv.tv_usec = now_tv.tv_usec + TICK_INTERVAL*1000;
	round_timeval(&next_tv);
}
//------------------------------------------------------------  SetDelay()
//
void SetDelay(long int ns){
	struct timespec ts_req, ts_rem;
	if (ns == 0) return;
	ts_req.tv_sec = 0;
	ts_req.tv_nsec = ns;
	/* Recursive call in case of signal reception*/
	if (nanosleep(&ts_req, &ts_rem)==-1)
		SetDelay(ts_rem.tv_nsec);
}



//------------------------------------------------------------  ExperimentTimeLeft()
//
long int ExperimentTimeLeft() {
	static struct timeval tvdiff;
	gettimeofday(&now_tv,NULL);
	if(timeval_substract(&tvdiff,next_tv,now_tv)) {
		next_tv.tv_usec = now_tv.tv_usec + TICK_INTERVAL*1000;
		round_timeval(&next_tv);
		return 0;
	}
	return tvdiff.tv_usec*1000;	
}

//------------------------------------------------------------  timeval_substract()
//
static int timeval_substract (struct timeval *result, struct timeval x, struct timeval y) {
	/* Perform the carry for the later subtraction by updating y. */
	if (x.tv_usec < y.tv_usec) {
		int nsec = (y.tv_usec - x.tv_usec) / 1000000 + 1;
		y.tv_usec -= 1000000 * nsec;
		y.tv_sec += nsec;
	}
	if (x.tv_usec - y.tv_usec > 1000000) {
		int nsec = (x.tv_usec - y.tv_usec) / 1000000;
		y.tv_usec += 1000000 * nsec;
		y.tv_sec -= nsec;
	}

	/* Compute the time remaining to wait. tv_usec is certainly positive. */
	result->tv_sec = x.tv_sec - y.tv_sec;
	result->tv_usec = x.tv_usec - y.tv_usec;

	/* Return 1 if result is negative. */
	return x.tv_sec < y.tv_sec;
}

//------------------------------------------------------------  round_timeval()
//
static void round_timeval(struct timeval* t) {
    if (t->tv_usec < 0) {
		t->tv_usec += ONE_MILLION ;
		t->tv_sec -- ;
    } 
    else if (t->tv_usec >= ONE_MILLION) {
		t->tv_usec -= ONE_MILLION ;
		t->tv_sec ++ ;
    }
}

//------------------------------------------------------------  getns()
//
long int getns() {	
	static struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_usec;
}


////------------------------------------------------------------  ExperimentTimeLeft()
////
//static long int ExperimentTimeLeft()
//{
	//static Uint32 now, next_time=0;

	//if( next_time <= (now = SDL_GetTicks()) )
		//next_time = now + TICK_INTERVAL;
    //else
		//return (long int)((next_time - now)*1000000);
	//return 0;
//}
