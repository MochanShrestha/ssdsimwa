#include "cQueue.h"

template <class type> 
cQueue<type>::cQueue(const int size)
{
	this->size = size;
	buffer = new type[size];
	start = 0;
	end = 0;
	length = 0;
}


template <class type>
cQueue<type>::~cQueue(void)
{
	delete[] buffer;
}

template <class type>
void cQueue<type>::queue (type val)
{
	if ( !isFull() )
	{
		buffer[end] = val;
		end = (end + 1) % size;
		length++;
	}
}

template <class type>
type cQueue<type>::dequeue ()
{
	if ( !isEmpty() )
	{
		type val = buffer[start];
		start = (start + 1 ) % size;
		length--;
		return val;
	}
	else
	{
		return -1;
	}
}

template <class type>
type cQueue<type>::max()
{
	type m = buffer[start];
	for (int i=1; i<length; i++)
	{
		type val = buffer[ (start+i)%size ];
		if ( val > m )
			m = val;
	}
	return m;
}

template <class type>
ostream &operator<< (ostream &stream, const cQueue<type>& a)
{
	stream << "[ ";
	for (int i=a.start; i<a.start+a.length; i++)
		stream << a.buffer[i%a.size] << " ";
	//stream << "| s:" << a.start << " e:" << a.end << " l:" << a.length;
	stream << " ]\n";
	return stream;
}