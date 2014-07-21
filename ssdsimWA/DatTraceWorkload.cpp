#include "DatTraceWorkload.h"
#include <assert.h>


DatTraceWorkload::DatTraceWorkload(int m, const char* filename, bool start) : TraceWorkload (m, filename, start)
{
	assert ( tracefile.is_open() );
}


DatTraceWorkload::~DatTraceWorkload(void)
{
}

Event DatTraceWorkload::getNextEvent()
{
	// Set the event
	Event e;
	e.time = (double)counter;
	
	do
	{
		tracefile >> e.sector;
	} while (e.sector >= m && !tracefile.eof());

	if (tracefile.fail())
	{
		e.time = -1;
	}

	assert (e.sector >= 0);
	e.op = WORKLOAD_OP_WRITE;

	return e;
}