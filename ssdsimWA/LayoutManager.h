
#ifndef _LAYOUTMANAGER_H_
#define _LAYOUTMANAGER_H_

#include "Address.h"
#include "SimParameters.h"
#include "Flash.h"
#include "StateManager.h"
#include "WriteBlocks.h"

#include "WriteLayoutManager.h"
#include "SingleWriteLM.h"
#include "ETTILayoutManager.h"

#include "CopybackLayoutManager.h"
#include "SingleWriteBlockLayoutManager.h"
#include "WSingleWriteBlockLayoutManager.h"
#include "CopybackBlocksLayoutManager.h"
#include "WriteStampLayoutManager.h"
#include "2RegionLayoutManager.h"

class GarbageCollector;

// These are the flags for the layout maanger
#define FLAG_LM_ETTI	1

class LayoutManager
{
public:
	LayoutManager(SimParameters& sp, StateManager& sm, Flash& fm, GarbageCollector* _gc);
	~LayoutManager();

	Address getNextWrite(const Event* e);
	Address getNextCopyback(const Page& p);

	// Check if we need a new block
	bool needNewCBBlock();

	// Set new block if we need a new block (called above)
	void setNewCBBlock (Address& a);


	// Returns the flag for the Layout Manager
	int getFlag() { return flag; }

	// Gives the number of blocks used as writeblocks
	int getNWriteBlocks(); 

	// Get function for the writeblocks
	WriteBlocks* getWriteBlocks() { return w_wb; }
	WriteBlocks* getCopybackBlocks() { return cb_wb; }

	// Output the status to the console
	friend ostream &operator<<(ostream &stream, LayoutManager& wb);

private:
	// This is the garbage collector needed to get new writeBlocks
	GarbageCollector* gc;

	// The LM for writes and copybacks
	WriteLayoutManager    *wlm;
	CopybackLayoutManager *cblm;

	// The reference to the current simulation parameters and the state
	SimParameters& sp;
	StateManager& sm;

	// The flash memory object
	Flash& f;

	// The WriteBlocks for the write and copyback layout managers
	WriteBlocks* w_wb;
	WriteBlocks* cb_wb;

	// Flags that shows the status of the Layout Manager
	int flag;

	// Calculates the cook time for the block
	void calcCookTime (int b, double p);
};

#endif