#include "TRACE3Workload.h"

#include <assert.h>

#include <string>
using namespace std;


TRACE3Workload::TRACE3Workload(const char* filename) : Workload (1, false)
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
	if ( header_s == "TRACE3" )
	{
		fscanf_s (infile, "%d\n%d\n", &m, &l);
		cout << header << "\tm=" << m << "\tl=" << l << endl;
	}
	else
	{
		cout << "File format with header" << header_s << " is not known. Exiting ..." << endl;
		exit(-1);
	}
}


TRACE3Workload::~TRACE3Workload(void)
{
	if ( infile != NULL )
		fclose(infile);
}

Event TRACE3Workload::getNextEvent()
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
		int region;

		fscanf_s (infile, "%d\t%d\n", &sector, &region);
		op = OP_WRITE;

		e.sector = sector;
		e.region = region;

		e.op = op;

		assert (e.sector >= 0);
	}

	return e;
}

void TRACE3Workload::peekValues (const char* filename, int& m, int& l)
{
	FILE* infile;
	char header[256];

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
	if ( header_s == "TRACE3" )
	{
		fscanf_s (infile, "%d\n%d\n", &m, &l);
		cout << header << "\tm=" << m << "\tl=" << l << endl;
	}
	else
	{
		cout << "File format with header" << header_s << " is not known. Exiting ..." << endl;
		exit(-1);
	}

	fclose(infile);
}