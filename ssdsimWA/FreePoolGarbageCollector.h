
#ifndef _FREEPOOLGARBAGECOLLECTOR_H_
#define _FREEPOOLGARBAGECOLLECTOR_H_

#include "garbagecollector.h"
#include "cQueue.h"
#include "cQueue.cpp"

class FreePoolGarbageCollector :
	public GarbageCollector
{
public:
#ifdef LMTEST
	FreePoolGarbageCollector(int pool_size, SimParameters& sp, StateManager& _sm, Flash& fm, MappingTable& mtable);
#else
	FreePoolGarbageCollector(int pool_size, SimParameters& sp, Stat& st, StateManager& _sm, Flash& fm, MappingTable& mtable);
#endif
	~FreePoolGarbageCollector(void);

	Address getCleanBlock(int option = 0);
	void copybackBlocks(LayoutManager& lm);

	void setRegion1exclude (bool flag) { region1exclude = flag; }

protected:
	// Find the block to erase
	virtual int findBlockToErase();

	// Circular queue for the freeblocks
	cQueue<int> freeBlocks;

	bool region1exclude;
};

#endif