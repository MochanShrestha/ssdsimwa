
#ifndef _WRITELAYOUTMANAGER_H_
#define _WRITELAYOUTMANAGER_H_

#include "Address.h"
#include "SimParameters.h"
#include "Flash.h"
#include "StateManager.h"
#include "WriteBlocks.h"

class WriteLayoutManager
{
public:
	WriteLayoutManager(SimParameters& s, StateManager& _sm, Flash& fm);
	~WriteLayoutManager(void);

	virtual Address getNextWrite(const Event* e) = 0;

protected:
	// Reference to the parameters of the simulation
	SimParameters& sp;
	// Stores the states
	StateManager& sm;
	// Access to the raw flash memory values
	Flash& f;

	WriteBlocks* w_wb;
};

#endif