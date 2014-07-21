
#ifndef _UNIFRANDWORKLOAD_
#define _UNIFRANDWORKLOAD_

#include "SynthWorkload.h"

class UnifRandWorkload :
	public SynthWorkload
{
public:
	UnifRandWorkload(int m, double r, double static_sectors, long long int l, int seed, bool start);
	~UnifRandWorkload(void);

	Event getNextEvent();

	virtual double calcETTI (Event& e);
};

#endif