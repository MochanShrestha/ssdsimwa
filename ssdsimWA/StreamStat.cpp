#include "StreamStat.h"

#include <float.h>

StreamStat::StreamStat(void)
{
	// Set the default values
	n = 0;
	max = -DBL_MAX;
	min = DBL_MAX;
	avg = 0.0;
}


StreamStat::~StreamStat(void)
{
}

void StreamStat::add ( double x )
{
	// Update that we have addded one more number
	n++;

	// Update the max
	if ( x > max )
	{
		max = x;
	}

	// Update the min
	if ( x < min )
	{
		min = x;
	}

	// Update the average
	avg = ( avg*(n-1) + x ) / (double)n;

	// update the values for the variance
	if ( n == 1)
	{
		M = x;
		S = 0;
	}
	else
	{
		double _M = M + (x - M)/n;
		S = S + (x - M)*(x - _M);
		M = _M;
	}
}