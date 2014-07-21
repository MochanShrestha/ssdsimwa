
#ifndef _TRACE3_WORKLOAD_H_
#define _TRACE3_WORKLOAD_H_

#include "workload.h"

#include <stdlib.h>

#define OP_WRITE		1
#define OP_DEL			2

class TRACE3Workload :
	public Workload
{
public:
	TRACE3Workload(const char* filename);
	~TRACE3Workload(void);

	Event getNextEvent();

	bool eow() { return ( feof(infile) != 0 ); };

	int getl() { return l; }

	// Looks into the file and gets the values of m and l
	static void peekValues (const char* filename, int& m, int& l);

protected:
	FILE* infile;

	int l;
	int il;
	char header[256];
};

#endif