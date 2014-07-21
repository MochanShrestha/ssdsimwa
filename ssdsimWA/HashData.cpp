#include "HashData.h"


HashData::HashData(void)
{
}


HashData::~HashData(void)
{
}

int HashData::getHashAlgo (string& hashalgorithm)
{
	if (hashalgorithm == "hash1")
		return 1;
	return 0;
}

int HashData::getHashDataSize (int hash_algo)
{
	switch ( hash_algo )
	{
	case 1:
		return 1;
	default:
		return 0;
	}
}

int HashData::hash (double time, int op, int sector, char* hash, int hash_algo)
{
	// only do this if hash is not NULL
	if ( hash )
	{
		switch ( hash_algo )
		{
		case 1:
			hash1 (time, op, sector, hash);
			return 0;
		default:
			return -1;
		}
	}

	return 0;
}

void HashData::hash1 (double time, int op, int sector, char* hash)
{
	// s^2 and modded by 255 through the type conversion
	hash[0] = (char)( sector * sector );
	//hash[0] = (char)(sector);
}