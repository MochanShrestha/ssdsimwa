
#ifndef _CQUEUE_H_
#define _CQUEUE_H_

/*
	A circular queue based on an array.
*/

#include <iostream>
using namespace std;

template <class type>
class cQueue
{
public:
	cQueue(const int size);
	~cQueue(void);

	// queue and dequeue elements
	void queue (type val);
	type dequeue ();

	// check the status of the queue
	bool isFull() { return length == size; }
	bool isEmpty() { return length == 0; }

	// get the max value in the queue
	type max();

	// gets the value at location i. Doesn't check if i is valid!
	type getValue(int i) { return buffer[(start+i)%size]; }

	// get the length of the queue
	int getLength() { return length; }

	template <class type>
	friend ostream &operator<< (ostream &stream, const cQueue<type>& a);

protected:
	type *buffer;
	int size;

	int start, end;
	int length;
};

//#include "cQueue.cpp"


#endif