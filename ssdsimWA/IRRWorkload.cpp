#include "IRRWorkload.h"


IRRWorkload::IRRWorkload(int m, int minR, int maxR, long long int l, unsigned long seed) : Workload (m, false), unif(0,m-1)
{
	// Seed the random number
	eng.seed (seed);

	min_range = minR;
	max_range = maxR;
	this->l = l;

	// Allocate the arrays
	selSectors = new int[m];

	sectorLastUpdate = new int[m];
	for (int j=0; j<m; j++)
		sectorLastUpdate[j] = j;

sectorTimeSinceUpdate = new int[m];
	for (int j=0; j<m; j++)
		sectorTimeSinceUpdate[j] = m - 1 - sectorLastUpdate[j];
}


IRRWorkload::~IRRWorkload(void)
{
	// Delete the allocated sectors
	delete[] sectorLastUpdate;
	delete[] sectorTimeSinceUpdate;
	delete[] selSectors;
}

Event IRRWorkload::getNextEvent()
{
	Event e;

	e.time = (double)counter;
	e.sector = calcSector();
	e.op = WORKLOAD_OP_WRITE;

	return e;
}


int IRRWorkload::calcSector()
{
	// The sector that is going to be selected on each loop
	int sector;
	bool sectorSelected;

	// Select the sectors in the range
	nSelSectors = 0;
	sectorSelected = false;
	for (int j=0; j<m; j++)
	{
		int timeSinceUpdate = sectorTimeSinceUpdate[j];

		// This sector must be selected now. No choice
		if ( timeSinceUpdate == max_range - 1 )
		{
			sector = j;
			sectorSelected = true;
			break;
		}

		// Select the sector
		if ( timeSinceUpdate < max_range && timeSinceUpdate >= min_range ) 
		{
			selSectors[nSelSectors++] = j;
		}
	}

	// Select a random sector between the choices we have
	if ( !sectorSelected)
	{
		if ( nSelSectors > 0)
			sector = selSectors [rand() % nSelSectors];
		else
			sector = unif(eng);
	}

	// Update when the sector was updated
	sectorLastUpdate[sector] = counter;

	// Update the times when the sector was last updated
	for (int j=0; j<m; j++)
	{
		sectorTimeSinceUpdate[j] += 1;
	}
	sectorTimeSinceUpdate[sector] = 0;

	return sector;
}