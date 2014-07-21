
#ifndef _HASHDATA_H_
#define _HASHDATA_H_

#include <string>
using namespace std;

class HashData
{
public:
	HashData(void);
	~HashData(void);

	static int getHashAlgo (string& hashalgorithm);
	static int getHashDataSize (int hash_algo);

	// Sets the hash for the event depending on the hash function being used 
	static int hash (double time, int op, int sector, char* hash, int hash_algo);

private:
	// ---------------------------------------------------------------------------
	// The hash functions. The hash is the calculation based on the current sector
	// and the time, op and sector.
	// ---------------------------------------------------------------------------
	// The hashing algorithms

	// hash1 only uses the sector to calculate a one char hash
	static void hash1 (double time, int op, int sector, char* hash);
};

#endif