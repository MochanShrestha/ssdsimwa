
#ifndef _DATTRACEWORKLOAD_H_
#define _DATTRACEWORKLOAD_H_

#include "traceworkload.h"
class DatTraceWorkload :
	public TraceWorkload
{
public:
	DatTraceWorkload(int m, const char* filename, bool start);
	~DatTraceWorkload(void);

	Event getNextEvent();
};

#endif