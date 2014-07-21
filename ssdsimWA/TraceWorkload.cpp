#include "TraceWorkload.h"


TraceWorkload::TraceWorkload(int m, const char* filename, bool start) : Workload (m, start), tracefile (filename)
{
}


TraceWorkload::~TraceWorkload(void)
{
}
