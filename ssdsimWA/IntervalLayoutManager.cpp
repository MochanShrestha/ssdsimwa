
#include "IntervalLayoutManager.h"

IntervalLayoutManager::IntervalLayoutManager(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& cb_wb, int start) : CopybackLayoutManager (s, _sm, fm)
{
	cb_wb = new WriteBlocks (s.lm_cbrules, start, s, _sm);
	this->cb_wb = cb_wb;
}


IntervalLayoutManager::~IntervalLayoutManager(void)
{
	//delete[] useCount;
	//delete[] copybackStats;
}

Address IntervalLayoutManager::getNextCopyback(const Page& p)
{
	Address a;
	int cb_block;

	assert ( !cb_wb->needNewBlock() );
	if ( !cb_wb->needNewBlock() )
	{
		// Decide on the write block based on the copyback count of the page
		int cb_metric = getPageMetric (p);
		cb_block = cb_wb->findWriteBlock ( cb_metric );

		// Update the stat for the page
		//copybackStats[cb_block].add ( getStatMetric(p) );

		// Get the next page in the write block
		a = cb_wb->getNextWriteAddress (cb_block);
	}

	return a;
}