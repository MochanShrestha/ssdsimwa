#include "StateManager.h"


StateManager::StateManager(SimParameters& _sp, Flash& _f) : sp(_sp), f(_f)
{
	vPages = new int[sp.n];
	for (int i=0; i<sp.n; i++)
	{
		vPages[i] = 0;
	}

	bStates = new int[sp.n];
	for (int i=0; i<sp.n; i++)
	{
		bStates[i] = BLOCK_STATUS_VIRGIN;
	}

	cook_times = new double[sp.n];
	for (int i=0; i<sp.n; i++)
	{
		cook_times[i] = -1.0;
	}
}


StateManager::~StateManager(void)
{
	delete[] bStates;
	delete[] vPages;
}


int StateManager::findBlockWithLeastValid()
{
	int minBlock = 0;
	int minValid = getnValid(0);

	// Loop through all the blocks and find the block with the lowest amount of valid blocks to erase
	for (int i=0; i<sp.n; i++)
	{
		if ( minValid > getnValid(i) )
		{
			minBlock = i;
			minValid = getnValid(i);
		}

		// If this has no valid pages, then we use this
		if ( minValid == 0 )
		{
			return minBlock;
		}
	}

	// This is the smallest we found. So, stop her.e
	return minBlock;
}