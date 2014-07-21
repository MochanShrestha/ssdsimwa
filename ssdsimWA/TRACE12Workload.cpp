#include "TRACE12Workload.h"

#include <assert.h>

#include <string>
using namespace std;


TRACE12Workload::TRACE12Workload(const char* filename) : Workload (1, false)
{
	// ------------------------------------------------------------------------------
	// Read the header of the file and figure out the format and read in the variables
	// ------------------------------------------------------------------------------
	// Open the file for reading
	if ( fopen_s (&infile, filename, "r") != 0 )
	{
		cout << "Error opening file. Quitting" << endl;
		exit(-1);
	}

	// Read the header and the variables
	fscanf_s (infile, "%s\n", header, 256);
	string header_s (header);

	// Determine the file format depending on the header
	if ( header_s == "TRACE1" )
	{
		format = FORMAT_TRACE1;
		fscanf_s (infile, "%d\n%d\n", &m, &l);
		cout << header << "\tm=" << m << "\tl=" << l << endl;
	}
	else if ( header_s == "TRACE1TI" )
	{
		format = FORMAT_TRACE1TI;
		fscanf_s (infile, "%d\n%d\n", &m, &l);
		cout << header << "\tm=" << m << "\tl=" << l << endl;
	}
	else if ( header_s == "TRACE2" )
	{
		format = FORMAT_TRACE2;
		fscanf_s (infile, "%d\n%d\n%d\n", &m, &l, &il);
		cout << header << "\tm=" << m << "\tl=" << l << "\til=" << il << endl;
	}
	else if ( header_s == "TRACE2TI" )
	{
		format = FORMAT_TRACE2TI;
		fscanf_s (infile, "%d\n%d\n%d\n", &m, &l, &il);
		cout << header << "\tm=" << m << "\tl=" << l << "\til=" << il << endl;
	}
	else
	{
		cout << "File format with header" << header_s << " is not known. Exiting ..." << endl;
		exit(-1);
	}
}


TRACE12Workload::~TRACE12Workload(void)
{
	if ( infile != NULL )
		fclose(infile);
}

Event TRACE12Workload::getNextEvent()
{
	// Set the event
	Event e;
	e.time = (double)counter;
	
	if ( eow() )
	{
		e.time = -1;
	}
	else
	{
		int op;
		int sector;
		int ti;

		switch (format)
		{
		case FORMAT_TRACE1:
			fscanf_s (infile, "%d\n", &sector);
			op = OP_WRITE;
			break;

		case FORMAT_TRACE1TI:
			fscanf_s (infile, "%d\t%d\n", &sector, &ti);
			op = OP_WRITE;
			break;

		case FORMAT_TRACE2:
			fscanf_s (infile, "%d\t%d\n", &op, & sector);
			break;

		case FORMAT_TRACE2TI:
			fscanf_s (infile, "%d\t%d", &op, & sector);
			if (op == OP_WRITE )
				fscanf_s (infile, "%d\n", &ti);
			else
				ti = -1;
			break;

		default:
			break;
		}

		e.sector = sector;
		e.op = op;

		assert (e.sector >= 0);
	}

	return e;
}