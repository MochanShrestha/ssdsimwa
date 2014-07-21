#include "SingleWriteLM.h"


SingleWriteLM::SingleWriteLM(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& w_wb, int start) : WriteLayoutManager (s, _sm, fm)
{
	int rules[] = {0, INT_MAX};
	w_wb = new WriteBlocks (1, rules, start, s, _sm);
	this->w_wb = w_wb;
}


SingleWriteLM::~SingleWriteLM(void)
{
}

Address SingleWriteLM::getNextWrite(const Event* e)
{
	Address a = w_wb->getNextWriteAddress(0);
	return a;
}