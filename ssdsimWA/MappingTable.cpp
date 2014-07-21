#include "MappingTable.h"

#include <iostream>
using namespace std;

#include <assert.h>

MappingTable::MappingTable(SimParameters& s) : sp(s)
{
	int m = sp.m;
	sectorToPage = new Address[m];
}


MappingTable::~MappingTable(void)
{
	delete[] sectorToPage;
}

Address MappingTable::getAddress (int sector)
{
	assert(sector < sp.m);
	return sectorToPage[sector];
}

void MappingTable::addMapping (int sector, Address& a)
{
	// The add mapping is for adding when the sector hasn't been assigned
	assert(sector < sp.m);
	sectorToPage[sector] = a;
}

void MappingTable::updateMapping (int sector, Address& a)
{
	// The update is for when the sector has been assigned
	assert(sector < sp.m);
	sectorToPage[sector] = a;
}

void MappingTable::removeMapping (int sector)
{
	assert(sector < sp.m);
	sectorToPage[sector].makeInvalid();
}