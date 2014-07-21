#pragma once
#include "workload.h"
class IRRWorkload :
	public Workload
{
public:
	IRRWorkload(int m, int minR, int maxR, long long int l, unsigned long seed);
	~IRRWorkload(void);

	// Virtual functions to implement
	Event getNextEvent();

	bool eow() { return counter >= l; };

protected:
	// Random number generators
	std::tr1::mt19937 eng;
	std::tr1::uniform_int<int> unif;

	// This stores when the sector was last udpated and how long it has been
	int* sectorLastUpdate;

	// The time since the sector was updated
	int* sectorTimeSinceUpdate;

	// The temp storage of sectors that have been selected
	int* selSectors;
	int nSelSectors;

	// the range of values
	int min_range, max_range;

	// The length of the workload
	long long int l;

	// Calculates the next sector
	int calcSector();
};

