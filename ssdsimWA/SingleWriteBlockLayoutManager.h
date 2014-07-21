
#ifndef _SINGLEWRITEBLOCKLAYOUTMANAGER_H_
#define _SINGLEWRITEBLOCKLAYOUTMANAGER_H_

#include "CopybackLayoutManager.h"

class SingleWriteBlockLayoutManager :
	public CopybackLayoutManager
{
public:
	SingleWriteBlockLayoutManager(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& cb_wb, int start);
	~SingleWriteBlockLayoutManager(void);

	Address getNextCopyback(const Page& p);

protected:
	WriteBlocks* cb_wb;
};

#endif