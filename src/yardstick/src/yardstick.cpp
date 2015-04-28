#include "yardstick.h"
#include <iostream>
#include <iomanip>

#define BILLION 1000000000

double toSeconds(struct timespec ts)
{
	return (double)ts.tv_sec + ((double)ts.tv_nsec / (double)BILLION);
}

struct timespec
timespec_diff(struct timespec start, struct timespec end)
{
  struct timespec diff;

  if ( end.tv_nsec - start.tv_nsec < 0 )
  {
    // If the nano seconds go below zero we will want to shift 1 second to
    // the nano seconds.
    end.tv_sec  -= 1;
    end.tv_nsec += BILLION; 
  }

  diff.tv_sec  = end.tv_sec - start.tv_sec;
  diff.tv_nsec = end.tv_nsec - start.tv_nsec; 

  return diff;
}


struct timespec
timespec_avg ( const std::list<struct timespec> &time_trials )
{
  time_t nano_sum = 0;
  struct timespec ts_avg;
  std::list<struct timespec>::const_iterator itr = time_trials.begin();
  
  for ( ; itr != time_trials.end(); itr++ )
  {
    nano_sum += (*itr).tv_nsec + ((*itr).tv_sec * BILLION);
  }
  
  nano_sum = (time_t) nano_sum / (time_t) time_trials.size(); 
  ts_avg.tv_sec  = (time_t) nano_sum / (time_t) BILLION;
  ts_avg.tv_nsec = (time_t) nano_sum % (time_t) BILLION;
 
  return ts_avg;
}

struct timespec
timespec_round_avg ( struct timespec ts_avg, int runs )
{
  time_t nano_sum;
  
  nano_sum = ts_avg.tv_nsec + ts_avg.tv_sec * BILLION;

  nano_sum = (time_t) nano_sum / (time_t) runs; 
  ts_avg.tv_sec  = (time_t) nano_sum / (time_t) BILLION;
  ts_avg.tv_nsec = (time_t) nano_sum % (time_t) BILLION;
 
  return ts_avg;
}

Yardstick::Yardstick()
{
    paused  = false;
    running = false;
}


Yardstick::~Yardstick()
{

}

void
Yardstick::start()
{
    running = true;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_start);
}

struct timespec
Yardstick::next()
{

    struct timespec elapsed = end();
    start();

    return elapsed;

}

struct timespec
Yardstick::end()
{
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_end);
    running = false;
    
    struct timespec elapsed = timespec_diff(ts_start, ts_end);
    time_trials.push_back(elapsed);

    return elapsed;
}

struct timespec
Yardstick::std_dev()
{
	double average = toSeconds(avg());
	double sumDev = 0;
	for(std::list<struct timespec>::iterator i = time_trials.begin(); i != time_trials.end();i++)
	{
		sumDev += pow(toSeconds((*i)) - average,2);
	}

	sumDev /= time_trials.size()-1;
	
	sumDev = sqrt(sumDev);
	struct timespec stddev;
	stddev.tv_sec = (int)sumDev;
	stddev.tv_nsec = (sumDev - stddev.tv_sec) * BILLION;
	return stddev;
}

double
Yardstick::total()
{
	return toSeconds(avg()) * time_trials.size();
}

void Yardstick::stop()
{
	running = false;
}

struct timespec
Yardstick::avg()
{
   return timespec_avg(time_trials);
}


