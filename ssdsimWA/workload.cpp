#include "workload.h"
#include <time.h>

Workload::Workload(int m, bool start) : unif_int (0, m)
{
	this->m = m;
	counter = 0;
	startwithallsectors = start;

	// Allocate and setup the permutation array if we are going to use it
	if ( startwithallsectors )
	{
		perm_m = new int[m];
		for (int i=0; i<m; i++)
			perm_m[i] = i;
		gen_int.seed((unsigned long)time(NULL));

		shuffle (perm_m, m);
		shuffle2 (perm_m, m);
		shuffle (perm_m, m);
		shuffle2 (perm_m, m);
		shuffle (perm_m, m);
		shuffle2 (perm_m, m);
		
		perm_index = 0;
	}
	else
	{
		// Not going to use the permutation array
		perm_m = NULL;
		perm_index = INT_MAX;
	}
}


Workload::~Workload(void)
{
	delete[] perm_m;
}

Event Workload::getNextPermEvent()
{
	Event e;

	if ( perm_index >= m )
		return e;

	e.time = perm_index;
	e.sector = perm_m [perm_index];
	e.op = WORKLOAD_OP_WRITE;

	perm_index++;

	//cout << perm_index << " " << e.sector << endl;

	return e;
}

Event Workload::getNext()
{
	// Increment the counter of the number of operations
	counter++;

	if ( perm_index < m)
		return getNextPermEvent();

	// return the next event from the workload if permutation array work is done
	return getNextEvent();
}

Event Workload::getNextETTI()
{
	// Increment the counter of the number of operations
	counter++;

	if ( perm_index < m)
	{
		Event et;
		et = getNextPermEvent();
		et.etti = calcETTI ( et );
		return et;
	}

	// return the next event from the workload if permutation array work is done
	return getNextEventETTI();
}

// ------
// code from http://stackoverflow.com/questions/3343797/is-this-c-implementation-of-fisher-yates-shuffle-correct
// -------

int Workload::rand_int(int n) {
	int limit = m - m % n;
	int rnd;

	do {
	rnd = unif_int(gen_int);
	} while (rnd >= limit);
	return rnd % n;
}

void Workload::shuffle2(int *_array, int n) 
{
	int j, temp;
	temp = _array[0];
	for (int i=0; i<n; i++)
	{
		j = rand_int(i + 1);
		temp = _array[j];
		_array[j] = _array[i];
		_array[i] = temp;
	}
}

void Workload::shuffle(int *_array, int n) 
{
  int i, j, tmp;

  for (i = n - 1; i > 0; i--) 
  {
    j = rand_int(i + 1);
    tmp = _array[j];
    _array[j] = _array[i];
    _array[i] = tmp;
  }
}
