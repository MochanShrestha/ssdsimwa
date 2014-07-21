
#include "WriteStampLayoutManager.h"

WriteStampLayoutManager::WriteStampLayoutManager(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& cb_wb, int start) : IntervalLayoutManager (s, _sm, fm, cb_wb, start)
{
}

int WriteStampLayoutManager::getPageMetric ( const Page& p )
{
	return (int)( sp.writestamp - p.getWriteStamp() );
}

double WriteStampLayoutManager::getStatMetric ( const Page& p )
{
	return (double)p.getCopybackCount();
}