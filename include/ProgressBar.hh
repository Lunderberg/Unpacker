#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <cstdlib>

class ProgressBar {
public:
	ProgressBar(size_t total,size_t skip);
	~ProgressBar();
	void Show(size_t current);
private:
	size_t total;
	size_t update_every;
	size_t iter_since_update;
	double time_start;
};

#endif

