#include "LayoutManager.h"

#include "GarbageCollector.h"

ostream &operator<<(ostream &stream, LayoutManager& wb)
{
	stream << *(wb.w_wb);
	stream << *(wb.cb_wb);
	return stream;
}


LayoutManager::LayoutManager(SimParameters& _sp, StateManager& _sm, Flash& fm, GarbageCollector* _gc) : sp(_sp), f(fm), sm(_sm)
{
	// Set an empty flag
	flag = 0;

	// Copy over the garbage collector pointer
	gc = _gc;

	// Set the writeblock pointers to NULL
	w_wb = NULL;
	cb_wb = NULL;

	// Create the write LM object
	if ( sp.lm_walgorithm == "single" )
	{
		wlm = new SingleWriteLM (sp, sm, fm, w_wb, 0);
	}
	else if ( sp.lm_walgorithm == "etti" )
	{
		wlm = new ETTILayoutManager (sp, sm, fm, w_wb, 0);
		flag |= FLAG_LM_ETTI;
	}
	else if ( sp.lm_walgorithm == "2region" )
	{
		wlm = new Region2LayoutManager (sp, sm, fm, w_wb, 0);
	}

	int cbstart = w_wb->getNWriteBlocks();

	// Create the copyback LM object
	if ( sp.lm_cbalgorithm == "single" )
	{
		cblm = new SingleWriteBlockLayoutManager (sp, sm, fm, cb_wb, cbstart);
	}
	else if ( sp.lm_cbalgorithm == "wsingle" )
	{
		// The single block copyback algorithm but uses the block from the queue of the first 
		cblm = new WSingleWriteBlockLayoutManager (sp, sm, fm, w_wb, cbstart);
	}
	else if ( sp.lm_cbalgorithm == "copyback" )
	{
		cblm = new CopybackBlocksLayoutManager (sp, sm, fm, cb_wb, cbstart);
	}
	else if ( sp.lm_cbalgorithm == "writestamp" )
	{
		cblm = new WriteStampLayoutManager (sp, sm, fm, cb_wb, cbstart);
	}
}

LayoutManager::~LayoutManager(void)
{
	if ( wlm )
		delete wlm;
	if ( cblm )
		delete cblm;
}

int LayoutManager::getNWriteBlocks() 
{ 
	if ( cb_wb == NULL )
		return w_wb->getNWriteBlocks();
	else
		return (w_wb->getNWriteBlocks() + cb_wb->getNWriteBlocks()); 
}

Address LayoutManager::getNextWrite(const Event* e)
{
	// Check if garbage collection needs to happen
	while ( w_wb->needNewBlock() )
	{
		// Get a clean block and the old block address
		Address a = gc->getCleanBlock(GARBCOL_WRITE);
		int oldBlock = w_wb->getNeedBlock();

		// +++++++++++++++++++++++++++++++++++
		// Temporary hack. Set the region to 1 if write block is the first one and set the region to 1
		if ( oldBlock == 0 )
		{
			f.getBlock(a.getBlock()).setRegion(1);
		}

		// Set the new block
		w_wb->setNewBlock(a);

		// Calculate stats on the old writeblock that is being replaced
		//calcCookTime ( oldBlock, 0.85 );

		// Do garbage collection
		gc->copybackBlocks (*this);
	}

	return wlm->getNextWrite(e);
}

Address LayoutManager::getNextCopyback(const Page& p)
{
	return cblm->getNextCopyback(p);
}

bool LayoutManager::needNewCBBlock()
{
	// Check if any of the writeblocks need a new write block
	if ( cb_wb != NULL )
		return cb_wb->needNewBlock();
	else
		return false;
}

void LayoutManager::setNewCBBlock (Address& a)
{
	// Assign new block to whoever needs the block
	if ( cb_wb->needNewBlock() )
	{
		cb_wb->setNewBlock ( a );
	}
}

void LayoutManager::calcCookTime (int b, double p)
{
	int minPages = (int)ceil(p * sp.c);

	double *pEIT = new double[sp.c];
	for (int i=0; i<sp.c; i++)
	{
		pEIT[i] = f.getBlock(b).getPage(i).getEIT(); 
	}

	for (int i=40000; i<60000; i++)
	{
		// The cook time we are considering and the number of invalids at that time
		double cctime = sp.current_timestamp + i;
		int iCount = 0;

		// Go through each page in the block
		for (int j=0; j<sp.c; j++)
		{
			if ( pEIT[j] < cctime )
			{
				iCount++;
			}
		}

		// Check if this is the adequate cook time
		if ( iCount > minPages )
		{
			// Set the cook time for the block
			sm.setCookTime (b, cctime);
			return;
		}
	}

	delete[] pEIT;

	sm.setCookTime (b, -1.0);
}