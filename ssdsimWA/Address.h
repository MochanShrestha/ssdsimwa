#ifndef _ADDRESS_H_
#define _ADDRESS_H_

#define ADDRESS_INVALID		-1
#define ADDRESS_CACHED		-2

#include <ostream>
using namespace std;

class Address
{
public:
	Address(void);
	Address (int b, int p);
	~Address(void);

	bool isOnDisk() const;
	bool isCached() const;
	void makeInvalid();

	Address& operator = (const Address& a);

	friend ostream &operator<< (ostream &stream, Address& a);

	// accesssor and setter functions
	int getBlock() const { return block; }
	void setBlock(int b) { block = b; }
	int getPage() const { return page; }
	void setPage(int p) { page = p; }
	//int getCacheLoc() { return page; }

protected:
	int block;
	int page;
};

#endif