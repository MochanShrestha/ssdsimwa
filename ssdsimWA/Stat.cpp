#include "Stat.h"

#include <iostream>
using namespace std;
#include "FlashMap.h"


// number of volatility bins
#define VOLUNITS	250000
// size of the volatility bin
#define VOLBINS		1


#define WA_WINDOW_SIZE 100000

Stat::Stat(SimParameters& s) : statfile ( s.wadistlog ), sp(s), stattfile (s.watlog), cbfile (s.cblog), debugfile (s.debuglog), wwatfile(s.wwatlog), wa_queue (WA_WINDOW_SIZE), tstotalfile (s.tstotalfilename), wwafile(s.wwadistlog)
{
	CEcounts = new int[sp.c];
	memset (CEcounts, 0, sp.c*sizeof(int) );
	memset (copybackCounts, 0, MAX_COPYBACKS*sizeof(int));

	total = 0;
	totalCopybacks = 0;
	workload_pos = 0;

	if ( sp.volUnits == -1 )
		volUnits = VOLUNITS;
	else
		volUnits = sp.volUnits;
	if ( sp.volBinSize == -1 )
		volBinSize = VOLBINS;
	else
		volBinSize = sp.volBinSize;
	stampCounts = new long long int[volUnits + 1];
	for (int i=0; i<=volUnits; i++)
		stampCounts[i] = 0;

	/*copybackWritestampMean = new double[VOLUNITS + 1];
	copybackWritestampN = new int [VOLUNITS + 1];
	for (int i=0; i<=VOLUNITS; i++)
	{
		copybackWritestampMean[i] = 0.0;
		copybackWritestampN[i] = 0;
	}*/

	if ( sp.cbwsfilename != "" )
	{
		wsccfile.open (sp.cbwsfilename);
		logcbws = true;
	}
	else
		logcbws = false;
}

Stat::~Stat(void)
{
	// Delete the arrays, we are done with them
	delete[] CEcounts;
	delete[] stampCounts;
	//delete[] copybackWritestampMean;
	//delete[] copybackWritestampN;
}

void Stat::addBlockStat(const Flash& f, const StateManager& sm, const int b)
{
	//cout << "Valid: " << sm.getnValid(b) << ", Last Erase: " << sp.current_timestamp - f.getBlock(b).getLastErased() << endl;

	if ( sm.getState (b) == BLOCK_STATUS_NONE )
	{
		// Add the total number of pages that are copied back
		totalCopybacks += sm.getnValid(b);

		// Update the stats regarding the time between garbage collections
		if ( f.getBlock(b).getLastErased() > 0 )
		{
			gcAge.add ( sp.current_timestamp - f.getBlock(b).getLastErased() );
		}

		// Update the write amplification stats
		CEcounts[sm.getnValid(b)]++; 
		total++;

		// Update the write amplification window
		if ( wa_queue.isFull() )
			wa_queue.dequeue();
		wa_queue.queue( sm.getnValid(b) );
	}

	// Output incremental updates about the write amplificiation to file
	//if ( total > 0 && total % 250 == 0 )
	if ( total > 0 && (int)(sp.current_timestamp / 25000) > workload_pos )
	{
		workload_pos = (int)(sp.current_timestamp / 25000);
		double wa = calcWA();
		stattfile << sp.current_timestamp << '\t' << wa << '\t' << wa - old_wa << endl;
		old_wa = wa;

		double wwa = calcWWA();
		wwatfile << sp.current_timestamp << '\t' << wwa << endl;

	//	// Calculate the number of none, clean and write blocks in the system
	//	/*int states[3];
	//	for (int i=0; i<3; i++)
	//		states[i] = 0;
	//	for (int i=0; i<sp.n; i++)
	//	{
	//		states[f.getBlock(i).getStatus()]++;
	//	}

	//	debugfile << total << '\t' << states[0] << '\t' << states[1] << '\t' << states[2] << endl;*/
	}
	//cout << (numValidPages(f)/((double)sp.n * sp.c)) << endl;

	// Write out the write amplification curves snapshots
	if ( sp.snap_int > 0 && total > 0 && total % sp.snap_int == 0 )
	{
		// What to put after the prefix
		int count = total / sp.snap_int;

		// Filename that we will create from the prefix
		char filename[256];

		if ( sp.snap_prefix != "")
		{
			// Write out the write amplification 
			sprintf (filename, "%s%d.log", sp.snap_prefix.c_str(), count);
			ofstream snapfile(filename);
			for (int i=0; i<sp.c; i++)
			{
				snapfile << i << '\t' << CEcounts[i] << '\t' << CEcounts[i]/(double)total << endl;
			}
		}

		if ( sp.wsnap_prefix != "")
		{
			// Write out the windowed write amplification
			sprintf (filename, "%s%d.log", sp.wsnap_prefix.c_str(), count);
			int *countbin = new int[sp.c];
			memset (countbin, 0, sizeof(int)*sp.c);
			for (int i=0; i<wa_queue.getLength(); i++)
			{
				countbin[wa_queue.getValue(i)]++;
			}
			ofstream snapfilew(filename);
			for (int i=0; i<sp.c; i++)
			{
				snapfilew << i << '\t' << countbin[i] << '\t' << ((double)countbin[i])/wa_queue.getLength() << endl;
			}

			delete[] countbin;
		}

		// Print out the relationship between the copyback totals and the time
		if ( !tstotalfile.fail() )
		{
			tstotalfile << sp.current_timestamp << '\t' << total << endl;
		}
	}
}

void Stat::pageInvalid(const Flash& f, const Address& a)
{
	// The page at the address a is being invalidated and thus, we record
	// it's copyback count
	int pCopybackCount = f.getPage(a).getCopybackCount();
	if ( pCopybackCount < MAX_COPYBACKS - 1 )
	{
		copybackCounts[pCopybackCount]++;
	}
	else
	{
		copybackCounts[MAX_COPYBACKS-1]++;
	}

	// Update the arrays of the writestamps
	long long int sectorStamp = f.getPage(a).getWriteStamp();
	if ( sectorStamp >= 0 )
	{
		int stampdur = (int)(sp.writestamp - sectorStamp);

		// Find the right bin and increment it
		int bindur = stampdur / volBinSize;
		if ( bindur < volUnits)
		{
			stampCounts[bindur]++;
		}
		else
		{
			stampCounts[volUnits]++;
		}
		//cout << writestamp << '\t' << sectorStamp << endl;
	}
}

double Stat::calcMean()
{
	double mean = 0;
	for (int i=0; i<sp.c; i++)
		mean += i*CEcounts[i];
	return (mean / total );
}

double Stat::calcWA()
{
	if ( total == 0 )
	{
		return -1;
	}

	double wa = 0;
	for (int i=0; i<sp.c; i++)
		wa += i * ((double)CEcounts[i])/total;
	return  ( sp.c / ( sp.c - wa ) );
	//return wa;
}

double Stat::calcWWA()
{
	double wwa = 0;

	int *countbin = new int[sp.c+1];
	memset (countbin, 0, sizeof(int)*(sp.c+1));

	for (int i=0; i<wa_queue.getLength(); i++)
	{
		countbin[wa_queue.getValue(i)]++;
	}
	for (int i=0; i<sp.c+1; i++)
	{
		wwa += i * ((double)countbin[i])/wa_queue.getLength();
	}

	delete[] countbin;
	return ( sp.c / ( sp.c - wwa) );
}

double Stat::calcsWA()
{
	return ( (sp.writestamp + totalCopybacks) / sp.current_timestamp );
}

double Stat::calcVPE()
{
	if ( total == 0 )
	{
		return -1;
	}
	double vpe = 0;
	for (int i=0; i<sp.c; i++)
		vpe += i * ((double)CEcounts[i])/total;

	return (vpe/sp.c*100.0);
}

int Stat::numValidPages(const Flash& f)
{
	int vPages = 0;
	for (int i=0; i<sp.n; i++)
	{
		vPages += f.getBlock(i).getValidF();
	}
	return vPages;
}

void Stat::pageCopyback(const Page& p)
{
	// Get how long the page has been valid for
	long long int stampdur = sp.writestamp - p.getWriteStamp();
	// Get the copyback count of the page
	int copybackcount = p.getCopybackCount();

	// Now that we have the writestamp and copyback count, write it to file
	if ( logcbws )
	{
		wsccfile << copybackcount << '\t' << stampdur << endl;
	}

	/* // Update the mean and the counts
	double n = (double)(copybackWritestampN[copybackcount]);
	double m = copybackWritestampMean[copybackcount];

	copybackWritestampMean[copybackcount] = m * (n)/(n+1) + (double)stampdur/(n+1);
	copybackWritestampN[copybackcount]++; */
}

void Stat::simEnd (Flash& f, StateManager& sm)
{
	// =============================================================================
	// Write the wear distribution to file
	// =============================================================================
	if ( sp.wearlog != "")
	{
		// First go through the blocks and find the highest erase count
		int nCounts = 0;
		for (int i=0; i<sp.n; i++)
		{
			int eCount = f.getBlock(i).getEraseCount();
			if ( eCount > nCounts )
			{
				nCounts = eCount;
			}
		}
		// Make space for the zero
		nCounts++;

		// First calculate the wear distribution
		int *eraseCounts = new int [ nCounts ];
		for (int i=0; i<nCounts; i++)
		{
			eraseCounts[i] = 0;
		}
		for (int i=0; i<sp.n; i++)
		{
			int eCount = f.getBlock(i).getEraseCount();
			eraseCounts [eCount]++;
		}

		ofstream wear_dist (sp.wearlog);
		for (int i=0; i<nCounts; i++)
			wear_dist << i << "\t" << eraseCounts[i] << endl;
	}

	// =============================================================================
	// Write the erase counts of each of the blocks in the flash memory.
	// =============================================================================
	if ( sp.erasecountslog != "")
	{
		ofstream ecs( sp.erasecountslog );
		for (int i=0; i<sp.n; i++)
		{
			ecs << i << '\t' << f.getBlock(i).getEraseCount() << endl;
		}
		ecs.close();
	}

	// ==============================================================
	// Write the final state of the flash memory as a temperature map
	// ==============================================================
	if ( sp.mapeps != "")
		FlashMap::outputTempMapSorted ( sp, f, sm, sp.mapeps );

	// =============================================================================
	// Write out the final stage of the copyback count distributions of the pages
	// =============================================================================
	if ( sp.finalCBcounts != "")
	{
		// First get the maximum number of copybacks that is in the pages of the flash memory
		int maxCopybacks = 0;
		for (int i=0; i<sp.n; i++)
		{
			for (int j=0; j<sp.c; j++)
			{
				int page_copybackcount = f.getBlock(i).getPage(j).getCopybackCount();
				if ( maxCopybacks < page_copybackcount )
				{
					maxCopybacks = page_copybackcount;
				}
			}
		}
		// Increment maxCopybacks since 0 is not included in our counts
		maxCopybacks++;

		// Now create the distribution based on this.
		int* finalCopybackDist = new int[maxCopybacks];
		memset (finalCopybackDist, 0, sizeof(int)*maxCopybacks);
		for (int i=0; i<sp.n; i++)
		{
			for (int j=0; j<sp.c; j++)
			{
				int page_copybackcount = f.getBlock(i).getPage(j).getCopybackCount();
				finalCopybackDist[page_copybackcount]++;
			}
		}

		// Write the contents to the log file
		ofstream finalcbcountsfile (sp.finalCBcounts );
		for (int i=0; i<maxCopybacks; i++)
		{
			finalcbcountsfile << i << '\t' << finalCopybackDist[i] << endl;
		}
		finalcbcountsfile.close();

		// Done. Clean up after yourself.
		delete[] finalCopybackDist;
	}

	for (int i=0; i<sp.c; i++)
	{
		//statfile << i << '\t' << ((double)CEcounts[i])/total << endl;
		statfile << i << '\t' << CEcounts[i] << '\t' << CEcounts[i]/(double)total << endl;
	}

	// Write out the window write amplification
	int *countbin = new int[sp.c];
	memset (countbin, 0, sizeof(int)*sp.c);
	for (int i=0; i<wa_queue.getLength(); i++)
	{
		countbin[wa_queue.getValue(i)]++;
	}
	for (int i=0; i<sp.c; i++)
	{
		wwafile << i << '\t' << countbin[i] << '\t' << ((double)countbin[i])/wa_queue.getLength() << endl;
	}
	delete[] countbin;

	for (int i=0; i<MAX_COPYBACKS; i++)
		cbfile << i << " " << copybackCounts[i] << " " << copybackCounts[i]*i << endl;

	// -----------------------------------------------------------------------
	// ------------- Calculate the volatility --------------------------------
	// -----------------------------------------------------------------------
	long long int total = 0;
	for (int i=0; i<=volUnits; i++)
		total += stampCounts[i];

	long long int *totals = new long long int[volUnits + 1];
	totals[0] = total;
	for (int i=1; i<=volUnits; i++)
		totals[i] = totals[i-1] - stampCounts[i-1];
	double *vols = new double[volUnits + 1];
	for (int i=0; i<volUnits; i++)
		vols[i] = (double)stampCounts[i] / (double)totals[i];

	// Output the data into the file
	ofstream volfile (sp.volfilename);
	for (int i=0; i<=volUnits; i++)
	{
		volfile << i*volBinSize << '\t' << stampCounts[i] << '\t' << totals[i] << '\t' << vols[i] << endl;
	}

	// Output the data between the writestamp and copybacks
	/*ofstream wsccfile ("log/ws_cc.log");
	for (int i=0; i<=VOLUNITS; i++)
	{
		wsccfile << i << '\t' << copybackWritestampMean[i] << '\t' << copybackWritestampN[i] << endl;
	}*/

	delete[] totals;
	delete[] vols;
}