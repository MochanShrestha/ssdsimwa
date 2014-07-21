#include "FreePoolGarbageCollector.h"

#ifdef LMTEST
FreePoolGarbageCollector::FreePoolGarbageCollector(int pool_size, SimParameters& sp, StateManager& _sm, Flash& fm, MappingTable& mtable) : GarbageCollector (sp, _sm, fm, mtable), freeBlocks(pool_size)
#else
FreePoolGarbageCollector::FreePoolGarbageCollector(int pool_size, SimParameters& sp, Stat& st, StateManager& _sm, Flash& fm, MappingTable& mtable) : GarbageCollector (sp, st, _sm, fm, mtable), freeBlocks(pool_size)
#endif
{
	// Allocate the free block to the last blocks in the flash memory and set 
	// it's status
	for (int i=0; i<pool_size; i++)
	{
		int block = sp.n - 1 - i;		
		freeBlocks.queue (block);
		sm.setState ( block, BLOCK_STATUS_CLEAN );
	}

	region1exclude = false;
}


FreePoolGarbageCollector::~FreePoolGarbageCollector(void)
{
}

int FreePoolGarbageCollector::findBlockToErase()
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

		//if ( sm.getCookTime(i) > sp.current_timestamp )
		//	continue;
		if ( region1exclude )
		{
			if ( f.getBlock(i).getRegion() == 1)
				if ( sm.getnValid(i) > 0 )
					continue;
		}

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

		if ( region1exclude )
		{
			if ( f.getBlock(i).getRegion() == 1)
				if ( sm.getnValid(i) > 0 )
					continue;
		}

		//if ( sm.getCookTime(i) > sp.current_timestamp )
		//	continue;

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

Address FreePoolGarbageCollector::getCleanBlock(int option)
{
	if ( freeBlocks.isEmpty() )
	{
		cout << "The clean block queue is empty. Investigate" << endl;
	}

	Address a;

	a.setBlock ( freeBlocks.dequeue() );
	a.setPage ( 0 );

	if ( a.getBlock() == -1 )
	{
		cout << "Block is invalid. Investigate" << endl;
	}

	return a;
}

void FreePoolGarbageCollector::copybackBlocks(LayoutManager& lm)
{
	while ( !freeBlocks.isFull() )
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
		freeBlocks.queue (eBlock);
		// Set the status of the new clean block as clean.
		sm.setState ( eBlock, BLOCK_STATUS_CLEAN );
	}
}