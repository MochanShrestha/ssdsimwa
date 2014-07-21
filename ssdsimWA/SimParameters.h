#ifndef _SIMPARAMETERS_H_
#define _SIMPARAMETERS_H_

#include <string>

#include "workload.h"
#include "cfgparser.h"

#include "UnifRandWorkload.h"
#include "ZipfWorkload.h"
#include "DatTraceWorkload.h"
#include "TRACE12Workload.h"
#include "TRACE3Workload.h"

#include "HashData.h"

class SimParameters
{
public:
	SimParameters(string simconfig);
	~SimParameters(void);

public:
	// The name of the file we load the configurations from
	string cfgfilename;

	//
	bool startfullsectors;	
	// The length of time we will simulate for
	long long int l;
	// The number of blocks
	int n;
	// pages per block
	int c;
	// the size of a page
	int psize;

	// the amount of overprovisioning
	double overp;
	// number of sectors
	int m;
	// the ratio of the reads and writes
	double r;
	// the ratio of the sectors that are static
	double static_sectors;
	// Factor used to determine the value of l
	int lfactor;

	// write amplification distribution logfile
	string wadistlog;
	string wwadistlog;

	string watlog;
	string wwatlog;
	string cblog;
	string debuglog;
	string wearlog;
	string erasecountslog;
	string mapeps;
	int blocks_per_row;
	int block_sample;
	string volfilename;
	string cbwsfilename;
	string tstotalfilename;
	string finalCBcounts;
	string lmstats;

	// The snap-shop files stuff
	int snap_int;
	string snap_prefix;
	string wsnap_prefix;

	string wtype;
	int wdist;

	string tfiletype;
	string tfilename;

	// ------ garbage collection variables
	// the algorithm string
	string gc_algorithm;
	// size of the free block pool that we are looking at
	int pool_size;
	// the lambda that chooses between the erase counts and valid pagess
	double lambda;

	// ------- layout manager variables
	// the layout manager algorithm
	string lm_walgorithm;
	string lm_wrules;
	string lm_cbalgorithm;
	string lm_cbrules;

	// -------- Volatility Calculations
	int volUnits, volBinSize;

	// -------- Cache
	int cachesize;

	// -------- Data for the hash
	// the name of the hash algorithm
	string hashalgorithm;
	// the size of the hash string
	int hash_size;
	// the numerical code of the hash algorithm
	int hash_algo;

	// ---------- Data for the visualizations
	bool viz;
	int viz_steps;

	string viz_writeblocks_s;
	string viz_heatmap_s;

	bool viz_writeblocks;
	bool viz_heatmap;

	// Paramters for how the meatmap is displayed
	int hm_blocksperrow;
	int hm_blocksample;
	// Will the heatmap show sorted or unsorted
	bool hm_sort;

	// Should the garbage collector not garbage collect region 1 blocks
	bool region1exclude;

	// ------------------------------------------------------------
	// -------- Dynamic Data Variables ----------------------------
	//       Updated as the data is updated
	// ------------------------------------------------------------
	double current_timestamp;
	// the current write stamp, the count of number of writes since start
	// of simulation
	long long int writestamp;

	// output the parameters
	friend ostream &operator<<(ostream &stream, SimParameters& sp);


	// Calculates the value of n from the values of m, c and over-provisioning
	void calcN() { n = (int) ceil ( m / ( c * overp ) ); }
};

#endif