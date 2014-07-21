#include "SortedPoolGarbageCollector.h"

Flash* sgc_f;

SortedPoolGarbageCollector::SortedPoolGarbageCollector(int pool_size, SimParameters& sp, Stat& st, StateManager& _sm, Flash& fm, MappingTable& mtable)  : GarbageCollector (sp, st, _sm, fm, mtable)
{
	this->pool_size = pool_size;

	// Pointer to flash memory used as a comparator
	sgc_f = &fm;

	// Allocate the free block to the last blocks in the flash memory and set 
	// it's status
	for (int i=0; i<pool_size; i++)
	{
		int block = sp.n - 1 - i;		
		insert (block);
		sm.setState ( block, BLOCK_STATUS_CLEAN );
		//cout << block << '\t' << freeBlocks.size() << endl;

		//set <int, WearComp>::iterator it;
		//for( it = freeBlocks.begin(); it != freeBlocks.end(); it++ ) {
		//	cout << *it << endl;
		//}
	}
}


SortedPoolGarbageCollector::~SortedPoolGarbageCollector(void)
{
}

int SortedPoolGarbageCollector::findBlockToErase()
{
	int minValid = sp.c;
	int minValidCount = 0;
	int aMinBlock = -1;

	// Loop through all the blocks and find the block with the lowest amount of valid blocks to erase
	for (int i=0; i<sp.n; i++)
	{
		// Ignore the free/write blocks since it has no valid blocks
		if ( sm.getState (i) > 0)
			continue;

		if ( sm.getCookTime(i) > sp.current_timestamp )
			continue;

		// Find the minimum valid pages
		if ( minValid > sm.getnValid(i) )
		{
			minValid = sm.getnValid(i);
			minValidCount = 1;
			aMinBlock = i;
		}
		// count how many times the min occurs
		else if ( minValid == sm.getnValid(i) )
		{
			minValidCount++;
		}

		/* // If this has no valid pages, then we use this
		if ( minValid == 0 )
		{
			return minBlock;
		}*/
	}

	if ( minValid == sp.c )
	{
		cout << "Out of space. All blocks are full and have no pages to use." << endl;
		exit (0);
	}

	if ( minValidCount == 1 )
		return aMinBlock;

	if ( minValidCount == 0 )
	{
		cout << "Error! minValidCount is 0. Garbage Collector couldn't find a single block to reclaim!" << endl;
		exit(-1);
		/*for (int i=0; i<sp.n; i++)
		{
			cout << "Block " << i << ", Valid: " << sm.getnValid(i) << endl;
		}
		cout.flush();*/
	}

	int* minBlocks = new int[minValidCount];
	int minBlocksIndex = 0;

	// Make a list of block with the minValid number of blocks
	for (int i=0; i<sp.n; i++)
	{
		// Ignore the free/write blocks since it has no valid blocks
		if ( sm.getState (i) > 0)
			continue;

		if ( sm.getCookTime(i) > sp.current_timestamp )
			continue;

		if ( minValid == sm.getnValid(i) )
		{
			minBlocks[minBlocksIndex] = i;
			minBlocksIndex++;
		}
	}

	// randomly select one of the minblocks
	int minBlock = minBlocks[rand() % minValidCount];
	delete[] minBlocks;

	// This is the smallest we found. So, stop here.
	return minBlock;
}

Address SortedPoolGarbageCollector::getCleanBlock(int option)
{
	Address a;

	if ( isEmpty() )
	{
		cout << "The clean block queue is empty. Investigate" << endl;
	}

	// Get the begining of the queue or the 
	int fb;
	if ( option == GARBCOL_COPYBACK )
	{
		// write block. use block with the highest erase count
		set <int, WearComp>::iterator it;
		it = freeBlocks.end();
		--it;
		fb = *it;
		// Remove the item
		freeBlocks.erase(it);
	}
	else
	{
		// copyback block. Use block with lowest erase count
		fb = *(freeBlocks.begin());
		// Remove the item
		freeBlocks.erase(freeBlocks.begin());
	}
	
	a.setBlock ( fb );
	a.setPage ( 0 );

	if ( a.getBlock() == -1 )
	{
		cout << "Block is invalid. Investigate" << endl;
	}

	return a;
}

void SortedPoolGarbageCollector::copybackBlocks(LayoutManager& lm)
{
	//cout << freeBlocks.size() << endl;
		
	while ( !isFull() )
	{
		// First find a block to erase
		int eBlock = findBlockToErase();
		//cout << a.block << " : " << f.getBlock(a.block).getValid() << endl;

#ifndef LMTEST
		// Get the stats of the block we are about to erase
		stat.addBlockStat ( f, sm, eBlock );
#endif
	
		// Copyback the block
		copybacklm (lm, eBlock);

		// -- The data of the block is copied back. Mark the block as clean, erase the block, queue it to the clean block queue and 
		// -- update the state manager about the new status of the block
		// Set the value of the block as erased
		sm.clrValid(eBlock);
		// erase the block
		f.getBlock(eBlock).Erase(sp.current_timestamp);
		// queue in the block as an erased block
		insert (eBlock);
		// Set the status of the new clean block as clean.
		sm.setState ( eBlock, BLOCK_STATUS_CLEAN );
	}
}

void SortedPoolGarbageCollector::insert ( int b )
{
	std::pair<set<int,WearComp>::iterator,bool> result = freeBlocks.insert ( b );
	
	// Check if inserted. If not, then write out there it was not inserted
	if ( result.second == false )
	{
		cout << b << " Not inserted! Data value matched " << *(result.first) << "." << endl;
	}
}