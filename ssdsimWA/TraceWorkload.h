
#ifndef _TRACEWORKLOAD_H_
#define _TRACEWORKLOAD_H_

#include <fstream>
using namespace std;

#include "workload.h"


class TraceWorkload :
	public Workload
{
public:
	TraceWorkload(int m, const char* filename, bool start);
	~TraceWorkload(void);

	bool eow() { return tracefile.eof(); };

protected:
	ifstream tracefile;
};

#endif