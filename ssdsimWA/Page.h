#ifndef _PAGE_H_
#define _PAGE_H_

#include <iostream>
using namespace std;
#include <assert.h>

class Page
{
public:
	Page(void);
	~Page(void);

	// sets the memory by the block
	void setMemory (char* mem) { memory = mem; }
	char* getMemory() { return memory; }
	void setData (const char* data) { memcpy(memory, data, mem_size); }

	// Size of the memory of each page
	static void setMemSize (int size) { mem_size = size; }
	static int  getMemSize() { return mem_size; }

	// set the state of the page
	void setState (int state) { this->state = state; }
	void setClean()   
	{ 
		setState(-1); 
		sector=-1; 
		copyback_count = 0; 
		timestamp = -1.0; 
		writestamp = -1;
		memset (memory, 0, mem_size);
	};
	void setInvalid() { setState(0); sector=-1; copyback_count = 0; }
	void setValid (double ts, const char* data, long long int ws)   
	{ 
		setState(1); 
		copyback_count = 0; 
		timestamp = ts; 
		writestamp = ws;
		setData (data);
	}

	// get the state of the page
	int getState() const   { return state; }
	bool isClean() const   { return state == -1; }
	bool isInvalid() const { return state == 0; }
	bool isValid() const   { return state == 1; }

	// Copyback count functions. setCopybackCout is used for debugging only. Otherwise
	// the copyback counts are set by themselves when pages are copied
	void setCopybackCount(int c) { copyback_count = c; }
	int getCopybackCount() const { return copyback_count; }

	// sector accessor and get functions
	int getSector() const { return sector; }
	void setSector(int s) { sector = s; }

	// timestamp accessor and setter functions
	double getTimestamp() const { return timestamp; }
	void setTimestamp (double ts) { timestamp = ts; }

	// writestamp accessor and setter function
	long long int getWriteStamp() const { return writestamp; }
	void setWriteStamp (long long int ws) { writestamp = ws; }

	// copyback a page
	Page& operator = (const Page& p);

	// write out the contents of the page
	friend ostream &operator<< (ostream &stream, Page& p);

protected:
	// state of the page
	int state;

	// sector that is stored in the page
	int sector;

	// the copyback count of the page
	int copyback_count;

	// the timestamp when the page was written
	double timestamp;

	// the expected time when the data in the page will be invalidated
	double etti;
	// expected invalidation time
	double eit;
public:
	void setETTI ( double e ) { etti = e; }
	double getETTI() { return etti; }
	void setEIT ( double e ) { eit = e; }
	double getEIT() { return eit; }

protected:
	// the writestamp
	long long int writestamp;

	// the memory where the page stores the data
	char* memory;

	// the size of the memory data unit. This is static and set by the block
	static int mem_size;
};

#endif
