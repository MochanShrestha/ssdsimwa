
#ifndef _EVENT_H_
#define _EVENT_H_

#define EVENT_DATA_SIZE	4

#include <iostream>
using namespace std;

class Event
{
public:
	Event(void);
	~Event(void);

	// Print out the conents of the Event
	friend ostream &operator<<(ostream &stream, Event& e);

	// Only used for the list in the cache. Only matches the sector
	bool operator==(const Event &e) const;

	const char* getData() const { return data; }
	void setData (const char *newdata, int data_size) { memcpy(data, newdata, data_size); }

public:
	// This is the time in seconds that the event has occured in
	double time;

	// This is the operation of the event
	int op;

	// This is the sector that the event is meant for
	int sector;

	// The region the workload (if using TRACE3 file or predicting the result. Default set to 1)
	int region;

	// The expected time in the future that the data will be invalid
	double etti;
	// The actual expected invalidation time (etti gives the time to invalidation rather than the actual time from when the event was generated)
	double eit;

protected:
	// This is the data for the event
	char data[EVENT_DATA_SIZE];
};

#endif