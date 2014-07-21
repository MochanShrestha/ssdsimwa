
#ifndef _ZIPFWORKLOAD_H_
#define _ZIPFWORKLOAD_H_

#include "synthworkload.h"
class ZipfWorkload :
	public SynthWorkload
{
public:
	ZipfWorkload(int m, double r, double static_sectors, long long int l, int seed, bool start);
	~ZipfWorkload(void);

	Event getNextEvent();

	virtual double calcETTI (Event& e);

	// Get the pdf and cdf of the distribution
	double* getPDF() { return x; }
	double* getCDF() { return y; }

protected:
	// This is the Zipf distribution (pdf)
	double *x;
	// Zipf cdf
	double *y;

public:
	// For obtaining a Zipf sample from a random sample
	int findSmallestGreaterThan ( double d );
	int findSmallestGreaterThan_linear ( double d );
};

#endif
