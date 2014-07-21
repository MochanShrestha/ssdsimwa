#include "SingleWriteBlockLayoutManager.h"


SingleWriteBlockLayoutManager::SingleWriteBlockLayoutManager(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& cb_wb, int start) : CopybackLayoutManager (s, _sm, fm)
{
	int rules[] = {0, INT_MAX};
	cb_wb = new WriteBlocks (1, rules, start, s, _sm);
	this->cb_wb = cb_wb;
}


SingleWriteBlockLayoutManager::~SingleWriteBlockLayoutManager(void)
{
}

Address SingleWriteBlockLayoutManager::getNextCopyback(const Page& p)
{
	return cb_wb->getNextWriteAddress(0);
}