
#ifndef _SSDSIM_H_
#define _SSDSIM_H_

#include "SimParameters.h"
#include "FTL.h"
#include "Stat.h"
#include "Cache.h"

#include <GL/glut.h>

class SSDSim
{
public:
	SSDSim(SimParameters& s, Stat& st);
	~SSDSim(void);

	// Starts the simulation
	void start();

	// Glut functions (standard stuff)
	void renderScene2 (void);

	// Update the window size of the heat map window
	void updateHeatmapWindowSize (int w, int h) { hmw_w = w; hmw_h = h; }

protected:
	SimParameters& sp;

	Stat& stat;

	Flash f;
	Cache c;
	FTL ftl;

	// --- Big OpenGL functions

	// run this for trace files
	void start1();
	// run this for synthetic files
	void start2();
	// Function for the glut
	void glutStart2();
	// This is one step of the simulation
	inline int simStep2 ();

	// --- Helper OpenGL functions

	// Draws the writeblock into the currrent selected window
	void drawWriteblocks();

	// Draws the heatmap
	void drawHeatmap();

	// This is the array of the heatmap that we will be using
	int hm_w, hm_h;
	int hmw_w, hmw_h;
	unsigned char* heatmap_buffer;

	// Draws the block stack of block b in location (x,y)
	void drawBlockStack (int b, int blockUseCount, float x, float y);


	// start time for the percent complete calculations
	clock_t start_time;
	// calculate the percentage complete and the start time
	inline void printPercentComplete (long long int i);

	// ---------------------------------------------------------------------------
	// End of simulation check functions
	// ---------------------------------------------------------------------------
	void simCheck();
	// Check that the mapping table points to a page with the right sector
	bool mapPageSectorCheck();
	// Check that the hash data stored matches the sector and timestamp
	bool hashDataCheck();

	// The hash for the current event. The size of the hash will be determined by
	// the hash function to use in the sim parameters
	char* hash;

	// calls the HashData class to perform the hashing
	void hashEvent (Event& e);

	// The workload object that we will use to create events
	Workload *w;

	// GLUT variable
	// simulation running index;
	long long int sim_i;

	// The indices of the glut windows
	int glwin_writeblocks;
	int glwin_heatmap;
};

#endif