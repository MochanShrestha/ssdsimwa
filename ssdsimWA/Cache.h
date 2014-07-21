
#ifndef _CACHE_H_
#define _CACHE_H_

#include <list>

#include "workload.h"
#include "Event.h"

class Cache
{
public:
	Cache(int cachesize);
	~Cache(void);

	// Add the event to the queue
	void add ( Event& e );

	// Removes the cached entry from the list
	void remove (int i);

	// Evicts a stored item from the list
	Event evict ();

	// Check if the cache is full or not
	bool isFull();

	// Check is the cache is being used
	bool isUsed();

	// Returns if the sector is in the cache or not. If found, the iterator is valid
	bool isInCache (int sector, list<Event>::iterator& iter);

	// Removes the sector at the given position
	void removeAt (list<Event>::iterator iter);

	// Process the event. If an event is evicted, it is re
	// Return value is 1 if re is valid and 0 if no eviction
	int process (Event& e, Event& re);

	// Print out the conents of the Cache
	friend ostream &operator<<(ostream &stream, Cache& c);

private:
	// The queue where we store the events
	list<Event> events;

	// flag to see if we are using the cache or not
	bool cacheUsed;

	// the number of pages that can be cached
	int size;
};

#endif