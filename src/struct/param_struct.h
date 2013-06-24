/*
 *  Definition of struct holding simulation paremeters
 *
 *  Copyright (C) Matthieu Hughes, 2013
 *  matthieuhughes.1@gmail.com
 *
 *  last update: 21/06/13, Matthieu Hughes
 *		removed struct and rdf stuff
 */

#ifndef _PARAM_STRUCT_H
#define _PARAM_STRUCT_H

struct params
{
    // input parameters
    double  
        Aff,        // solvent-solvent repulsion strength (usu. 25)
        Amm,        // monomer-monomer repulsion strength (usu. 25)
        Amf,        // monomer-solvent repulsion strength (usu. 15)
        SIGMA,      // random force strength (usu. 3)
        GAMMA,      // dissipative force strenght (usu. 4.5)
        K,          // spring constant for spring force (usu. 40)
        fDrive,		// nanopore driving force
        density,    // particle density (usu. 3)
        L[3],       // box side lengths {x,y,z}
        wallDensity,// density of wall particles (usu. 3)
                            // set to <=0 for no wall
    	poreWidth,  // (square) nanopore width
                            // set to <0 for no pore
	dt,         // timestep (usu. 0.04)
	tMax,       // simulation time
        eqTime;     // time for system to equilibrate

    int     
	Nmon,       // polymer chain length
        wallLayers, // number of wall particle layers (usu. 2)
	freqSamp,	// sample frequency
	freqOut,	// print frequency
        ydens,      // yes to print density profile
        ydensxy,    // yes to print xy density profiles
        yaveTemp,   // yes to print average temperature
        ymonPos;    // yes to print monomer positions
		
    // calculated parameters/constants
    double  
	L_HALF[3],		// half of box side lengths
        wallDist,		// distance between wall parts in 1D
        Awall,			// solvent-wall and mon-wall force strength
        xPoreBoundary[2],	// pore boundary in x directions
        yPoreBoundary[2],	//  "      "        y     "
        zPoreBoundary[2],	//  "      "        z     "
	recNmon,		// reciprocal of Nmon
        recRootdt,		// reciprocal of sqrt(dt)
        root3,			// sqrt(3)
	RMAXHARM,		// bond-breaking distance
	REQ,		        // eq. bond length
	MAXFORCEHARM,		// force when r>RMAXHARM
	diff2,			// RMAXHARM-REQ
	recdiff2sq,	        // reciprocal of diff2*diff2
        cellDist[3],		// distance between cells in 1D
        Ncell,			// number of cells
        elapsedTime,		// time counter
        binWidthDens;           // width of each density bin

    int     
	Ntot,			// total particle number
        Nfluid,	                // total fluid particles (incl. wall parts)
        Nwall,			// total wall particles
        Nwall_x,		// wall particles in x direction
        Nwall_y,		// wall particles in y direction
        nsamp,			// counter for number of sampling steps
        *prevMonPos,		// previous mon positions (for trans calc)
        *currentMonPos,		// current mon positions
        NmonCis,		// number of monomers on cis side
        NmonTrans,		// number of monomers on trans side
        NmonPore,   		// number of monomers in pore
        NbinDens,              // number of bins in density histogram
        *dens;                  // array for density histogram
};

typedef struct params params;
extern params p;

#endif /* _PARAM_STRUCT_H */
