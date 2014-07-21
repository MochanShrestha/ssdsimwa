#ifndef _MAPPINGTABLE_H_
#define _MAPPINGTABLE_H_

#include "Address.h"
#include "SimParameters.h"

class MappingTable
{
public:
	MappingTable(SimParameters& s);
	~MappingTable(void);

	// Gets the address of the sector
	Address getAddress (int sector);

	void addMapping (int sector, Address& a);
	void updateMapping (int sector, Address& a);
	void removeMapping (int sector);

	int getLength() { return sp.m; }

protected:
	// This is the arrays that map sectors to pages
	Address *sectorToPage;
	// A refrence to the parameters object
	SimParameters& sp;
};

#endif