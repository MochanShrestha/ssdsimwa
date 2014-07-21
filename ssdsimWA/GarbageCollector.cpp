#include "GarbageCollector.h"

#include <assert.h>

#ifdef LMTEST
GarbageCollector::GarbageCollector(SimParameters& s, StateManager& _sm, Flash& fm, MappingTable& mtable) : sp(s), f(fm), mt(mtable), sm(_sm)
#else
GarbageCollector::GarbageCollector(SimParameters& s, Stat& st, StateManager& _sm, Flash& fm, MappingTable& mtable) : sp(s), f(fm), stat(st), mt(mtable), sm(_sm)
#endif
{
}


GarbageCollector::~GarbageCollector(void)
{
}

int GarbageCollector::copyback (int b1, int b2)
{
	int pagei = 0;

	// Copyback the block
	for (int i=0; i<sp.c; i++)
	{
		// If the block is valid
		if (f.getBlock(b1).isValid(i))
		{
			f.getBlock(b2).getPage(pagei) = f.getBlock(b1).getPage(i);

			// Update the mapping tables
			Address ca (b2, pagei);
			mt.updateMapping( f.getBlock(b1).getSector(i), ca );

			pagei++;
		}
	}

	sm.setValid (b2, sm.getnValid(b1) );

	return pagei;
}

void GarbageCollector::copybacklm (LayoutManager& lm, int bi)
{
	// This is the block which we have to copyback
	Block& b = f.getBlock(bi);

	// Iterate through each page in the block to copyback the valid pages
	for (int i=0; i<sp.c; i++)
	{
		// Check if a page is valid
		if ( b.isValid(i) )
		{
			// Check if a clean block is needed or not to copyback this page
			if ( lm.needNewCBBlock() )
				lm.setNewCBBlock ( getCleanBlock(GARBCOL_COPYBACK) );

			// Get the address to copyback the page
			Address a = lm.getNextCopyback( b.getPage(i) );

			// Copyback the page
#ifndef LMTEST
			stat.pageCopyback ( b.getPage(i) );
#endif
			f.getBlock(a.getBlock()).copyback ( a.getPage(), b.getPage(i) );
			sm.incValid ( a.getBlock() );

			// Update the mapping tables
			mt.updateMapping ( b.getSector(i), a );
		}
	}
}