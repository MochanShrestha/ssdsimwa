
#ifndef _SORTED_POOL_GARBAGE_COLLECTOR_H_
#define _SORTED_POOL_GARBAGE_COLLECTOR_H_

#include "garbagecollector.h"
#include <set>

// Pointer to the flash memory object to be used for comparing the wear levels
extern Flash* sgc_f;

// Comparison class for set
class WearComp
{
public:
	bool operator()(int b1, int b2)
	{
		if( sgc_f->getBlock(b1).getEraseCount() <= sgc_f->getBlock(b2).getEraseCount() )
			return true;
		else
			return false;
	}
};

class SortedPoolGarbageCollector :
	public GarbageCollector
{
public:
	SortedPoolGarbageCollector(int pool_size, SimParameters& sp, Stat& st, StateManager& _sm, Flash& fm, MappingTable& mtable);
	~SortedPoolGarbageCollector(void);

	Address getCleanBlock(int option);
	void copybackBlocks(LayoutManager& lm);

protected:
	// Find the block to erase
	virtual int findBlockToErase();

	// The size of the garbage collection pool
	int pool_size;

	// Check that freeBlocks
	bool isFull() { return freeBlocks.size() == pool_size; }
	bool isEmpty() { return freeBlocks.size() == 0; }

	std::set<int, WearComp> freeBlocks;

	void insert ( int b );
};

#endif