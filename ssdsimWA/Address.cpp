#include "Address.h"

Address::Address(void)
{
	makeInvalid();
}

Address::Address (int b, int p) 
{ 
	block = b; 
	page = p; 
}


Address::~Address(void)
{
}

bool Address::isOnDisk() const 
{ 
	return block >= 0;
}

bool Address::isCached() const
{
	return block == ADDRESS_CACHED;
}

void Address::makeInvalid() 
{
	block = -1; 
	page = -1;
}

ostream &operator<< (ostream &stream, Address& a)
{
	if (a.isOnDisk())
	{
		stream << a.block << "." << a.page;
	}
	else
	{
		stream << "I";
	}
	return stream;
}

Address& Address::operator = (const Address& a)
{
	block = a.block;
	page = a.page;
	return *this;
}