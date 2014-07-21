#include "CopybackBlocksLayoutManager.h"


CopybackBlocksLayoutManager::CopybackBlocksLayoutManager(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& cb_wb, int start) : IntervalLayoutManager (s, _sm, fm, cb_wb, start)
{
}

int CopybackBlocksLayoutManager::getPageMetric ( const Page& p )
{
	return p.getCopybackCount();
}

double CopybackBlocksLayoutManager::getStatMetric ( const Page& p )
{
	return (double)(sp.writestamp - p.getWriteStamp());
}