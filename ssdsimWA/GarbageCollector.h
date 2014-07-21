
#ifndef _GARBAGECOLLECTOR_H_
#define _GARBAGECOLLECTOR_H_

#include "Flash.h"
#include "SimParameters.h"
#include "Stat.h"
#include "MappingTable.h"
#include "LayoutManager.h"
#include "StateManager.h"

#define GARBCOL_WRITE		1
#define GARBCOL_COPYBACK	2

class GarbageCollector
{
public:
#ifdef LMTEST
	GarbageCollector(SimParameters& sp, StateManager& _sm, Flash& fm, MappingTable& mtable);
#else
	GarbageCollector(SimParameters& sp, Stat& st, StateManager& _sm, Flash& fm, MappingTable& mtable);
#endif
	virtual ~GarbageCollector(void);

	// Returns a block to start writing on.
	virtual Address getCleanBlock(int options = 0) = NULL;

	// Performs all the copybacks. LayoutManager tells where to put the copyback pages
	virtual void copybackBlocks(LayoutManager& lm) = NULL;

protected:
	Flash& f;
	SimParameters& sp;
#ifndef LMTEST
	Stat& stat;
#endif
	MappingTable& mt;
	StateManager& sm;

	int copyback (int b1, int b2);

	void copybacklm (LayoutManager& lm, int bi);
};

#endif
