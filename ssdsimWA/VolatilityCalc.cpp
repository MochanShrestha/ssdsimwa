#include "VolatilityCalc.h"

// number of volatility bins
#define VOLUNITS	2500
// size of the volatility bin
#define VOLBINS		1

VolatilityCalc::VolatilityCalc(SimParameters& s, Stat& st) : sp(s), stat(st)
{
	// Allocate the sector map and set it to -1 indicating nothing stored for the sector
	sectorWritestamps = new int[sp.m];
	for (int i=0; i<sp.m; i++)
		sectorWritestamps[i] = -1;

	stampCounts = new int[VOLUNITS + 1];
	for (int i=0; i<=VOLUNITS; i++)
		stampCounts[i] = 0;

	// Create the workload
	if ( sp.wtype == "s")			// synthetic workload
	{
		int seed = (int)time(NULL);

		switch (sp.wdist)
		{
		case 1:
			w = new UnifRandWorkload (sp.m, sp.r, sp.static_sectors, sp.l, seed, sp.startfullsectors);
			break;
		case 2:
			w = new ZipfWorkload (sp.m, sp.r, sp.static_sectors, sp.l, seed, sp.startfullsectors);
			break;
		}
	}
	else if ( sp.wtype == "t")		// trace workload
	{
		if ( sp.tfiletype == "dat" )
		{
			w = new DatTraceWorkload ( sp.m, sp.tfilename.c_str(), sp.startfullsectors );
		}
	}
}


VolatilityCalc::~VolatilityCalc(void)
{
	delete[] sectorWritestamps;
	delete[] stampCounts;
}

void VolatilityCalc::start()
{
	Event e;

	int pcomplete = 0;

	clock_t start = clock();
	cout.setf(ios::fixed);

	// This is the number of writes that has happened
	int writestamp = 0;

	// Loop over the length of the workload
	do
	{
		e = w->getNext();
		sp.current_timestamp = e.time;

		// Either write or delete, we have to keep track of how many for the particular timestamp
		int sectorStamp = sectorWritestamps[e.sector];
		if ( sectorStamp >= 0 )
		{
			int stampdur = writestamp - sectorStamp;

			// Find the right bin and increment it
			int bindur = stampdur / VOLBINS;
			if ( bindur < VOLUNITS)
			{
				stampCounts[bindur]++;
			}
			else
			{
				stampCounts[VOLUNITS]++;
			}
			//cout << writestamp << '\t' << sectorStamp << endl;
		}

		// If the op is write, then we have to update the sector writestamp table
		if ( e.op == WORKLOAD_OP_WRITE )
		{
			sectorWritestamps[e.sector] = writestamp;
			writestamp++;
		}
		else
		{
			sectorWritestamps[e.sector] = -1;
		}

	} while ( !w->eow() );

	cout << "Counter : " << w->getCounter() << " (" << w->getCounter()/((double)(sp.n*sp.c)) << " ) " << endl;

	clock_t end = clock();
	double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
	cout << endl << "Took " << time_elapsed_in_seconds << "s" << endl;

	// -----------------------------------------------------------------------
	// ------------- Calculate the volatility --------------------------------
	// -----------------------------------------------------------------------
	long long int total = 0;
	for (int i=0; i<=VOLUNITS; i++)
		total += stampCounts[i];

	long long int totals[VOLUNITS + 1];
	totals[0] = total;
	for (int i=1; i<=VOLUNITS; i++)
		totals[i] = totals[i-1] - stampCounts[i-1];
	double vols[VOLUNITS + 1];
	for (int i=0; i<VOLUNITS; i++)
		vols[i] = (double)stampCounts[i] / (double)totals[i];

	// Output the data into the file
	ofstream volfile ("log/vol.log");
	for (int i=0; i<=VOLUNITS; i++)
	{
		volfile << i << '\t' << stampCounts[i] << '\t' << totals[i] << '\t' << vols[i] << endl;
	}
}