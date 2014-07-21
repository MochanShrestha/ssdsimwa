#include "Event.h"


Event::Event(void)
{
	time = 0;
	memset (data, 0, EVENT_DATA_SIZE * sizeof(char));
	op = 0;
	sector = -1;
	region = 1;
	etti = -1;
	eit = -1;
}


Event::~Event(void)
{
}

ostream &operator<<(ostream &stream, Event& e)
{
	stream << e.time << ". ";
	switch (e.op)
	{
	case 1:
		stream << "w";
		break;
	case 2:
		stream << "d";
		break;
	default:
		stream << "u";
		break;
	}
	stream << " " << e.sector;
	stream << " (data: " << (int)(e.data[0]);// << ", ad:" << (int)( (char)e.sector );
	if ( e.etti > 0 )
	{
		stream << ", " << "etti: " << e.etti;
	}
	stream << ")";
	stream << endl;
	return stream;
}

bool Event::operator==(const Event &e) const 
{
	// Only used for the list comparison for removing
	return sector == e.sector;
}