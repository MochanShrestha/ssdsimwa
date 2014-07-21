
#ifndef _VOLATILITYCALC_H_
#define _VOLATILITYCALC_H_

#include "SimParameters.h"
#include "FTL.h"
#include "Stat.h"

class VolatilityCalc
{
public:
	VolatilityCalc(SimParameters& s, Stat& st);
	~VolatilityCalc(void);

	// Starts the simulation
	void start();

protected:
	SimParameters& sp;
	Stat& stat;

	// We keep what was the last write-stamp when the page was updated
	int* sectorWritestamps;

	// We keep track of the number of times a sector was updated within the particular write-count
	int* stampCounts;

	// The workload object to create the events and data from
	Workload *w;
};

#endif