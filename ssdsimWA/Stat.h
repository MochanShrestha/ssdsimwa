
#ifndef _STAT_H_
#define _STAT_H_

#include <fstream>
using namespace std;

#include "Flash.h"
#include "Block.h"
#include "SimParameters.h"
#include "StateManager.h"

#include "StreamStat.h"
#include "cQueue.h"

#define MAX_COPYBACKS 2500

class Stat
{
public:
	Stat(SimParameters& s);
	~Stat(void);

	// Set the values and stats
	void setParam (SimParameters& s);

	// The simulation has ended. Calculate wear distribution
	void simEnd (Flash& f, StateManager& sm);

	// calculates the write amplification
	double calcMean();
	double calcWA();
	double calcWWA();

	// calculates the system write amplification
	double calcsWA();

	// calculates the valid pages on erase (on the given window)
	double calcVPE();

	// ------------- Stats collected upon events ----------------------

	// This block is about to be erased. Get the stats of the block
	void addBlockStat(const Flash& f, const StateManager& sm, const int b);

	// Page Invalidation
	void pageInvalid(const Flash& f, const Address& a);

	// Page copyback
	void pageCopyback(const Page& p);

	int getTotalCopybacks() { return totalCopybacks; }

protected:
	ofstream statfile;				// outputs the write amplification distribution
	ofstream wwafile;				// outputs the window write amplification distribution
	ofstream stattfile;				// calculates the write amplification over time
	ofstream cbfile;				// outputs the copyback counts
	ofstream wsccfile;				// outputs the log of the 
	ofstream wwatfile;				// outputs the window wa at time intervals
	ofstream tstotalfile;			// Writes out the relationship between the timestamp and the total number of 

	bool logcbws;					// Log the copyback count versus the write count log

	ofstream debugfile;

	// When a block is erased, keeps track of how many pages were copied
	int* CEcounts;

	// total number of pages that have been copied back
	int totalCopybacks;

	// Total number of blocks that have been erased
	int total;
	
	// for calculating the windowed write amplification
	cQueue<int> wa_queue;
	int workload_pos;

	// This is the number of pages per block
	SimParameters& sp;

	// calculates the number of valid pages resident in the flash memory
	int numValidPages(const Flash& f);

	// store the copyback counts
	int copybackCounts[MAX_COPYBACKS];

	// We keep track of the number of times a sector was updated within the particular write-count
	long long int* stampCounts;

	//double *copybackWritestampMean;
	//int *copybackWritestampN;

	// the old write amplification value
	double old_wa;

	// Stats for the volatility of the workload
	int volUnits, volBinSize;

public:
	// This is the age between the times pages are garbage collected
	StreamStat gcAge;
};

#endif