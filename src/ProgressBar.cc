#include "ProgressBar.hh"

#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/time.h>

double get_time(){
    struct timeval t;
    gettimeofday(&t, NULL);
    double d = t.tv_sec + (double) t.tv_usec/1000000;
    return d;
}

std::string format_tdiff(double tdiff){
	std::stringstream ss;
	if(tdiff >= 3600){
		int hours = tdiff/3600;
		ss << hours << "h";
		tdiff -= 3600*hours;
	}
	if(tdiff >= 60){
		int minutes = tdiff/60;
		ss << minutes << "m";
		tdiff -= 60*minutes;
	}

	int seconds = tdiff;
	ss << seconds << "s";

	return ss.str();
}

ProgressBar::ProgressBar(size_t total, size_t update_every)
	: iter_since_update(-1) {
	this->total = total;
	this->update_every = update_every;
	time_start = get_time();
}

void ProgressBar::Show(size_t current) {
	if(++iter_since_update < update_every){
		return;
	}
	iter_since_update = 0;

        std::ios state(NULL);
        state.copyfmt(std::cout);

	if (total > 0) {
		double tdiff = get_time() - time_start;
		double frac_completed = double(current)/total;
		double tremaining = (1-frac_completed) * (tdiff/frac_completed);
		std::cout << "\r" << current << "/" << total
			  << " (" << std::fixed << std::setprecision(1) << frac_completed*100 << "%, "
			  << format_tdiff(tremaining) << ")"
			  << std::flush;
	} else {
		std::cout << "\r" << current
			  << std::flush;
	}

        std::cout.copyfmt(state);
}

ProgressBar::~ProgressBar() {
	std::cout << std::endl;
}


