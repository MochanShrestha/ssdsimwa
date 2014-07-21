
#ifndef _WSINGLE_WRITEBLOCK_LAYOUT_MANAGER_
#define _WSINGLE_WRITEBLOCK_LAYOUT_MANAGER_


#include "copybacklayoutmanager.h"

class WSingleWriteBlockLayoutManager :
	public CopybackLayoutManager
{
public:
	WSingleWriteBlockLayoutManager(SimParameters& s, StateManager& _sm, Flash& fm, WriteBlocks*& cb_wb, int start);
	~WSingleWriteBlockLayoutManager(void);
	
	Address getNextCopyback(const Page& p);

protected:
	WriteBlocks* cb_wb;

};

#endif