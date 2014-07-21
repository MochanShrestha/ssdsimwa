
#ifndef _2REGIONLAYOUTMANAGER_H_
#define _2REGIONLAYOUTMANAGER_H_

#include "writelayoutmanager.h"

class Region2LayoutManager : public WriteLayoutManager
{
public:
	Region2LayoutManager(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& w_wb, int start);
	~Region2LayoutManager(void);

	// virtual functions that need to be implemented
	Address getNextWrite(const Event* e);
};

#endif