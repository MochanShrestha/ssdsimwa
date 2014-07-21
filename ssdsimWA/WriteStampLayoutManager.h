
#ifndef _WRITESTAMPLAYOUTMANAGER_H_
#define _WRITESTAMPLAYOUTMANAGER_H_

#include "IntervalLayoutManager.h"
#include "CBRules.h"

class WriteStampLayoutManager : 
	public IntervalLayoutManager
{
public:
	WriteStampLayoutManager(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& cb_wb, int start);

protected:
	virtual int getPageMetric ( const Page& p );

	virtual double getStatMetric ( const Page& p );
};

#endif