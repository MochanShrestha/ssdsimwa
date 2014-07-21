#ifndef _BLOCK_H_
#define _BLOCK_H_

#include "Page.h"

class Block
{
public:
	Block();
	~Block(void);

	void setC (int c, char* memory);

	// Get the number of valid pages in the block
	int getValidF() const;

	// Erases the blocks. Sets all the pages to clean
	void Erase(double time);

	void setValid (int p, double ts, const char* data, long long int ws);
	void setInvalid (int p);
	void setClean (int p);

	bool isValid(int p) const;
	bool isInvalid(int p) const;
	bool isClean(int p) const;

	void setSector (int p, int sector);
	int getSector (int p);

	Page& getPage(int p);

	// get the erase count of the block
	int getEraseCount() { return eraseCount; }

	// Copyback the page at address a into the pth page of the block
	void copyback ( int pi, Page& p );

	// Regioning accessing and setting functions
	int getRegion() { return region; }
	void setRegion (const int r) { region = r; }

protected:
	// Number of pages in this block
	int c;
	// The array of pages
	Page* pages;
	// The number of valid pages
	//int vpages;
	
	// The number of times the block has been erased
	int eraseCount;

	// The time when it was last erased
	double lastErased;

	// The region for the block if regioning is used
	int region;
public:
		// returns the last time the block was erased
	double getLastErased() { return lastErased; }
};

#endif