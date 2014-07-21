#include "WSingleWriteBlockLayoutManager.h"


WSingleWriteBlockLayoutManager::WSingleWriteBlockLayoutManager(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& cb_wb, int start) : CopybackLayoutManager (s, _sm, fm)
{
	this->cb_wb = cb_wb;
}


WSingleWriteBlockLayoutManager::~WSingleWriteBlockLayoutManager(void)
{
}

Address WSingleWriteBlockLayoutManager::getNextCopyback(const Page& p)
{
	return cb_wb->getNextWriteAddress(0);
}