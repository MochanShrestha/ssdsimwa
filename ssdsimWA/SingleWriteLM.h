
#ifndef _SINGLEWRITELM_H_
#define _SINGLEWRITELM_H_

#include "writelayoutmanager.h"

class SingleWriteLM : public WriteLayoutManager
{
public:
	SingleWriteLM(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& w_wb, int start);
	~SingleWriteLM(void);

	// virtual functions that need to be implemented
	Address getNextWrite(const Event* e);
};

#endif