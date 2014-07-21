#include "Block.h"
#include <assert.h>

Block::Block(void)
{
	c = -1;

	// This will incremented every time the Erase function is called
	eraseCount = 0;

	// The block has never been erased
	lastErased = -1;

	// Remove the out of the band information
	region = 0;
}


Block::~Block(void)
{
	delete[] pages;
}

void Block::setC (int c, char* memory)
{
	this->c = c;
	pages = new Page[c];

	for (int i=0; i<c; i++)
	{
		pages[i].setMemory (memory + i*Page::getMemSize());
	}
}

int Block::getValidF() const
{
	int validN = 0;
	for (int i=0; i<c; i++)
	{
		if (pages[i].getState() == 1)
		{
			validN++;
		}
	}
	return validN;
}

void Block::Erase(double time)
{
	assert (c>0);

	// Set all the pages to be clean and the block to have no valid pages
	for (int i=0; i<c; i++)
	{
		pages[i].setClean();
	}

	// Increment the erase count
	eraseCount++;

	// Remove the out of band data from the block
	// Set the erase time
	lastErased = time;
	region = 0;
}

void Block::setValid (int p, double ts, const char* data, long long int ws)
{
	assert(p>=0 && p<c);
	pages[p].setValid(ts, data, ws);
}

void Block::setInvalid (int p)
{
	assert(p>=0 && p<c);
	pages[p].setInvalid();
}

void Block::setClean (int p)
{
	assert(p>=0 && p<c);
	pages[p].setClean();
}

int Block::getSector (int p)
{
	assert(p>=0 && p<c);
	return pages[p].getSector();
}

void Block::setSector (int p, int sector)
{
	assert(p>=0 && p<c);
	pages[p].setSector(sector);
}

bool Block::isValid(int p) const
{
	assert(p>=0 && p<c);
	return pages[p].isValid();
}

bool Block::isInvalid(int p) const
{
	assert(p>=0 && p<c);
	return pages[p].isInvalid();
}

bool Block::isClean(int p) const
{
	assert(p>=0 && p<c);
	return pages[p].isClean();
}

Page& Block::getPage(int p)
{
	assert(p>=0 && p<c);
	return pages[p];
}

void Block::copyback ( int pi, Page& p)
{
	assert (pi>=0 && pi<c);
	pages[pi] = p;
}