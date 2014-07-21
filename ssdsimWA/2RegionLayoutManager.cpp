#include "2RegionLayoutManager.h"


Region2LayoutManager::Region2LayoutManager(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& w_wb, int start) : WriteLayoutManager (s, _sm, fm)
{
	int rules[] = {0, INT_MAX};
	w_wb = new WriteBlocks (3, rules, start, s, _sm);
	this->w_wb = w_wb;
}


Region2LayoutManager::~Region2LayoutManager(void)
{
}

Address Region2LayoutManager::getNextWrite(const Event* e)
{
	Address a;

	switch ( e->region )
	{
	case -1:
		// Static
		a = w_wb->getNextWriteAddress(2);
		break;
	case 0:
		// Cached
		cout << "Cached sector in the workload. Something is wrong!" << endl;
		exit(-1);
		break;
	case 1:
		a = w_wb->getNextWriteAddress(0);
		break;
	case 2:
		a = w_wb->getNextWriteAddress(1);
		break;
	}
	
	return a;
}