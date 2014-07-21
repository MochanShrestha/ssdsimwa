#include "SimParameters.h"
#include <time.h>

#define CFGPARSER_NOTFOUND	0


SimParameters::SimParameters(string simconfig)
{
	// Keep the name of the cfg file
	cfgfilename = simconfig;

	// Load the configuration parser
    ConfigParser_t cfg;
    if (cfg.readFile(simconfig))
    {
        cout << "Error: Cannot open config file 'example.cfg'\n" << endl;
		cout << "File given was: " << simconfig << endl;
		exit(-1);
    }

	// Read in the values of the simulator
    string section, option;

	// flash configurations
	section = "Flash";
    option = "n";
	// For trace workloads, n can be omitted
    /*if ( cfg.getValue(section, option, &n) == CFGPARSER_NOTFOUND )
	{
		cout << "The number of blocks 'n' was not given." << endl;
		cout << "Cannot continue ..." << endl;
		exit(-1);
	}*/
	cfg.getValue(section, option, &n);
    option = "c";
	if ( cfg.getValue(section, option, &c) == CFGPARSER_NOTFOUND )
	{
		cout << "The pages per block 'c' was not given." << endl;
		cout << "Cannot continue ..." << endl;
		exit(-1);
	}
	psize = 1;

	// FTL configuration
	section = "FTL";
	option = "overp";
	cfg.getValue(section, option, &overp);
	option = "cache";
	cfg.getValue(section, option, &cachesize);

	// workload configuration
	section = "Workload";
	option = "type";
	cfg.getValue(section, option, &wtype);
	option = "startfullsectors";
	string startfullsectors_str;
	cfg.getValue(section, option, &startfullsectors_str);
	if ( startfullsectors_str == "y" )
	{
		startfullsectors = true;
	}
	else
	{
		startfullsectors = false;
	}

	if ( wtype == "s")			// synthetic workload
	{
		option = "dist";
		cfg.getValue(section, option, &wdist);	
		option = "r";
		cfg.getValue(section, option, &r);
		option = "static";
		cfg.getValue(section, option, &static_sectors);
		option = "lfactor";
		cfg.getValue(section, option, &lfactor);

		int seed = (int)time(NULL);

		// set up the derived values from the parameters
		l = n * c * lfactor;
		m = (int)floor(n * c * overp);
	}
	else if ( wtype == "t")		// trace workload
	{
		option = "tfiletype";
		cfg.getValue(section, option, &tfiletype);
		if ( tfiletype == "dat" )
		{
			option = "tfile";
			cfg.getValue(section, option, &tfilename);

			option = "m";
			cfg.getValue(section, option, &m);
			n = (int) ceil ( m / ( c * overp ) );
		}
		else if ( tfiletype == "trace" )
		{
			option = "tfile";
			cfg.getValue(section, option, &tfilename);

			option = "m";
			cfg.getValue(section, option, &m);

			option = "n";
			cfg.getValue(section, option, &n);
		}
		else if ( tfiletype == "TRACE3" )
		{
			option = "tfile";
			cfg.getValue(section, option, &tfilename);
			int tl;
			TRACE3Workload::peekValues (tfilename.c_str(), m, tl);
			l = tl;
			calcN();
			cout << "Peeking: m=" << m << ", l=" << l << ", n=" << n << endl;
		}
		else
		{
			cout << "Unknown trace file type" << endl;
			exit(-1);
		}
	}

	// Sim parameters
	section = "Sim";
	option = "hash";
	cfg.getValue(section, option, &hashalgorithm);
	hash_algo = HashData::getHashAlgo ( hashalgorithm );
	hash_size = HashData::getHashDataSize ( hash_algo );
	option = "viz_steps";
	cfg.getValue(section, option, &viz_steps);
	viz = false;
	option = "viz_writeblocks";
	cfg.getValue(section, option, &viz_writeblocks_s);
	if ( viz_writeblocks_s == "y" )
	{
		// Set the flag that we will be showing the heatmap visualization
		viz = true;
		viz_writeblocks = true;

		// Set the default parameters for the heatmap parameters
		hm_blocksperrow = 1;
		hm_blocksample = 1;

		// Get the parameters from the config file
		option = "hm_blocksperrow";
		cfg.getValue(section, option, &hm_blocksperrow);
		option = "hm_blocksample";
		cfg.getValue(section, option, &hm_blocksample);
		option = "hm_sort";
		string hm_sort_s;
		cfg.getValue(section, option, &hm_sort_s);
		if ( hm_sort_s == "y" )
		{
			hm_sort = true;
		}
		else
		{
			hm_sort = false;
		}
	}
	else
	{
		viz_writeblocks = false;
	}
	option = "viz_heatmap";
	cfg.getValue(section, option, &viz_heatmap_s);
	if ( viz_heatmap_s == "y" )
	{
		viz = true;	
		viz_heatmap = true;
	}
	else
	{
		viz_heatmap = false;
	}

	// stats configuration
	// default values
	snap_int = 0;
	
	section = "Stats";
	option = "wadist";
	cfg.getValue(section, option, &wadistlog);
	option = "wwadist";
	cfg.getValue(section, option, &wwadistlog);
	option = "wat";
	cfg.getValue(section, option, &watlog);
	option = "wwat";
	cfg.getValue(section, option, &wwatlog);
	option = "cb";
	cfg.getValue(section, option, &cblog);
	option = "debug";
	cfg.getValue(section, option, &debuglog);
	option = "wear";
	cfg.getValue(section, option, &wearlog);
	option = "erasecounts";
	cfg.getValue(section, option, &erasecountslog);
	option = "map";
	cfg.getValue(section, option, &mapeps);
	option = "blocksperrow";
	if ( !cfg.getValue(section, option, &blocks_per_row) )
		blocks_per_row = 4;
	option = "blocksample";
	if ( !cfg.getValue(section, option, &block_sample) )
		block_sample = 1;
	option = "vol";
	cfg.getValue(section, option, &volfilename);
	option = "volunits";
	volUnits = -1;
	cfg.getValue(section, option, &volUnits);
	option = "volbinsize";
	volBinSize = -1;
	cfg.getValue(section, option, &volBinSize);
	option = "cbws";
	cfg.getValue(section, option, &cbwsfilename);
	option = "tstotal";
	cfg.getValue(section, option, &tstotalfilename);
	option = "snapint";
	cfg.getValue(section, option, &snap_int);
	option = "snapprefix";
	cfg.getValue(section, option, &snap_prefix);
	option = "wsnapprefix";
	cfg.getValue(section, option, &wsnap_prefix);
	option = "finalCBcounts";
	cfg.getValue(section, option, &finalCBcounts);
	option = "lmstats";
	cfg.getValue(section, option, &lmstats);

	// Garbage Collector configuration
	section = "GarbageCollector";
	option = "algorithm";
	cfg.getValue(section, option, &gc_algorithm);
	option = "poolsize";
	cfg.getValue(section, option, &pool_size);
	option = "lambda";
	cfg.getValue(section, option, &lambda);
	option = "region1exclude";
	string region1exclude_str;
	cfg.getValue(section, option, &region1exclude_str);
	if ( region1exclude_str == "y" )
	{
		region1exclude = true;
	}
	else
	{
		region1exclude = false;
	}

	// Layout Manager configuration
	section = "LayoutManager";
	option = "walgorithm";
	if ( cfg.getValue(section, option, &lm_walgorithm) == CFGPARSER_NOTFOUND )
	{
		cout << "The write layout manager algorithm was not given." << endl;
		cout << "Cannot continue ..." << endl;
		exit(-1);
	}
	option = "wrules";
	cfg.getValue(section, option, &lm_wrules);
	
	option = "cbalgorithm";
	if ( cfg.getValue(section, option, &lm_cbalgorithm) == CFGPARSER_NOTFOUND )
	{
		cout << "The copyback layout manager algorithm was not given." << endl;
		cout << "Cannot continue ..." << endl;
		exit(-1);
	}
	// Get the copyback rules string
	option = "cbrules";
	cfg.getValue(section, option, &lm_cbrules);

	// Initalize the dynamic content to default values
	current_timestamp = 0;
	writestamp = 0;
}


SimParameters::~SimParameters(void)
{
}

ostream &operator<<(ostream &stream, SimParameters& sp)
{
	cout << "cfg file: " << sp.cfgfilename << endl << endl;

	cout << "[Flash]" << endl;
	cout << "n:" << sp.n << endl;
	cout << "c:" << sp.c << endl << endl;

	cout << "[FTL]" << endl;
	cout << "overp : " << sp.overp;
	cout << " (Worst Case WA: " << 1/(1-sp.overp) << ") ";
	cout << endl << endl;
	cout << "Cache Size : " << sp.cachesize << endl << endl;

	cout << "[Workload]" << endl;
	cout << "m: " << sp.m << endl;
	cout << "w: " << sp.wtype << endl;
	cout << "startfullsectors: " << sp.startfullsectors << endl;
	if ( sp.wtype == "s" )
	{
		cout << "dist: " << sp.wdist << endl;
		cout << "r: " << sp.r << endl;
		cout << "static: " << sp.static_sectors << endl;
		cout << "lfactor: " << sp.lfactor << endl;
		cout << "l: " << sp.l << endl;
		cout << endl;
	}
	else if (sp.wtype == "t" )
	{
		cout << "Trace Type: " << sp.tfiletype << endl;
		cout << "Tracefile " << sp.tfilename << endl;
		cout << endl;
	}

	cout << "[Sim]" << endl;
	cout << "hash: " << sp.hashalgorithm << endl;
	cout << "Viz: " << (sp.viz?'Y':'N');
	if ( sp.viz ) 
	{
		cout << "(Steps: " << sp.viz_steps << ") " << endl;
		cout << " [ ";
		if ( sp.viz_writeblocks == true )
		{
			cout << "writeblocks ";
		}
		if ( sp.viz_heatmap == true )
		{
			cout << "heatmap (" << sp.hm_blocksperrow << "," << sp.hm_blocksample << ",";
			cout << "sort:" << (sp.hm_sort?"y":"n");
			cout << ")" << " ";
		}
		cout << "]"; 
	}
	cout << endl;
	cout << endl;

	cout << "[Stats]" << endl;
	cout << "wadist: " << sp.wadistlog << endl;
	cout << "window wadist: " << sp.wwadistlog << endl;
	cout << "wat: " << sp.watlog << endl;
	if ( sp.mapeps != "")
	{
		cout << "map eps: " << sp.mapeps << " (" << sp.blocks_per_row << "," << sp.block_sample << ")" << endl;
	}
	cout << "vol: " << sp.volfilename << " (" << sp.volUnits << "," << sp.volBinSize << ")" << endl;
	if ( sp.tstotalfilename != "")
	{
		cout << "Ts-total Copybacks: " << sp.tstotalfilename << endl;
	}
	if ( sp.snap_int > 0 )
	{
		cout << "Snapshot Interval: " << sp.snap_int << endl;
		cout << "Snapshot prefix: " << sp.snap_prefix << endl;
		cout << "W. Snaps prefix: " << sp.wsnap_prefix << endl;
	}
	cout << endl;

	cout << "[GarbageCollector]" << endl;
	cout << sp.gc_algorithm;
	if ( sp.gc_algorithm == "freepool" || sp.gc_algorithm == "sfreepool")
	{
		cout << " ( pool_size=" << sp.pool_size << " )" << endl;
	}
	else if ( sp.gc_algorithm == "gcscore")
	{
		cout << " ( pool_size=" << sp.pool_size << " ) ";
		cout << "lambda: " << sp.lambda << endl;
	}
	else
	{
		cout << endl;
	}
	cout << endl;

	cout << "[LayoutManager]" << endl;
	cout << sp.lm_walgorithm << "\t(" << sp.lm_wrules << ")" << endl;
	cout << sp.lm_cbalgorithm << "\t(" << sp.lm_cbrules << ")" << endl;

	return stream;
}
