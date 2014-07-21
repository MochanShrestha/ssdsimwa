#include "FlashMap.h"

#include <algorithm>
using namespace std;

#include "board.h"
using namespace LibBoard;

// Stuff from windows.h that the HSL to RGB uses

typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef unsigned short WORD;

typedef DWORD COLORREF;
typedef unsigned int uint;

#define sGetRValue(rgb)   ((BYTE) (rgb)) 
#define sGetGValue(rgb)   ((BYTE) (((WORD) (rgb)) >> 8)) 
#define sGetBValue(rgb)   ((BYTE) ((rgb) >> 16)) 

#define sRGB(r, g ,b)  ((DWORD) (((BYTE) (r) | ((WORD) (g) << 8)) | (((DWORD) (BYTE) (b)) << 16))) 


// -------------------------------------------------
// -------------------------------------------------
// Colorspace routines -----------------------------
// -------------------------------------------------
// -------------------------------------------------

// This is a subfunction of HSLtoRGB
void HSLtoRGB_Subfunction(uint& c, const double& temp1, const double& temp2, const double& temp3)
{
	if((temp3 * 6) < 1)
		c = (uint)((temp2 + (temp1 - temp2)*6*temp3)*100);
	else
	if((temp3 * 2) < 1)
		c = (uint)(temp1*100);
	else
	if((temp3 * 3) < 2)
		c = (uint)((temp2 + (temp1 - temp2)*(.66666 - temp3)*6)*100);
	else
		c = (uint)(temp2*100);
	return;
}

// This function extracts the hue, saturation, and luminance from "color" 
// and places these values in h, s, and l respectively.
void RGBtoHSL(const COLORREF& color,uint& h, uint& s, uint& l)
{
	uint r = (uint)sGetRValue(color);
	uint g = (uint)sGetGValue(color);
	uint b = (uint)sGetBValue(color);

	double r_percent = ((double)r)/255;
	double g_percent = ((double)g)/255;
	double b_percent = ((double)b)/255;

	double max_color = 0;
	if((r_percent >= g_percent) && (r_percent >= b_percent))
		max_color = r_percent;
	if((g_percent >= r_percent) && (g_percent >= b_percent))
		max_color = g_percent;
	if((b_percent >= r_percent) && (b_percent >= g_percent))
		max_color = b_percent;

	double min_color = 0;
	if((r_percent <= g_percent) && (r_percent <= b_percent))
		min_color = r_percent;
	if((g_percent <= r_percent) && (g_percent <= b_percent))
		min_color = g_percent;
	if((b_percent <= r_percent) && (b_percent <= g_percent))
		min_color = b_percent;

	double L = 0;
	double S = 0;
	double H = 0;

	L = (max_color + min_color)/2;

	if(max_color == min_color)
	{
		S = 0;
		H = 0;
	}
	else
	{
		if(L < .50)
		{
			S = (max_color - min_color)/(max_color + min_color);
		}
		else
		{
			S = (max_color - min_color)/(2 - max_color - min_color);
		}
		if(max_color == r_percent)
		{
			H = (g_percent - b_percent)/(max_color - min_color);
		}
		if(max_color == g_percent)
		{
			H = 2 + (b_percent - r_percent)/(max_color - min_color);
		}
		if(max_color == b_percent)
		{
			H = 4 + (r_percent - g_percent)/(max_color - min_color);
		}
	}
	s = (uint)(S*100);
	l = (uint)(L*100);
	H = H*60;
	if(H < 0)
		H += 360;
	h = (uint)H;
}

// This function converts the "color" object to the equivalent RGB values of
// the hue, saturation, and luminance passed as h, s, and l respectively
void HSLtoRGB(const double& H, const double& S, const double& L, COLORREF& color)
{
	uint r = 0;
	uint g = 0;
	uint b = 0;

	if(S == 0)
	{
		r = (uint)L*100;
		g = (uint)L*100;
		b = (uint)L*100;
	}
	else
	{
		double temp1 = 0;
		if(L < .50)
		{
			temp1 = L*(1 + S);
		}
		else
		{
			temp1 = L + S - (L*S);
		}

		double temp2 = 2*L - temp1;

		double temp3 = 0;
		for(int i = 0 ; i < 3 ; i++)
		{
			switch(i)
			{
			case 0: // red
				{
					temp3 = H + .33333;
					if(temp3 > 1)
						temp3 -= 1;
					HSLtoRGB_Subfunction(r,temp1,temp2,temp3);
					break;
				}
			case 1: // green
				{
					temp3 = H;
					HSLtoRGB_Subfunction(g,temp1,temp2,temp3);
					break;
				}
			case 2: // blue
				{
					temp3 = H - .33333;
					if(temp3 < 0)
						temp3 += 1;
					HSLtoRGB_Subfunction(b,temp1,temp2,temp3);
					break;
				}
			default:
				{

				}
			}
		}
	}
	r = (uint)((((double)r)/100)*255);
	g = (uint)((((double)g)/100)*255);
	b = (uint)((((double)b)/100)*255);
	color = sRGB(r,g,b);
}

void HSLtoRGB(const uint& h, const uint& s, const uint& l, COLORREF& color)
{
	double L = ((double)l)/100;
	double S = ((double)s)/100;
	double H = ((double)h)/360;

	HSLtoRGB (H, S, L, color);
}

// ------------------------------------------------------------
// ------ Flash Map routines ----------------------------------
// ------------------------------------------------------------

FlashMap::FlashMap(void)
{
}


FlashMap::~FlashMap(void)
{
}

#define SCALE  0.66

void FlashMap::outputTempMap (SimParameters& sp, Flash& f, StateManager& sm, string outfile)
{
	Board board;

	// Go through each block
	for (int i=0; i<sp.n; i++)
	{
		// Go through each page in the block
		for (int j=0; j<sp.c; j++)
		{
			Page& p = f.getBlock(i).getPage(j);
			double page_ts = p.getTimestamp();

			// Set the color of the square depending on the temperature of the page
			double page_temp = 1 - page_ts / sp.current_timestamp;
			COLORREF color;
			HSLtoRGB (page_temp*SCALE, 0.5, 0.5, color);
			board.setPenColorRGBi ( sGetRValue(color), sGetGValue(color), sGetBValue(color) );

			// Write the square that signifies the state of the page
			int row = i / sp.blocks_per_row;
			int mval = i % sp.blocks_per_row;
			int col = sp.c*mval + mval + j;
			board.fillRectangle ((float)col, (float)row, 1.0, 1.0);
		}
	}

	board.saveEPS ( outfile.c_str() );
}

typedef std::pair<double, int> my_pair;
bool sort_pred(const my_pair& left, const my_pair& right)
{
	return left.first > right.first;
}
bool sort_rpred(const my_pair& left, const my_pair& right)
{
	return left.first < right.first;
}

/*
------------ Color calculation functions --------------------------------------
*/

double FlashMap::calcColor (double page_ts, double cur_ts)
{
	double color;

	color =	1 - page_ts / cur_ts;

	return color;
}

double FlashMap::calcColorWin (double page_ts, double cur_ts, double window)
{
	double color;

	if ( cur_ts > window )
	{
		if ( cur_ts - window > page_ts )
			page_ts	= 0;
		else
			page_ts = page_ts - (cur_ts - window);
		cur_ts	= window;
	}
	color =	1 - page_ts / cur_ts;

	return color;
}

double FlashMap::calcColorpow (double page_ts, double cur_ts, double powv)
{
	double color;

	color =	pow (1 - page_ts / cur_ts, powv);

	return color;
}

/*
----------------------------------------------------------------------------------
*/



void FlashMap::glOutputMap (SimParameters& sp, Flash& f, StateManager& sm, unsigned char* buffer, int width, int height, int rwidth)
{
	// This holds our sorted data
	std::vector<my_pair> data;

	if ( sp.hm_sort )
	{
		// Go through the flash memory and make a list of the sum of the timestamps. 
		for (int i=0; i<sp.n; i++)
		{
			double t_ts = 0.0;
			for (int j=0; j<sp.c; j++)
			{
				if ( f.getBlock(i).getPage(j).isValid() )
					t_ts += ( sp.current_timestamp - f.getBlock(i).getPage(j).getTimestamp() );
				else if ( f.getBlock(i).getPage(j).isClean() )
				{
					// Quick hack so that the fully invalid pages stay above the clean pages
					t_ts += -0.0001;
				}
			}
			data.push_back ( my_pair (t_ts, i) );
		}
	}

	// Sort the data
	std::sort(data.begin(), data.end(), sort_rpred);


	// Figure out how many blocks we are going to draw
	int maxBlocks = height*sp.hm_blocksperrow;
	int reqBlocks = sp.n/(sp.hm_blocksample);
	int Ni = reqBlocks>maxBlocks ? maxBlocks: reqBlocks;

	// Go through each block
	for (int i=0; i<Ni; i++)
	{
		// This is the block that we are going to display in the map
		//int blockNo = sp.hm_blocksample*i;
		int blockNo;
		if ( sp.hm_sort )
			blockNo = data[sp.hm_blocksample*i].second;
		else
			blockNo = sp.hm_blocksample*i;

		// Go through each page in the block
		for (int j=0; j<sp.c; j++)
		{
			// Iterate through each page of the block
			Page& p = f.getBlock(blockNo).getPage(j);
			double page_ts = p.getTimestamp();

			// Set the color of the square depending on the temperature of the page
			COLORREF color;
			if (p.getState() == 1)
			{
				//double page_temp = calcColor (page_ts, sp.current_timestamp);
				//double page_temp = calcColorWin (page_ts, sp.current_timestamp, sp.n*sp.c*2.0f );
				double page_temp = calcColorpow (page_ts, sp.current_timestamp, 0.5 );
				HSLtoRGB (page_temp*SCALE, 0.5, 0.5, color);
				//board.setPenColorRGBi ( sGetRValue(color), sGetGValue(color), sGetBValue(color) );
			}
			else if (p.isInvalid() )
			{
				color = 0x77777777;
			}
			else
			{
				// Clean page
				color = 0x22222222;
			}

			// Write the square that signifies the state of the page
			int row = i / sp.hm_blocksperrow;
			int mval = i % sp.hm_blocksperrow;
			int col = 3 * (row*width + rwidth*mval + j);
			//int col = 3*i*128 + 3*j;
			
			buffer[col]			= sGetRValue(color);
			buffer[col + 1]		= sGetGValue(color);
			buffer[col + 2]		= sGetBValue(color);
			//board.fillRectangle ((float)col, (float)row, 1.0, 1.0);
		}
	}
}


void FlashMap::outputTempMapSorted (SimParameters& sp, Flash& f, StateManager& sm, string outfile)
{
	// This holds our sorted data
	std::vector<my_pair> data;

	// Go through the flash memory and make a list of the sum of the timestamps. 
	for (int i=0; i<sp.n; i++)
	{
		double t_ts = 0.0;
		for (int j=0; j<sp.c; j++)
		{
			t_ts += f.getBlock(i).getPage(j).getTimestamp();
		}
		data.push_back ( my_pair (t_ts, i) );
	}

	// Sort the data
	std::sort(data.begin(), data.end(), sort_pred);

	// Draw the graph

	Board board;

	// The number of blocks that have been written out
	int outblocks = 0;

	// Go through each block
	for (int i=0; i<sp.n; i+=sp.block_sample)
	{
		int block_i = data[i].second;
		Block& b = f.getBlock(block_i);
		// Don't output the clean and write blocks
		if ( sp.block_sample > 1 && sm.getState ( block_i) != BLOCK_STATUS_NONE )
			continue;

		// Go through each page in the block
		for (int j=0; j<sp.c; j++)
		{
			Page& p = b.getPage(j);
			double page_ts = p.getTimestamp();

			// Set the color of the square depending on the temperature of the page
			if (p.getState() == 1)
			{
				double page_temp = 1 - page_ts / sp.current_timestamp;
				page_temp = pow (page_temp, 0.5);
				COLORREF color;
				HSLtoRGB (page_temp*SCALE, 0.7, 0.7, color);
				board.setPenColorRGBi ( sGetRValue(color), sGetGValue(color), sGetBValue(color) );
			}
			else if (p.getState() == 0)
			{
				board.setPenColorRGBi ( 0,0,0 );
			}

			// Write the square that signifies the state of the page
			int row = outblocks / sp.blocks_per_row;
			int mval = outblocks % sp.blocks_per_row;
			int col = sp.c*mval + mval + j;
			board.fillRectangle ((float)col, (float)row, 1.0, 1.0);
		}
		outblocks++;
	}

	board.saveEPS ( outfile.c_str() );
}

void FlashMap::outputValidMap (SimParameters& sp, Flash& f, StateManager& sm, string outfile)
{
	Board board;

	// These are the colors that we will be using 
	Color color_white (255, 255, 255);
	Color color_black (0, 0, 0);
	Color color_grey (150, 150, 150);

	// Go through each block
	for (int i=0; i<sp.n; i++)
	{
		// Go through each page in the block
		for (int j=0; j<sp.c; j++)
		{
			int state = f.getBlock(i).getPage(j).getState();

			// Set the color of the square depending on the state of the page
			if ( state == 1 ) {
				// valid
				board.setPenColor ( color_grey );
			} else if ( state == 0 )
			{
				// invalid
				board.setPenColor ( color_black );
			} else	{
				if ( sm.getState(i) == BLOCK_STATUS_WRITE )
				{
					// clean block
					board.setPenColor ( color_white );
				}
				else if ( sm.getState(i) == BLOCK_STATUS_CLEAN )
				{
					// clean block. color is green
					board.setPenColor ( color_white );
				}
				else
				{
					// clean block. color is white
					board.setPenColorRGBi( 255, 255, 255 );
				}
			}

			int row = i / sp.blocks_per_row;
			int mval = i % sp.blocks_per_row;
			int col = sp.c*mval + mval + j;

			board.fillRectangle ((float)col, (float)row, 1.0, 1.0);
		}
	}

	board.saveEPS ( outfile.c_str() );
}

void FlashMap::outputTempChart ( string outfile )
{
	Board board;

	#define NBOXES 100

	for (int i=0; i<NBOXES; i++)
	{
		COLORREF color;
		HSLtoRGB (i/(double)NBOXES*SCALE, 0.5, 0.5, color);
		board.setPenColorRGBi ( sGetRValue(color), sGetGValue(color), sGetBValue(color) );
		board.fillRectangle ((float)i, (float)0, 1.0, 1.0);
	}

	board.scale(10);

	board.saveSVG ( outfile.c_str() );
}