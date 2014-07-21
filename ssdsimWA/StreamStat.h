
#ifndef _STREAMSTAT_H_
#define _STREAMSTAT_H_

#include <math.h>

class StreamStat
{
public:
	StreamStat(void);
	~StreamStat(void);

	void add ( double x );

	// Get functions
	int getN() { return n; }
	double getAvg() { return avg; }
	double getMax() { return max; }
	double getMin() { return min; }
	double getVar() { if ( n > 1) { return S/(n-1); } else { return 0; } }
	double getStd() { return sqrt(getVar()); }

private:
	// the stats we keep track of : average, max, min
	double avg, max, min;

	// values used to keep track of the running variance
	double M, S;

	// how many numbers have been added
	int n;
};

#endif