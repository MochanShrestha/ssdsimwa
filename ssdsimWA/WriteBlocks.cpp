#include "WriteBlocks.h"

#include <iostream>
#include <sstream>
#include <string>
using namespace std;

WriteBlocks::WriteBlocks(string rules_str, int start, SimParameters& _sp, StateManager& _sm) : sp(_sp), sm(_sm)
{
	// parse rules fills in the number of blocks and the rules
	parseRules(rules_str, nBlocks, rules);

	// Initalize the other arrays
	createWriteBlocks(start);
}

WriteBlocks::WriteBlocks(int n, int* r, int start, SimParameters& _sp, StateManager& _sm) : sp(_sp), sm(_sm)
{
	nBlocks = n;

	// hold a copy of the rules
	rules = new int[nBlocks+1];

	// Copy the rules
	// There are one more rule than the number of blocks
	for (int i=0; i<nBlocks+1; i++)
	{
		rules[i] = r[i];
	}

	// Allocate the write block arrays
	createWriteBlocks(start);
}

void WriteBlocks::createWriteBlocks(int start)
{
	// Allocate the write blocks and write positions
	writeBlocks = new int[nBlocks];
	writePos = new int[nBlocks];

	// Allocate the counts
	blockUseCount = new int[nBlocks];

	// Use the first nWriteBlocks as the first write blocks to use
	for (int i=0; i<nBlocks; i++)
	{
		writeBlocks[i] = i+start;
		sm.setState (writeBlocks[i], BLOCK_STATUS_WRITE);
		writePos[i] = 0;

		// None of the blocks have been used and set them to zero
		blockUseCount[i] = 0;
	}

	// Flag for if new clean blocks are needed
	needBlock = false;
	needBlockNo = -1;
}

WriteBlocks::~WriteBlocks(void)
{
	delete[] writeBlocks;
	delete[] writePos;
}

Address WriteBlocks::getNextWriteAddress (int b)
{
	Address a;

	// Find the new write address
	assert ( !needNewBlock() );
	if ( !needNewBlock() )
	{
		a.setBlock ( writeBlocks[b] );
		a.setPage ( writePos[b] );

		writePos[b]++;

		if ( writePos[b] == sp.c )
		{
			needBlock = true;
			needBlockNo = b;
		}
	}
	else
	{
		cout << "Requested new address when a new block was required!" << endl;
	}

	return a;
}

void WriteBlocks::setNewBlock (Address& a)
{
	assert ( needNewBlock() );
	if ( needNewBlock() )
	{
		int oldBlock = writeBlocks[ needBlockNo ];
				
		writeBlocks[ needBlockNo ] = a.getBlock();
		writePos   [ needBlockNo ] = a.getPage();

		sm.setState ( oldBlock, BLOCK_STATUS_NONE );
		sm.setState ( a.getBlock(), BLOCK_STATUS_WRITE );

		// We don't need a block anymore
		needBlock = false;

		// Update the block usage count
		blockUseCount[needBlockNo]++;
	}
}

int WriteBlocks::findWriteBlock (int m)
{
	// This catches the negative values and returns an error
	if ( m < rules[0] ) 
		return -1;

	// check the array and find out where it should go
	for (int i=0; i<nBlocks; i++)
	{
		if ( m >= rules[i] && m < rules[i+1] )
		{
			return i;
		}
	}

	// Should never get here but to shut up the warnings
	return nBlocks-1;
}

void WriteBlocks::parseRules(string rules, int& nCBblocks, int *&cbrules)
{
	int nCommas = 0;

	// Find the size of the array we have to create by counting commas
	for (unsigned int i=0; i<rules.length(); i++)
	{
		if ( rules[i] == ',' )
		{
			nCommas++;
		}
	}

	// Now allocate the array of rules
	int nRules = nCommas + 3;
	nCBblocks = nRules - 1;
	cbrules = new int[nRules];
	
	// Fill out the array
	cbrules[0] = 0;
	stringstream rulestream (rules);
	string num;
	for (int i=0; i<=nCommas; i++)
	{
		getline (rulestream, num, ',');
		stringstream nums (num);
		nums >> cbrules[i+1];
	}
	cbrules[nRules-1] = INT_MAX;
}

ostream& operator<<(ostream &stream, WriteBlocks& wb)
{
	stream << "nBlocks: " << wb.nBlocks << " [needBlock:" << (wb.needBlock?'Y':'N') << "]" << endl;
	for (int i=0; i<wb.nBlocks; i++)
	{
		if ( wb.needBlock && i == wb.needBlockNo )
			stream << " *";
		else
			stream << "  ";
		stream << i << ". " << " {" << wb.writeBlocks[i] << ", " << wb.writePos[i] << "} ";
		stream << "\t [" << wb.rules[i] << ", " << wb.rules[i+1] << ")";
		stream << endl;
	}

	return stream;
}