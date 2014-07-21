
#ifndef _COPYBACKLAYOUTMANAGER_H_
#define _COPYBACKLAYOUTMANAGER_H_

#include "Address.h"
#include "SimParameters.h"
#include "Flash.h"
#include "StateManager.h"
#include "WriteBlocks.h"

class CopybackLayoutManager
{
public:
	CopybackLayoutManager(SimParameters& s, StateManager& _sm, Flash& fm);
	virtual ~CopybackLayoutManager(void);

	virtual Address getNextCopyback(const Page& p) = 0;

protected:
	// Reference to the parameters of the simulation
	SimParameters& sp;
	// Stores the states
	StateManager& sm;
	// Access to the raw flash memory values
	Flash& f;
};

#endif