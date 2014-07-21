
#ifndef _TRACE12_WORKLOAD_H_
#define _TRACE12_WORKLOAD_H_

#define FORMAT_TRACE1		1
#define FORMAT_TRACE1TI		2
#define FORMAT_TRACE2		3
#define FORMAT_TRACE2TI		4

#define OP_WRITE		1
#define OP_DEL			2

#include "traceworkload.h"

#include <stdlib.h>

class TRACE12Workload :
	public Workload
{
public:
	TRACE12Workload(const char* filename);
	~TRACE12Workload(void);

	Event getNextEvent();

	bool eow() { return ( feof(infile) != 0 ); };

	int getl() { return l; }

protected:
	FILE* infile;

	int l;
	int il;
	char header[256];

	int format;
};

#endif