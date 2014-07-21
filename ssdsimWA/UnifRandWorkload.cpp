#include "UnifRandWorkload.h"

#include <stdlib.h>


UnifRandWorkload::UnifRandWorkload(int m, double r, double static_sectors, long long int l, int seed, bool start) : SynthWorkload(m,r,static_sectors,l,seed,start)
{
}


UnifRandWorkload::~UnifRandWorkload(void)
{
}

Event UnifRandWorkload::getNextEvent()
{
	// Set the event
	Event e;
	e.time = (double)counter;
	e.sector = randM();
	e.op = getRandOp();

	return e;
}

double UnifRandWorkload::calcETTI (Event& e)
{
	return (double)m;
}