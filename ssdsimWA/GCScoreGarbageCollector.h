
#ifndef _GCSCORE_GARBAGE_COLLECTOR_H_
#define _GCSCORE_GARBAGE_COLLECTOR_H_

#include "freepoolgarbagecollector.h"
class GCScoreGarbageCollector :
	public FreePoolGarbageCollector
{
public:
	GCScoreGarbageCollector(int pool_size, SimParameters& sp, Stat& st, StateManager& _sm, Flash& fm, MappingTable& mtable);
	~GCScoreGarbageCollector(void);

protected:
	// Get the score of a block to determine which one to erase
	double getGCscore (int b);

	// Find the block to erase, over-ride the one in FreePoolGarbageCollector
	virtual int findBlockToErase();

	// gamma that is used for the gc-score
	double lambda;
};

#endif
