#include "InPlaceGarbageCollector.h"


InPlaceGarbageCollector::InPlaceGarbageCollector(SimParameters& sp, Stat& st, StateManager& _sm, Flash& fm, MappingTable& mtable) : GarbageCollector (sp, st, _sm, fm, mtable)
{
}


InPlaceGarbageCollector::~InPlaceGarbageCollector(void)
{
}

int InPlaceGarbageCollector::findBlockToErase()
{
	return sm.findBlockWithLeastValid();
}

Address InPlaceGarbageCollector::getCleanBlock()
{
	Address a;

	// First find a block to erase
	a.setBlock ( findBlockToErase() );
	//cout << a.block << " : " << f.getBlock(a.block).getValid() << endl;
	stat.addBlockStat ( f, sm, a.getBlock() );

	// copyback the block to itself
	a.setPage  ( copyback (a.getBlock(), a.getBlock()) );

	// Clear the rest of the pages as being clean
	for (int i=a.getPage(); i<sp.c; i++)
	{
		f.getBlock(a.getBlock()).setClean(i);
	}

	return a;
}