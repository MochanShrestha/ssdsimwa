
#ifndef _SYNTHWORKLOAD_
#define _SYNTHWORKLOAD_

#include "workload.h"
class SynthWorkload :
	public Workload
{
public:
	SynthWorkload(int m, double r, double _static, long long int l, unsigned long seed, bool start = false);
	~SynthWorkload(void);

	bool eow() { return counter >= l; };

protected:
	// This is the random seed to create the strewam of random numbers
	int seed;
	
	// The ratio between read and write
	double r;
	
	// The ratio of blocks that are static, they are written to first and never accessed again
	double static_sectors;
	// This is the number of sectors that we will generate
	int m2;

	// The length of the workload
	long long int l;

	// Gives a random value between 0 and 1
	double rand1();
	// Gets a random operation
	int getRandOp() { return (rand1() < (1-r)) + 1; };

	// get a random number rounde
	int randM();

	std::tr1::uniform_int<int> *unif;
	//std::tr1::minstd_rand gen;			//Linear congruential generator
	std::tr1::mt19937 gen;					//Marsenne twister

	// random number generation
	std::tr1::uniform_real<double> unifd;
};

#endif