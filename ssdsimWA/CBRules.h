
#ifndef _CBRULES_H_
#define _CBRULES_H_

#include <string>
using namespace std;

class CBRules
{
public:
	CBRules(string rules);
	~CBRules(void);

	// get the number of copyback blocks that need to be used
	int getnCBblocks() { return nCBblocks; }

	// get which copyback block to write to
	int getCBblock (int c);

	// Get the array of rules
	int* getRules() { return cbrules; }

protected:
	// Data about the rules
	int nRules;
	int nCBblocks;
	int* cbrules;

	// get the rules from the string of the rules
	void parseRules(string rules);

	// write out
	friend ostream &operator<< (ostream &stream, CBRules& r);
};

#endif