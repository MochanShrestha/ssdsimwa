
#include "ETTILayoutManager.h"


ETTILayoutManager::ETTILayoutManager(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& w_wb, int start) : WriteLayoutManager ( s, _sm, fm )
{
	w_wb = new WriteBlocks (s.lm_wrules, 0, s, _sm);
	this->w_wb = w_wb;
}


ETTILayoutManager::~ETTILayoutManager(void)
{
	delete w_wb;
}

Address ETTILayoutManager::getNextWrite(const Event* e)
{
	Address a;

	int etti = (int)(e->etti);
	if ( etti < 0 )
		etti = 0;
	int b = w_wb->findWriteBlock ( etti );
	a = w_wb->getNextWriteAddress (b);

	return a;
}

bool ETTILayoutManager::needNewBlock()
{
	return w_wb->needNewBlock();
}

void ETTILayoutManager::setNewBlock (Address& a)
{
	w_wb->setNewBlock ( a );
}