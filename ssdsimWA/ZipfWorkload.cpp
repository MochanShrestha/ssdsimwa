#include "ZipfWorkload.h"

#include <iostream>
using namespace std;

ZipfWorkload::ZipfWorkload(int m, double r, double static_sectors, long long int l, int seed, bool start) : SynthWorkload (m, r, static_sectors, l, seed, start)
{
	x = new double[m2];
	y = new double[m2];

	// Fill in the Zipf distribution
	int i;
	double s = 1.0;
	double sum = 0.0;

	// Create the Zipf PDF
	for (i=0; i<m2; i++)
		x[i] = pow (i+1, -s);
	for (i=0; i<m2; i++)
		sum += x[i];
	for (i=0; i<m2; i++)
		x[i] /= sum;

	// Turn it into a CDF
	memcpy (y, x, m2*sizeof(double));
	for (i=1; i<m2; i++)
		y[i] += y[i-1];
}


ZipfWorkload::~ZipfWorkload(void)
{
	delete[] x;
	delete[] y;
}

Event ZipfWorkload::getNextEvent()
{
	// Set the event
	Event e;
	e.time = (double)counter;
	e.sector = findSmallestGreaterThan ( rand1() );
	//e.sector = findSmallestGreaterThan_linear ( rand1() );
	e.op = getRandOp();

	return e;
}

int ZipfWorkload::findSmallestGreaterThan ( double d )
{
	int startv = 0;
	int endv = m2-1;
	int center;
	double cx;

	if ( d <= y[startv] )
		return startv;

	// Do a binary search of the data
	while ( endv - startv > 1 )
	{
		center = (endv - startv) / 2 + startv;
		cx = y[center];
		if ( d >= cx )
		{
			startv = center;
		}
		else
		{
			endv = center;
		}
	}

	return endv;
}

int ZipfWorkload::findSmallestGreaterThan_linear ( double d )
{
	for (int i=0; i<m2; i++)
	{
		if ( d <= y[i] )
		{
			return i;
		}
	}
	return m2-1;
}

double ZipfWorkload::calcETTI (Event& e)
{
	double xi = x[e.sector];
	return 1/xi;
}