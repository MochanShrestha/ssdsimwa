
#include "FTL.h"

#ifdef LMTEST
FTL::FTL(Flash& fm, Cache& _c, SimParameters& _sp) : f(fm), c(_c), sp(_sp), mt(sp), sm(sp, fm)
#else
FTL::FTL(Flash& fm, Cache& _c, SimParameters& _sp, Stat& st) : f(fm), c(_c), sp(_sp), mt(sp), stat(st), sm(sp, fm)
#endif
{
	// Create the garbage collector object based on the configuration
	if ( sp.gc_algorithm == "freepool" )
	{
#ifdef LMTEST
		gc = new FreePoolGarbageCollector ( sp.pool_size, sp, sm, fm, mt );
#else
		gc = new FreePoolGarbageCollector ( sp.pool_size, sp, st, sm, fm, mt );
#endif
	} 
	else if ( sp.gc_algorithm == "sfreepool" )
	{
		gc = new SortedPoolGarbageCollector ( sp.pool_size, sp, st, sm, fm, mt );
	}
	else if ( sp.gc_algorithm == "gcscore" )
	{
		gc = new GCScoreGarbageCollector ( sp.pool_size, sp, st, sm, fm, mt );
	}
	else if ( sp.gc_algorithm == "inplace" )
	{
		//gc = new InPlaceGarbageCollector (sp, st, sm, fm, mt);
	}

	// Create the layout manager object
	lm = new LayoutManager (sp, sm, fm, gc);

	if ( sp.region1exclude && sp.gc_algorithm == "freepool" && sp.lm_walgorithm == "2region")
	{
		FreePoolGarbageCollector *fpgc = (FreePoolGarbageCollector*)gc;
		fpgc->setRegion1exclude(true);
	}

	assert ( lm->getNWriteBlocks() + sp.pool_size < sp.n );
}


FTL::~FTL(void)
{
	delete lm;
	delete gc;
}

void FTL::operate (Event& e)
{
	switch (e.op)
	{
	// write
	case WORKLOAD_OP_WRITE:
		writeOp (e);
		break;

	// delete
	case WORKLOAD_OP_DEL:
		deleteOp (e);
		break;
	}
}

void FTL::writeOp (Event& e)
{
	//cout << "Write: " << e.sector << endl;
	//cout << c;

	// Make the page invalid if it in the flash memory
	invalidSector ( e.sector );

	/*
	// selective caching - decide if we are going to cache this or not
	if ( e.etti > 0 && e.etti < sp.cachesize * 5 )
	{
		// Don't cache
		writeToDisk ( e );
		return;
	}
	*/

	// Check if we are using the cache or not
	if ( c.isUsed() )
	{
		// Check if we have space in the cache. If not free up some space
		if ( c.isFull() )
		{
			// Pop an item out of cache and write to disk
			Event ne = c.evict();

			// Write the new event to cache
			writeToDisk (ne);
		}

		// Update the mapping table to say this is on the cache
		Address a;
		a.setBlock ( ADDRESS_CACHED );
		mt.addMapping ( e.sector, a );

		// Put it in the cache
		c.add ( e );
	}
	else
	{
		// Since we are not using the cache, just write the event directly to disk
		writeToDisk ( e );
	}
	//cout << endl << endl << endl << endl;
}

void FTL::writeToDisk (Event& e)
{
	//cout << "Write to Disk: " << e.sector << endl;
	
	// Get the location for the next write
	Address a = lm->getNextWrite(&e);

	// Update the mapping tables and mark the page valid
	if (a.isOnDisk())
	{
		mt.addMapping (e.sector, a);
		f.program ( a, e );

		//cout << a << endl;

		/*if ( (e.getData())[0] != (int)((char)(e.sector)) )
		{
			cout << e << endl;
			exit(-1);
		}*/
		
		sm.incValid ( a.getBlock() );
	}

	// We processed a write even. Increase the write stamp
	sp.writestamp++;
}

void FTL::deleteOp (Event& e)
{
	invalidSector ( e.sector );
}

void FTL::invalidSector (int sector)
{
	// Make the page as invalid
	Address a = mt.getAddress(sector);

	// Check if the sector is cached and remove if so
	if ( a.isCached() )
	{
		// invalid the sector in the cache
		invalidSectorCache (sector, a);
	}

	// Check if this sector is on flash memory
	if ( a.isOnDisk() )
	{
		// invalid the sector in the disk
		invalidSectorDisk (sector, a);
	}
}

void FTL::invalidSectorCache (int sector, Address& a)
{
	// Remove the data from the cache so that the space can be used for something else
	c.remove ( sector );

	// Update the sector map
	mt.removeMapping (sector);
}

void FTL::invalidSectorDisk  (int sector, Address& a)
{
#ifndef LMTEST
	// Update the stats
	stat.pageInvalid (f, a);
#endif

	// Update the physical attributes
	f.setInvalid (a);

	// Update the state manager
	sm.decValid ( a.getBlock() );

	// Update the sector map
	mt.removeMapping(sector);
}

void FTL::close()
{
#ifndef LMTEST
	// Go through the valid pages and get their stats
	for (int i=0; i<mt.getLength(); i++)
	{
		Address a = mt.getAddress(i);
		if (a.isOnDisk())
			stat.pageInvalid (f, a);
	}
#endif
}