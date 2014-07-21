SSDSim
======

SSD Simulator for Write Amplification and Wear Leveling
by Mochan Shrestha

USAGE: SSDSimWA.exe <cfgfile>

Since Visual Studio stores the command line configurations somewhere other than the project files, the command
line argument that works for the project structure is:
test/sim.cfg

Uses the following libraries
- LibBoard: A vector graphics C++ library (LibBoard: A vector graphics C++ library) for outputting heatmaps in eps
- FreeGlut (http://freeglut.sourceforge.net/) for real time visualization
- CFGparser (http://cfgparser.sourceforge.net/) for reading the simulation configuration files