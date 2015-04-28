#ifndef __YARDSTICK_H__
#define __YARDSTICK_H__

#include <ctime>
#include <cmath>
#include <list>
#include <iostream>
#include <string>

using std::string;
using std::cout;

class Yardstick
{
    public:
        Yardstick();
        ~Yardstick();
        
        //
        // Start the benchmark from a stopped state (end)
        //
        // Conditional: running == false
        //
        void start();

        //
        // Pause the benchmark in the middle of a run
        // 
        // Conditional: running == true
        //
        //void pause();
       
        // 
        // Stop the current run and start a new one
        //
        // Conditional: running == true
        //
        struct timespec next();
        
        //
        // End the current run
        // and push back a trial
        //
        // Conditional: running == true
        //
        struct timespec end();

		//
		// End the current run
		// and dont push back a tial
		//
		// Conditional: running == true
        //
        void stop();

        // Return an average of all the current time trials
        //
        struct timespec avg();

		//Return the standard deviation across all trials
		//
		struct timespec std_dev();

        //
        // Return the standard deviation of the current time trials
        //
        //struct timespec std_dev();

		double total();

		//
		// Print out formatted informationg about the trials.
		//
    private:
        bool running; 
        bool paused;
        std::list<struct timespec> time_trials; 
        struct timespec ts_start, ts_end;

};


#endif
