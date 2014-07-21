#include "Page.h"

// states. 0=invalid, 1=valid and -1=clean

// The shared memory size among the pages
int Page::mem_size;

Page::Page(void)
{
	state = -1;
	sector = -1;
	copyback_count = 0;
	timestamp = -1.0;
	writestamp = -1;
	etti = -1;
}


Page::~Page(void)
{
}

Page& Page::operator = (const Page& p) 
{
	// Sanity check
	assert ( p.isValid() );
	assert ( isClean() );

	// copy the data
	state = p.state;
	sector = p.sector;
	timestamp = p.timestamp;
	writestamp = p.writestamp;
	etti = p.etti;

	// copy the memory data
	memcpy ( memory, p.memory, mem_size );
	
	// increment the copyback counter
	copyback_count = p.copyback_count + 1;
	
	return *this;
}

ostream& operator<< (ostream &stream, Page& p)
{
	cout << "State:" << p.state << ", Sector:" << p.sector << ", ETTI:" << p.etti << ", TS:" << p.timestamp;
	cout << ", Data:";
	for (int i=0; i<Page::mem_size; i++)
		cout << (int)(p.memory[i]);

	return stream;
}