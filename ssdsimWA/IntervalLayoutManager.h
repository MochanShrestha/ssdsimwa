
#ifndef _INTERVALLAYOUTMANAGER_H_
#define _INTERVALLAYOUTMANAGER_H_

/*
	Purpose of this class is to have an interval based layout management where an interval is given
	for rules and the copyback block is chosen from the set of intervals given.

	The specialization of the intervals to copyback counts or write-stamps is done for classes that inherit
	this class.
*/

#include "CopybackLayoutManager.h"
#include "CBRules.h"
#include "StreamStat.h"

class IntervalLayoutManager :
	public CopybackLayoutManager
{
public:
	IntervalLayoutManager(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& cb_wb, int start);
	virtual ~IntervalLayoutManager(void);

	Address getNextCopyback(const Page& p);

protected:
	// The writeblock for the copybacks
	WriteBlocks* cb_wb;

	// Get the address of the next page to write and increment the counter, set flags if necessary
	Address getNextWriteAddress (int b);

	// Get the metric used to choose the interval
	virtual int getPageMetric ( const Page& p ) = 0;

	// Get the metric to keep the stats for
	virtual double getStatMetric ( const Page& p ) = 0;

	// If we need clean blocks or not
	bool needBlock;
	int needBlockNo;
};

#endif