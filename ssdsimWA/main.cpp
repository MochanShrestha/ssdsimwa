
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;
#include "board.h"
using namespace LibBoard;
#include <time.h>

#include "workload.h"
#include "UnifRandWorkload.h"
#include "ZipfWorkload.h"
#include "SimParameters.h"
#include "SSDSim.h"
#include "Stat.h"
#include "cfgparser.h"
#include "cQueue.h"
#include "CBRules.h"
#include "VolatilityCalc.h"
#include "StreamStat.h"

#include "FlashMap.h"

void randTest();
void workloadTest();
int configTest();
void SSDSimTest();
void SSDSimTestBig(int argc, char* argv[]);
void mappingTabletest();
void circQTest();
void sizeTest();
void cbRulesTest();
void testLibBoard();
void testLibBoard2();
void testFlashMap();
void sortTest();
void calcVolatility(int argc, char* argv[]);
void testCache(int argc, char* argv[]);
void testStreamStat();

void main(int argc, char* argv[])
{
	SSDSimTestBig ( argc, argv );

	//randTest();
	//workloadTest();
	//SSDSimTest();
	//configTest();
	//circQTest();
	//mappingTabletest();
	//sizeTest();
	//cbRulesTest();
	//testLibBoard2();
	//testFlashMap();
	//sortTest();
	//calcVolatility ( argc, argv );
	//testCache ( argc, argv );
	//testStreamStat();
}

void SSDSimTestBig(int argc, char* argv[])
{
	string cfgfile;

	if (argc == 1)
	{
		cout << "No cfg parameter given" << endl;
		return;
	}
	else
	{
		cfgfile = argv[1];
	}

	SimParameters s( cfgfile );
	cout << s;
	Stat stat(s);

	SSDSim sim(s, stat);
	sim.start();
}

void testStreamStat()
{
	StreamStat stat;

	for (int i=0; i<16; i++)
	{
		stat.add ( (double)i );
	}

	cout << endl;
	cout << "Avg: " << stat.getAvg() << endl;
	cout << "Var: " << stat.getVar() << endl;
	cout << "Std: " << stat.getStd() << endl;
	cout << "Max: " << stat.getMax() << endl;
	cout << "Min: " << stat.getMin() << endl;
}

void testCache(int argc, char* argv[])
{
	Cache c(3);

	Event e;
	Event e1, e2, e3, e4;
	char data[4];

	e1.time  = 1.0;
	e1.op = WORKLOAD_OP_WRITE;
	e1.sector = 12;
	data[0] = 1;
	e1.setData (data , 1);
	c.process ( e1, e );

	e2.time = 2.0;
	e2.op = WORKLOAD_OP_WRITE;
	e2.sector = 45;
	data[0] = 2;
	e2.setData (data , 1);
	c.process ( e2, e );

	e3.time = 3.0;
	e3.op = WORKLOAD_OP_WRITE;
	e3.sector = 33;
	data[0] = 3;
	e3.setData (data , 1);
	c.process ( e3, e );

	cout << c;

	data[0] = 11;
	e1.setData (data, 1);
	c.process ( e1, e );

	cout << c;

	e4.time = 5.0;
	e4.op = WORKLOAD_OP_WRITE;
	e4.sector = 85;
	data[0] = 5;
	e4.setData (data, 1);
	if ( c.process ( e4, e ) )
	{
		cout << "Evict: " << e;
	}

	//list<Event>::iterator iter;
	//cout << "Is in Cache? " << c.isInCache(45, iter) << endl;
	//c.removeAt(iter);

	cout << c;

	//c.remove ( 45 );

	//cout << c;

	//cout << c.evict();

	//cout << c;
}

void calcVolatility(int argc, char* argv[])
{
	string cfgfile;

	if (argc == 1)
	{
		cout << "No cfg parameter given" << endl;
		return;
	}
	else
	{
		cfgfile = argv[1];
	}

	SimParameters s( cfgfile );
	cout << s;
	Stat stat(s);

	VolatilityCalc v(s, stat);
	v.start();
}

typedef std::pair<double, int> my_pair;
bool sort_pred2 (const my_pair& left, const my_pair& right)
{
	return left.first < right.first;
}

void sortTest()
{
    double a[7] = {23, 1, 33, -20, 6, 6, 9};
	
	std::vector<my_pair> data;

	for (int i=0; i<7; i++)
	{
		data.push_back ( my_pair(a[i], i) );
	}
    
	std::sort(data.begin(), data.end(), sort_pred2);

	for (vector<my_pair>::iterator pos=data.begin(); pos!=data.end(); ++pos) 
	{
        cout << pos->first << ' ' << pos->second << ' ' << endl;
    }
}

void testFlashMap()
{
	FlashMap::outputTempChart ("test/tempchart.svg");
}

void testLibBoard2()
{
	Board board;

	board.setPenColorRGBi( 0, 0, 255 );
	board.fillRectangle (0.0, 0.0, 1.0, 1.0);
	board.setPenColorRGBi( 255, 0, 0 );
	board.fillRectangle (0.0, 1.0, 1.0, 1.0);

	board.scale(10);

	board.saveSVG( "test/layout.svg" );
}

void testLibBoard()
{
	Board board;

	// Default unit is the Postscript dot (1/720 inch).
	board.drawRectangle( -1, 1, 2.0, 2.0 );
	board.setPenColorRGBi( 0, 0, 255 );
	board.fillCircle( 0.0f, 0.0f, 0.05f );

	std::vector<Point> points;
	for ( float x = -1.0f; x < 1.0f; x+=0.02f ) {
		points.push_back( Point( x, sin(x*6.28f) ) );
	}
	board.setLineWidth( 1.0 );
	board.setPenColorRGBi( 0, 0, 255 );
	board.drawPolyline( points );
	board.drawArrow( 0, 0, 0.5, 0.5, true );

	board.fillGouraudTriangle( -0.5, 0, Color( 255, 0, 0 ), 
			     0, 0, Color( 0, 255, 0 ),
			     -0.3f, 0.3f, Color( 0, 0, 255 ) );

  
	board.setPenColorRGBi( 255, 1, 1 ).fillGouraudTriangle( -0.5, 0, 1.0f,
							  0, 0, 2.0f,
							  -0.3f, -0.3f, 0.1f );

	board.scale(100);

	board.saveEPS( "test/draw1.eps" );
	board.saveFIG( "test/draw1.fig" );
	board.saveSVG( "test/draw1.svg" );
	board.saveEPS( "test/draw1_Letter.eps", Board::Letter );
	board.saveFIG( "test/draw1_Letter.fig", Board::Letter );
	board.saveSVG( "test/draw1_Letter.svg", Board::Letter );
}

void cbRulesTest()
{
	CBRules rules ("1,2,3,4,5,8,10");
	cout << rules;

	for (int i=0; i<20; i++)
	{
		cout << i << " : " << rules.getCBblock(i) << endl;
	}
}

void sizeTest()
{
	long int x = 0;
	cout << sizeof(x) << endl;
}

void circQTest()
{
	cQueue<int> q (4);
	q.queue (10);
	cout << q;

	q.queue (5);
	cout << q;

	q.queue (2);
	cout << q;

	cout << q.dequeue() << endl;
	cout << q;

	cout << "MAX: " << q.max() << endl;

	q.queue (3);
	cout << q;

	cout << q.dequeue() << endl;
	cout << q;

	q.queue (1);
	cout << q;

	cout << "MAX: " << q.max() << endl;

	cout << q.dequeue() << endl;
	cout << q;

	q.queue (3);
	cout << q;

	q.queue (6);
	cout << q;

	cout << q.dequeue() << endl;
	cout << q;

	cout << "MAX: " << q.max() << endl;

	cout << q.dequeue() << endl;
	cout << q;

	cout << q.dequeue() << endl;
	cout << q;
	
	cout << q.dequeue() << endl;
	cout << q;
}

void mappingTabletest()
{
	SimParameters s("sim.cfg");
	MappingTable mt (s);

	// Test setting the mapping table
	int sector = 100;
	Address a (3, 4);
	mt.addMapping (sector, a);
	cout << mt.getAddress(sector) << endl;
	mt.removeMapping (sector);
	cout << mt.getAddress(sector) << endl;
	mt.addMapping (sector, a);
	cout << mt.getAddress(sector) << endl;
	mt.removeMapping (sector);
	cout << mt.getAddress(sector) << endl;
}

void SSDSimTest()
{
	int l = 100000;
	int m = 250;
	double r = 0.9;

	//UnifRandWorkload w (m, r, l);
	ZipfWorkload w (m, r, 1.0, l, (int)time(NULL), true);
	SimParameters s("sim.cfg");
	s.n = 15;
	s.c = 16;
	s.l = l;
	s.psize = 1;

	Stat stat(s);

	SSDSim sim(s, stat);
	sim.start();
}

int configTest()
{
    /* load sample config */
    ConfigParser_t cfg;
    if (cfg.readFile("sim.cfg"))
    {
        printf("Error: Cannot open config file 'example.cfg'\n");
        return 1;
    }

    string section, option;
    int n, c;

    section = "Flash";
    option = "n";
    
    if (cfg.getValue(section, option, &n))
        cout << "Section/option [" << section << "] " << option << " found - value: " << n << endl;
    else
        cout << "Section/option [" << section << "] " << option << " not found" << endl;

    option = "c";

    /* get double value */
    if (cfg.getValue(section, option, &c))
        cout << "Section/option [" << section << "] " << option << " found - value: " << c << endl;
    else
        cout << "Section/option [" << section << "] " << option << " not found" << endl;

    return 0;
}

void workloadTest()
{
	int l = 1000;
	int m = 250;
	int seed = 100;
	double r = 0.8;

	//UnifRandWorkload w (m, r, seed);
	ZipfWorkload w (m, r, 1.0, l, seed, true);
	Event e;

	for (int i=0; i<l; i++)
	{
		e = w.getNext();
		cout << e;
	}
}

void randTest()
{
	int n; 
	int t = 100;

	for (int i=0; i<t; i++)
	{
		n = rand();
		double d = (double)n/RAND_MAX;
		cout << d << endl;
	}
}
