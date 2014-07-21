#include "SSDSim.h"

#include <iostream>
#include <iomanip>
using namespace std;

#include <time.h>

#include "FlashMap.h"


// Constants that are used

// The maximum size of the heatmap window
#define MAX_HEATMAP_WIDTH		1000
#define MAX_HEATMAP_HEIGHT		1000

#define HEATMAP_SPACER			4

/*
 ----------------------------------------------------------------------
 --------------- Some GLUT related stuff ------------------------------
 ----------------------------------------------------------------------
*/

void drawBitmapText(char *string,float x,float y,float z) 
{  
	char *c;
	glRasterPos3f(x, y,z);
	for (c=string; *c != '\0'; c++) 
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
	}
}

void changeSize (int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio =  w * 1.0f / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

SSDSim* current_sim;

void renderScene2 (void)
{
	current_sim->renderScene2();
}

void changeSize2 (int w, int h)
{
	current_sim->updateHeatmapWindowSize(w,h);
	changeSize(w,h);
}


// Utility function that sets the window position to x, y. I just use z=0 and w=1
void window_pos( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   GLfloat fx, fy;

   /* Push current matrix mode and viewport attributes */
   glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );

   /* Setup projection parameters */
   glMatrixMode( GL_PROJECTION );
   glPushMatrix();
   glLoadIdentity();
   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   glLoadIdentity();

   glDepthRange( z, z );
   glViewport( (int) x - 1, (int) y - 1, 2, 2 );

   /* set the raster (window) position */
   fx = x - (int) x;
   fy = y - (int) y;
   glRasterPos4f( fx, fy, 0.0, w);

   /* restore matrices, viewport and matrix mode */
   glPopMatrix();
   glMatrixMode( GL_PROJECTION );
   glPopMatrix();

   glPopAttrib();
}




SSDSim::SSDSim(SimParameters& s, Stat& st) : sp(s), f(s), c(s.cachesize), ftl(f,c,s,st), stat(st)
{
	if ( s.hash_size > 0 )
		hash = new char[s.hash_size];
	else
		hash = NULL;

	// Create the workload
	if ( sp.wtype == "s")			// synthetic workload
	{
		//int seed = (int)time(NULL);
		int seed = 100;

		switch (sp.wdist)
		{
		case 1:
			w = new UnifRandWorkload (sp.m, sp.r, sp.static_sectors, sp.l, seed, sp.startfullsectors);
			break;
		case 2:
			w = new ZipfWorkload (sp.m, sp.r, sp.static_sectors, sp.l, seed, sp.startfullsectors);
			break;
		}
	}
	else if ( sp.wtype == "t")		// trace workload
	{
		if ( sp.tfiletype == "dat" )
		{
			w = new DatTraceWorkload ( sp.m, sp.tfilename.c_str(), sp.startfullsectors );
		}
		else if ( sp.tfiletype == "trace" )
		{
			w = new TRACE12Workload ( sp.tfilename.c_str() );
			TRACE12Workload *tw = (TRACE12Workload*)w;
			sp.l = tw->getl();
		}
		else if ( sp.tfiletype == "TRACE3" )
		{
			w = new TRACE3Workload ( sp.tfilename.c_str() );
		}
	}
	else
	{
		cout << "Could not figure out the workload input type. Exiting ..." << endl;
		exit(-1);
	}

	// Update the current sim pointer to get the GLUT stuff working
	current_sim = this;

	// Initalize the writing buffer
	hm_w = MAX_HEATMAP_WIDTH;
	hm_h = MAX_HEATMAP_HEIGHT;

	// Update the size of the window for showing the heat map
	hm_w = ( sp.c + HEATMAP_SPACER ) * sp.hm_blocksperrow;
	hm_h = min(MAX_HEATMAP_HEIGHT, sp.n / (sp.hm_blocksample * sp.hm_blocksperrow));
	hmw_w = hm_w;
	hmw_h = hm_h;

	// Allocate the heatmap drawing buffer
	heatmap_buffer = new unsigned char[hm_w*hm_h*3];
	// CLean up the buffer for the blank spaces
	memset (heatmap_buffer, 0, hm_w*hm_h*3);
}


SSDSim::~SSDSim(void)
{
	cout << "Total Copybacks: " << stat.getTotalCopybacks();

	if (hash)
		delete[] hash;

	if (heatmap_buffer)
		delete[] heatmap_buffer;
}

void SSDSim::start()
{
	if (sp.wtype == "t")
	{
		if ( sp.viz )
		{
			if ( sp.tfiletype == "TRACE3" )
			{
				glutStart2();
			}
			else
			{
				// Have to write the glut version for start1 but we have traces with pre-written values of l
				start1();
			}
		}
		else
		{
			start1();
		}
	}
	else if (sp.wtype == "s")
	{
		if ( sp.viz ) 
		{
			glutStart2();
		}
		else
		{
			start2();
		}
	}
}

void SSDSim::start1()
{
	Event e;

	int pcomplete = 0;

	clock_t start = clock();
	cout.setf(ios::fixed);

	cout << "-----------------------------------------------------" << endl;

	// Loop over the length of the workload
	do
	{
		// Get the next event
		e = w->getNext();

		// hash
		hashEvent (e);

		// operate
		if (e.time > 0)
			ftl.operate(e);

		sp.current_timestamp = e.time;

		// calculate the percentage complete
		int tpcomplete = (int)( ((double)w->getCounter())/sp.l * 100.0);

		//if ( w->getCounter() % 100000 == 0)
		//	cout << w->getCounter() << '\t' << sp.l << endl;
		
		if (tpcomplete > pcomplete)
		{
			pcomplete = tpcomplete;
			printPercentComplete(w->getCounter());
		}

	} while ( !w->eow() );

	cout << "Counter : " << w->getCounter() << " (" << w->getCounter()/((double)(sp.n*sp.c)) << " ) " << endl;
	simCheck();

	ftl.close();
	stat.simEnd ( f, ftl.getStateManager() );

	clock_t end = clock();
	double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
	cout << endl << "Took " << time_elapsed_in_seconds << "s" << endl;
}

void SSDSim::start2()
{
	int pcomplete = 0;

	start_time = clock();
	cout.setf(ios::fixed);

	// Loop over the length of the workload
	for (long long int i=0; i<sp.l; i++)
	{
		if ( simStep2() < 0 )
			break;

		// calculate the percentage complete
		int tpcomplete = (int)( ((double)i)/sp.l * 100.0);
		if (tpcomplete > pcomplete)
		{
			pcomplete = tpcomplete;
			printPercentComplete(i);
		}
	}

	cout << "Counter : " << w->getCounter() << " (" << w->getCounter()/((double)(sp.n*sp.c)) << " ) " << endl << endl;
	simCheck();

	ftl.close();
	stat.simEnd ( f, ftl.getStateManager() );

	clock_t end = clock();
	double time_elapsed_in_seconds = (end - start_time)/(double)CLOCKS_PER_SEC;
	cout << endl << "Took " << time_elapsed_in_seconds << "s" << endl;
}

void SSDSim::printPercentComplete (long long int i)
{
	double pcomplete = (i*100.0)/(double)(sp.l);

	clock_t now = clock();
	double time_elapsed = (now - start_time)/(double)CLOCKS_PER_SEC;
	double time_to_finish = time_elapsed / pcomplete * (100 - pcomplete);
	
	std::cout.setf(std::ios::fixed);
    std::cout.precision(2);

	cout << pcomplete << "% \tETA: ";
	
	if (time_to_finish > 60.0)
	{
		double min_to_fin = floor(time_to_finish / 60.0);
		cout << setprecision(0) << min_to_fin << "m ";
		cout << setprecision(1) << time_to_finish - min_to_fin*60 << "s";
	}
	else
	{
		cout << setprecision(1) << time_to_finish << "s";
	}

	// Output write amplification and VPE
	/*
	cout << " (" << setprecision(3);
	double wa = stat.calcsWA();
	if (wa > 0)
		cout << wa;
	else
		cout << "-";
	cout << ", VPE: " << setprecision(1);
	double vpe = stat.calcVPE();
	if ( vpe > 0 )
		cout << vpe;
	else
		cout << "-";
	cout << ") ";*/
		
	// Output the age of the garbage collection
	// cout << "GCAge: (" << stat.gcAge.getAvg() << ", " << stat.gcAge.getStd() << ")";

	// Output the number of copybacks so far
	cout << " (CBs: " << stat.getTotalCopybacks() << ")";

	cout << endl;
}

void SSDSim::drawBlockStack (int b, int blockUseCount, float x, float y)
{
	// Constant values for the graphics
	float bWidth = 0.9f;
	float bSpacing = -0.25f;
	float bHeight = -0.2f;

	float x1, x2, y1, y2;
	x1 = x;
	x2 = x+bWidth;

	for (int i = 0; i < sp.c; ++i) 
	{
		// Get the status of the page
		Address a ( b, i );
		int page_status = f.getPage(a).getState();
		//cout << a << page_status << endl;

		switch (page_status)
		{
		case -1:
			// Clean - WHITE
			glColor3f (1.0f, 1.0f, 1.0f);
			break;
		case 0:
			// Invalid - RED
			glColor3f (1.0f, 0.0f, 0.0f);
			break;
		case 1:
			// Valid - BLUE
			glColor3f (0.0f, 0.0f, 1.0f);
			break;
		default:
			break;
		}

		y1 = y  + i*bSpacing;
		y2 = y1 + bHeight;

		glBegin(GL_TRIANGLES);
		glVertex3f ( x1, y1, 0.0f );
		glVertex3f ( x2, y1, 0.0f );
		glVertex3f ( x2, y2, 0.0f);
		glEnd();

		glBegin(GL_TRIANGLES);
		glVertex3f ( x1, y2, 0.0f);
		glVertex3f ( x1, y1, 0.0f);
		glVertex3f ( x2, y2, 0.0f);
		glEnd();
	}

	// Write how many times the writeblock has been used
	glColor3f ( 0.0f, 1.0f, 0.0f );
	char number[256];
	_itoa(blockUseCount, number,10);
	drawBitmapText (number, x1, y + (sp.c+1)*bSpacing + bHeight, 0);
}

void SSDSim::drawWriteblocks()
{
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	// Pointer to the writeblocks
	WriteBlocks* w = ftl.getLayoutManager()->getWriteBlocks();
	int nMeters = w->getNWriteBlocks();
	int nBars = sp.c;

	float bLeft = -10.0f;
	float mWidth = 1.0f;
	float bTop = (nBars+1)*0.25f;

	// Set the camera
	gluLookAt(	nMeters/2.0f, (nBars*0.25)/2, 40.0f,
				nMeters/2.0f, (nBars*0.25)/2,  0.0f,
				0.0f, 1.0f,  0.0f);

	// Draw the write blocks
	for (int j=0; j < nMeters; ++j)
	{
		// Get the address of the block
		int wBlock = w->getWriteBlock(j);

		// Draw the block on the screen
		float bx = bLeft + mWidth*j;
		float by = bTop;
		int blockUseCount = w->getBlockUseCount(j);
		drawBlockStack (wBlock, blockUseCount, bx, by);
	}

	// Do the copyback blocks
	WriteBlocks* cb_w = ftl.getLayoutManager()->getCopybackBlocks();
	if ( cb_w != NULL)
	{
		int nMeters_cb = cb_w->getNWriteBlocks();

		for (int j=0; j < nMeters_cb; ++j)
		{
			// Get the address of the block
			int wBlock = cb_w->getWriteBlock(j);

			// Draw the block on the screen
			float bx = bLeft + mWidth* (j+1+nMeters);
			float by = bTop;
			int blockUseCount = cb_w->getBlockUseCount(j);
			drawBlockStack (wBlock, blockUseCount, bx, by);
		}
	}

	glutSwapBuffers();
}

void SSDSim::drawHeatmap()
{
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	// Fill up the buffer
    FlashMap::glOutputMap (sp, f, ftl.getStateManager(), heatmap_buffer, hm_w, hm_h, sp.c + HEATMAP_SPACER);

	// Center where we are going to show the pixels
	float xoff = 0.0f, yoff = 0.0f;
	if ( hmw_w > hm_w )
		xoff = (hmw_w - hm_w)/2.0f;
	if ( hmw_h > hm_h )
		yoff = (hmw_h - hm_h)/2.0f;
	window_pos (xoff, yoff, 0.0f, 1.0f);

	// Blast the pixels onto the screen
	glDrawPixels (hm_w, hm_h, GL_RGB, GL_UNSIGNED_BYTE, heatmap_buffer);

	// Do the double buffering
	glutSwapBuffers();
}

void SSDSim::renderScene2 (void)
{
	// Run a step in the simulation
	for (int i=0; i<sp.viz_steps; i++)
	{
		if ( simStep2() < 0 )
			exit(0);
		sim_i++;

		if ( sim_i % 10000 == 0 )
		{
			printPercentComplete ( sim_i );
		}
	}

	// ----------------------------------------
	// Do the graphics stuff next
	if ( sp.viz_writeblocks)
	{
		glutSetWindow(glwin_writeblocks);
		drawWriteblocks();
	}

	if ( sp.viz_heatmap)
	{
		glutSetWindow(glwin_heatmap);
		drawHeatmap();
	}
}

void SSDSim::glutStart2()
{
	int argc = 0;
	char** argv = NULL;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);

	int x = 100;
	int y = 100;

	if ( sp.viz_writeblocks == true )
	{
		glwin_writeblocks = glutCreateWindow("Writeblocks");
		
		glutPositionWindow(x,y);
		x += 350;
		
		// register callbacks
		glutDisplayFunc(::renderScene2);
		glutReshapeFunc(changeSize);
	}

	if ( sp.viz_heatmap == true )
	{
		glutInitWindowSize (hm_w, hm_h);
		glwin_heatmap = glutCreateWindow("HeatMap");

		glutPositionWindow(x,y);

		glutDisplayFunc(::renderScene2);
		glutReshapeFunc(changeSize2);		
	}

	// The idle function that is not associated with any window
	glutIdleFunc(::renderScene2);

	// enter GLUT event processing cycle
	sim_i = 0;
	start_time = clock();

	glutMainLoop();
}

int SSDSim::simStep2 ()
{
	Event e;

	// Get the next event based on whether we have an ETTI LM
	if ( ftl.getLayoutManager()->getFlag() | FLAG_LM_ETTI )
	{
		e = w->getNextETTI();
		e.eit = sp.current_timestamp + e.etti;
	}
	else
	{
		e = w->getNext();
	}

	hashEvent(e);
	ftl.operate(e);
	sp.current_timestamp = e.time;

	// Check if the conditions are met that we are done with the simulation
	if ( w->eow() )
		return -1;

	return 0;
}


void SSDSim::hashEvent (Event& e)
{
	// Only do this if hash is not NULL or the hash size is bigger than 0
	if ( hash )
	{
		HashData::hash (e.time, e.op, e.sector, hash, sp.hash_algo);
		e.setData (hash, sp.hash_size);
		//cout << (int)(e.getData())[0] << "\t-\t";
		//cout << (int)hash[0] << "\t-\t";
		//cout << e.sector << "\t-\t" << (int)( (char)e.sector ) << endl;
	}
}

void SSDSim::simCheck()
{
	// mapPageSector check
	cout << "Map-Page-Sector Check: ";
	if ( mapPageSectorCheck() )
	{
		cout << "SUCCESS";
	}
	else
	{
		cout << "FAILED!!!";
	}
	cout << endl;

	// Hash Data check
	cout << "Hash Data Check: ";
	if ( hashDataCheck() )
	{
		cout << "SUCCESS";
	}
	else
	{
		cout << "FAILED!!!";
	}

	cout << endl;
}

bool SSDSim::mapPageSectorCheck()
{
	// Go through all the entries of the mapping table
	for (int i=0; i<sp.m; i++)
	{
		// Get the address of the sector in the disk
		Address a = ftl.getAddress(i);
		// If the sector is stored on the disk
		if ( a.isOnDisk() )
		{
			// Get the page associated with the address
			Page& p = f.getPage(a);

			// Check that the sector in the page is the same as the sector in the mapping table
			if ( p.getSector() != i )
			{
				cout << endl << "Sector " << i << ": Address " << a << ", Page: " << p << endl;
				return false;
			}
		}
	}

	return true;
}

bool SSDSim::hashDataCheck()
{
	// The place to store the hash
	char *calcHash = new char[sp.hash_size];

	// Go through all the blocks in the flash memory
	for (int i=0; i<sp.n; i++)
	{
		// And each page in the block
		for (int j=0; j<sp.c; j++)
		{
			// Check if the page is valid
			Page& p = f.getBlock(i).getPage(j);
			if ( p.isValid() )
			{
				// Calculate the hash from the sector and timestamp stored in the flash memory
				HashData::hash ( p.getTimestamp(), WORKLOAD_OP_WRITE, p.getSector(), calcHash, sp.hash_algo );
				char* hash = p.getMemory();
				for (int k=0; k<sp.hash_size; k++)
				{
					if ( hash[k] != calcHash[k] )
					{
						cout << "( Block: " << i << ", Page: " << j << " " << endl;
						cout << p << endl;
						cout << " " << (int)(*calcHash) << " ) " << endl;
						cout << ftl.getAddress(p.getSector()) << endl;
						return false;
					}
				}
			}
		}
	}

	delete[] calcHash;

	return true;
}