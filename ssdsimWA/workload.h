
#ifndef _WORKLOAD_H_
#define _WORKLOAD_H_

#define WORKLOAD_OP_WRITE	1
#define WORKLOAD_OP_DEL		2

#include "Event.h"

#include <random>
using namespace std::tr1;
#include <time.h>

class Workload
{
public:
	Workload(int m, bool start = false);
	virtual ~Workload(void);

	// Gets the next event in the workload
	virtual Event getNextEvent() = 0;

	// Gets the next event in the workload. Base in on getNextEvent and calcETTI
	virtual Event getNextEventETTI()
	{
		Event e;
		e = getNextEvent();
		e.etti = calcETTI (e);
		return e;
	}

	// Calculate the ETTI for the event
	virtual double calcETTI (Event& e) { return -1; };


	// Gets the next event
	Event getNext();

	// Gets the next event with ETTI
	Event getNextETTI();


	// Check if this is the end of the workload
	virtual bool eow() = 0;

	int getM() { return m; }

	int getCounter() { return counter; }

protected:
	// the number of sectors in the workload
	int m;
	
	// Keeps track of the number of operations that have occured
	int counter;

	// creates and readies a random permutation of the secotrs m if we want to write all the sectors to disk
	bool startwithallsectors;
	// the permutation of the m sectors in an array
	int *perm_m;

	// Get an event from the list of permuted values
	Event getNextPermEvent();
	int perm_index;

	// Fisher-Yates shuffle support
	std::tr1::uniform_int<int> unif_int;
	std::tr1::mt19937 gen_int;
	int rand_int(int n);
	void shuffle(int *_array, int n);
	void shuffle2(int *_array, int n);
};

#endif