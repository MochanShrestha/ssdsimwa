#pragma once

#include "Flash.h"
#include "Cache.h"
#include "MappingTable.h"
#include "LayoutManager.h"
#include "Event.h"
#include "Stat.h"

#include "GarbageCollector.h"
#include "InPlaceGarbageCollector.h"
#include "FreePoolGarbageCollector.h"
#include "SortedPoolGarbageCollector.h"
#include "GCScoreGarbageCollector.h"

#include "SingleWriteBlockLayoutManager.h"
#include "IntervalLayoutManager.h"
#include "CopybackBlocksLayoutManager.h"
#include "WriteStampLayoutManager.h"
#include "ETTILayoutManager.h"

class FTL
{
public:
#ifdef LMTEST
	FTL(Flash& f, Cache& c, SimParameters& _sp);
#else
	FTL(Flash& f, Cache& c, SimParameters& _sp, Stat& st);
#endif
	~FTL(void);

	void operate (Event& e);

	void writeOp (Event& e);
	void deleteOp (Event& e);

	// Gets the address from the mapping table
	Address getAddress (int sector) { return mt.getAddress(sector); }

	// Done with the simulation.
	void close();


	// Get the layout manager
	LayoutManager* getLayoutManager() { return lm; }

	// Get the state Manager
	StateManager& getStateManager() { return sm; }

	// Get the mapping table
	MappingTable& getMappingTable() { return mt; }

protected:
	// The simulation paramters
	SimParameters& sp;
	// The mapping tables to store the translation of the sectors to pages
	MappingTable mt;
	// Takes care of where to write
	LayoutManager *lm;
	// Takes care of the garbage collection
	GarbageCollector* gc;
	// Manages the information about the state of the flash memory
	StateManager sm;

	// The actual data array
	Flash& f;
	// The cache where we store temporary data
	Cache& c;


	// makes a sector invalid
	void invalidSector (int sector);
	void invalidSectorCache (int sector, Address& a);
	void invalidSectorDisk  (int sector, Address& a);

	// write a sector to disk
	void writeToDisk (Event& e);

#ifndef LMTEST
	Stat& stat;
#endif
};

