
#ifndef _FLASHMAP_H_
#define _FLASHMAP_H_

#include "Flash.h"
#include "StateManager.h"

class FlashMap
{
public:
	FlashMap(void);
	~FlashMap(void);

	static void outputTempMap (SimParameters& sp, Flash& f, StateManager& sm, string outfile);
	static void outputTempMapSorted (SimParameters& sp, Flash& f, StateManager& sm, string outfile);

	// Outputs the heatmap to outfile as EPS using the board library
	static void outputValidMap (SimParameters& sp, Flash& f, StateManager& sm, string outfile);

	// Outputs the heatmap to the gl window
	static void glOutputMap (SimParameters& sp, Flash& f, StateManager& sm, unsigned char* buffer, int width, int height, int rwidth);

	static void outputTempChart ( string outfile );

private:
	// Equations to calculate the color of the heatmap

	// Simple one that just linearly estimates the history. Problem is the red shift with history.
	static inline double calcColor (double page_ts, double cur_ts);

	// Calculates the color but limits itself to the window only
	static inline double calcColorWin (double page_ts, double cur_ts, double window);

	// Exponentiates the time so >1 pow gives more range to recent, <1 more to past
	static inline double calcColorpow (double page_ts, double cur_ts, double powv);
};

#endif