#include "SynthWorkload.h"
#include <random>
#include <math.h>

SynthWorkload::SynthWorkload(int m, double r, double _static, long long int l, unsigned long seed = (unsigned long)time(NULL), bool start) : Workload (m, start), unifd(0,1)
{
	this->seed = seed;
	this->r = r;
	this->l = l;
	static_sectors = _static;
	m2 = (int)floor ( (1 - static_sectors) * m );
	//cout << "static_sectors = " << static_sectors << endl;
	//cout << "m2 = " << m2 << endl << endl;
	unif = new std::tr1::uniform_int<int> (0, m2-1);

	gen.seed(seed);

	counter = 0;
}


SynthWorkload::~SynthWorkload(void)
{
	delete unif;
}

int SynthWorkload::randM()
{
	return (*unif)(gen);
}

double SynthWorkload::rand1()
{
	return unifd(gen);
}