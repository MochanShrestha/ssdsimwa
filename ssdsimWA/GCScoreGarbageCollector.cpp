#include "GCScoreGarbageCollector.h"

#include <math.h>
#include <queue>


GCScoreGarbageCollector::GCScoreGarbageCollector(int pool_size, SimParameters& sp, Stat& st, StateManager& _sm, Flash& fm, MappingTable& mtable)
	:FreePoolGarbageCollector(pool_size, sp, st, _sm, fm, mtable)
{
	lambda = sp.lambda;
}


GCScoreGarbageCollector::~GCScoreGarbageCollector(void)
{
}

double GCScoreGarbageCollector::getGCscore (int b)
{
	// Get the number of valid and the erase count
	int nValid = sm.getnValid(b);
	int eraseCount = f.getBlock(b).getEraseCount();

	// Calculate the GC-Score for the block
	double score = lambda * ( nValid / (double)sp.c );
	//double score = (double)eraseCount;

	return score;
}

int GCScoreGarbageCollector::findBlockToErase()
{
	double minScore = DBL_MAX;
	int minBlock = -1;

	// Queue to store the number of mininums found
	queue<int> mins;

	// Loop through all the blocks and find the block with the lowest score to erase
	for (int i=0; i<sp.n; i++)
	{
		// Ignore the free/write blocks since it has no valid blocks
		if ( sm.getState (i) > 0)
			continue;

		if ( sm.getCookTime(i) > sp.current_timestamp )
			continue;

		// Get the gc-score of the block
		double score = getGCscore (i);

		// See if this is the min or not
		if ( score < minScore )
		{
			minScore = score;
			minBlock = i;

			// Clear the queue and add the minBlock to it
			while ( !mins.empty() )
				mins.pop();
			mins.push (minBlock);
		}
		else if ( score == minScore )
		{
			mins.push (i);
		}
	}

	// Check how many are in the queue. If 1 we just return the minBlock. Otherwise, choose one randomly
	if ( mins.size() == 1)
	{
		return minBlock;
	}
	else
	{
		// randomly choose a value from the queue
		int nMins = mins.size();
		int randVal = rand() % nMins;
		for (int j=0; j<randVal; j++)
			mins.pop();
		return mins.front();
	}
}