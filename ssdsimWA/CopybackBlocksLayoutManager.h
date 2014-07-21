
#ifndef _COPYBACKBLOCKSLAYOUTMANAGER_H_
#define _COPYBACKBLOCKSLAYOUTMANAGER_H_

#include "IntervalLayoutManager.h"
#include "CBRules.h"

class CopybackBlocksLayoutManager :
	public IntervalLayoutManager
{
public:
	CopybackBlocksLayoutManager(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& cb_wb, int start);

protected:
	virtual int getPageMetric ( const Page& p );

	virtual double getStatMetric ( const Page& p );
};

#endif