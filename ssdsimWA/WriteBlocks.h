
#ifndef _WRITEBLOCKS_H_
#define _WRITEBLOCKS_H_

#include "StateManager.h"

class WriteBlocks
{
public:
	WriteBlocks(string rules_str, int start, SimParameters& sp, StateManager& sm);
	WriteBlocks(int n, int* rules, int start, SimParameters& sp, StateManager& sm);
	~WriteBlocks(void);

	// Check if a new block is needed or not
	bool needNewBlock() { return needBlock; }
	int  getNeedBlock() { return needBlockNo; }

	// Get the next write address for a given writeblock
	Address getNextWriteAddress (int b);

	// Sets the new block to the one given in the address
	void setNewBlock (Address& a);

	// Find the writeblock from the interval rules
	int findWriteBlock (int m);

	// get the rules from the string of the rules
	static void parseRules(string rules, int& nCBblocks, int *&cbrules);


	// Gets the number of writeblocks
	int getNWriteBlocks() { return nBlocks; }

	// Send the block number of the writeblock
	int getWriteBlock(int i) { if ( i < nBlocks) return writeBlocks[i]; else return -1; }

	// Get the block use count
	int getBlockUseCount(int i) { return blockUseCount[i]; }


	// Output the status to the console
	friend ostream &operator<<(ostream &stream, WriteBlocks& wb);

private:
	// The reference to the Sim parameters
	SimParameters& sp;

	// The reference to the StateManager object. Used to tell the state manager which blocks are being used as writeblocks
	StateManager& sm;

	// The number of writeblocks that we are using
	int nBlocks;

	// The arrays to hold the write blocks and the write positions within them
	int *writeBlocks;
	int *writePos;

	// Count of how many writeblocks have been used
	int *blockUseCount;

	// The rules for the intervals
	int *rules;

	// If we need clean blocks or not
	bool needBlock;
	int needBlockNo;

	// initalize the writeblocks array; this was created so that the rules can be initalized by the parseRules
	void createWriteBlocks(int start);

	// Holds how many time each copyback block was used
	//int *useCount;

	// The streaming stats for the block that we are going to maintain
	//StreamStat* copybackStats;
};

#endif