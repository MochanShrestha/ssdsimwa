
#ifndef _STATEMANAGER_H_
#define _STATEMANAGER_H_

#include "Flash.h"

#define BLOCK_STATUS_VIRGIN				-1
#define BLOCK_STATUS_NONE				 0
#define BLOCK_STATUS_CLEAN				 1
#define BLOCK_STATUS_WRITE				 2

class StateManager
{
public:
	StateManager(SimParameters& _sp, Flash& _f);
	virtual ~StateManager(void);

	// set the data and in the state manager. Changes the data for the number of valid pages in a block
	void incValid ( int b ) { assert(b < sp.n); vPages[b]++;  }
	void decValid ( int b ) { assert(b < sp.n); vPages[b]--;  }
	void clrValid ( int b ) { assert(b < sp.n); vPages[b] = 0; }
	int getnValid ( int b ) const { return vPages[b]; }
	void setValid ( int b, int v ) { assert(b < sp.n); vPages[b] = v; }

	// set and get the state of the block
	void setState ( int b, int state )  {  assert(b < sp.n); bStates[b] = state; }
	int  getState ( int b ) const       { return bStates[b]; }

	// get and set the cook times
	void setCookTime ( int b, double ct ) { cook_times[b] = ct; }
	double getCookTime (int b ) const     { return cook_times[b]; }

	// find the block with the lowest number of valid pages
	int findBlockWithLeastValid();

protected:
	// The simulation parameters
	SimParameters& sp;
	// The info of the flash memory
	Flash& f;

	// ---------- Block Information -----------------------
	// the number of valid pages in each block
	int* vPages;
	// the state of the block
	int* bStates;
	// the cook-times of the blocks
	double* cook_times;
};

#endif
