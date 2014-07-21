
#ifndef _INPLACEGARBAGECOLLECTOR_H_
#define _INPLACEGARBAGECOLLECTOR_H_

#include "garbagecollector.h"
class InPlaceGarbageCollector :
	public GarbageCollector
{
public:
	InPlaceGarbageCollector(SimParameters& sp, Stat& st, StateManager& _sm, Flash& fm, MappingTable& mtable);
	~InPlaceGarbageCollector(void);

	Address getCleanBlock();

protected:
	// Find the block to erase
	int findBlockToErase();
};

#endif