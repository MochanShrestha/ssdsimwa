#include "Cache.h"


Cache::Cache(int cachesize)
{
	// Get the size and see if the cache is being used or not
	size = cachesize;
	if ( size == 0 )
		cacheUsed = false;
	else
		cacheUsed = true;
}


Cache::~Cache(void)
{
}

void Cache::add ( Event& e )
{
	if ( !isFull() )
		events.push_front ( e );
}

void Cache::remove (int i) 
{
	Event e;
	e.op = WORKLOAD_OP_WRITE;
	e.sector = i;
	events.remove (e);
}

Event Cache::evict () 
{
	// Get the last item in the list
	list<Event>::iterator iter = events.end();
	iter--;
	Event e = *iter; 

	// Remove the last item
	events.erase (iter);
	return e; 
}

bool Cache::isFull() 
{ 
	return events.size() == size; 
}

bool Cache::isUsed() 
{ 
	return cacheUsed; 
}

bool Cache::isInCache (int sector, list<Event>::iterator& iter)
{	
	for (iter=events.begin(); iter!=events.end(); ++iter)
	{
		if ( (*iter).sector == sector )
		{
			return true;
		}
	}
	return false;
}

void Cache::removeAt (list<Event>::iterator iter)
{
	events.erase (iter);
}

int Cache::process (Event& e, Event& re)
{
	list<Event>::iterator iter;
	int rv = 0;

	bool inCache = isInCache ( e.sector, iter );
	//cout << "inCache: " << inCache << endl;
	if ( inCache )
	{
		removeAt ( iter );
		add (e);
	}
	else
	{
		if ( isFull() )
		{
			re = evict ();
			rv = 1;
			//cout << "Evit: " << re;
		}
		add (e);
	}

	return rv;
}

ostream &operator<<(ostream &stream, Cache& c)
{
	cout << "Cache used: " << c.events.size() << '/' << c.size << endl;

	list<Event>::iterator i;
	for (i=c.events.begin(); i != c.events.end(); ++i)
	{
		cout << *i;
	}
	cout << endl;

	return stream;
}