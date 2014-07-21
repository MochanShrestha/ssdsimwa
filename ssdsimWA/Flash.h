
#ifndef _FLASH_H_
#define _FLASH_H_

#include "SimParameters.h"
#include "Block.h"
#include "Address.h"

class Flash
{
public:
	Flash(SimParameters& _sp);
	~Flash(void);

	void setValid (Address& a, int sector, double ts, const char* data, long long int ws);
	void setInvalid (Address& a);

	Block& getBlock(int n) const { return blocks[n];}
	Page& getPage(const Address& a) const { return blocks[a.getBlock()].getPage(a.getPage()); }

	// Programs a page with the current event
	void program (Address& a, Event& e);

protected:
	// Where the data for the flash memory is stored
	Block* blocks;

	// This is where the data for the flash memory is stored. Right now it stores the hash to check
	// if the algorithms worked correctly
	char* memory;

	// Simparameters object
	SimParameters& sp;
};

#endif