
#ifndef _ETTILAYOUTMANAGER_H_
#define _ETTILAYOUTMANAGER_H_

#include "WriteLayoutManager.h"

class ETTILayoutManager :
	public WriteLayoutManager
{
public:
	ETTILayoutManager(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& w_wb, int start);
	~ETTILayoutManager(void);

	// virtual functions that need to be overwritten
	Address getNextWrite(const Event* e);

	// Check if we need a new block
	bool needNewBlock();

	// Set new block
	void setNewBlock (Address& a);
};

#endif