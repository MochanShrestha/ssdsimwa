#include "Flash.h"

#include <assert.h>


Flash::Flash(SimParameters& _sp) : sp(_sp)
{
	// Allocate the memory
	if ( sp.hash_size > 0 )
	{
		memory = new char[sp.n * sp.c * sp.hash_size];
	}
	else
	{
		memory = NULL;
	}

	// Allocate the blocks array
	blocks = new Block[sp.n];
	Page::setMemSize (sp.hash_size);

	// Set the blocks up so it can allocate the pages
	for (int i=0; i<sp.n; i++)
	{
		// set the block size and the memory for the logical block
		blocks[i].setC(sp.c, memory + i*sp.c*sp.hash_size);
	}
}


Flash::~Flash(void)
{
	if ( memory )
		delete[] memory;
}

void Flash::setValid (Address& a, int sector, double ts, const char* data, long long int ws) 
{
	// Make sure the page is clean
	assert (blocks[a.getBlock()].isClean(a.getPage()));

	// Set the page as valid and add in the data for it
	blocks[a.getBlock()].setValid (a.getPage(), ts, data, ws);
	blocks[a.getBlock()].setSector(a.getPage(), sector);
}

void Flash::setInvalid (Address& a) 
{
	assert (blocks[a.getBlock()].isValid(a.getPage()));

	blocks[a.getBlock()].setInvalid(a.getPage());
}

void Flash::program (Address& a, Event& e)
{
	// Make sure the page is clean
	assert (blocks[a.getBlock()].isClean(a.getPage()));

	// Set the page as valid and add in the data for it
	blocks[a.getBlock()].setValid (a.getPage(), e.time, e.getData(), sp.writestamp);
	blocks[a.getBlock()].setSector(a.getPage(), e.sector);

	blocks[a.getBlock()].getPage(a.getPage()).setETTI ( e.etti );
	blocks[a.getBlock()].getPage(a.getPage()).setEIT  ( e.eit );
}