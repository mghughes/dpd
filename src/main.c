/*  
 * Main function and global declarations for DPD simulation program
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 * 
 * last update: 13/06/13, Matthieu Hughes
 */

/* DPD header files */
#include "sim/calc.h"
#include "init/cell.h"
#include "struct/cell_struct.h"
#include "struct/file_struct.h"
#include "init/io.h"
#include "init/mem.h"
#include "init/param_init.h"
#include "struct/param_struct.h"
#include "init/part_init.h"
#include "struct/part_struct.h"
#include "sim/sim_helper.h"
#include "sim/sim_update.h"

/* Library header files */
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>	// for getpid function
#include <unistd.h>     //       "	

/* Global structures */
particle *part;
cellList *cell;
params p;
files f;

/* Exit values for DPD program:
 *		0 - exit from end of main  - success
 *	or	0 - exit from calcNseg function (complete translocation) - success
 *		1 - exit from fileOpenError - failure
 *		2 - exit from mallocError - failuire
 */


/*
 * --------------
 * MAIN FUNCTION
 * --------------
 */
int main()
{
    /* Initial time */
    double ti = (double)clock()/CLOCKS_PER_SEC;
	
    /* Get simulation parameters */
    getParams();
    calcParams();

    /* Seed program using process ID */
    srandom( getpid() );

    /* Open output files and print simulation parameters read */
    openFiles();
    printParams();

    /* Initialize particles */
    allocateMemory();
    initFluid();


    /* Initialize polymer, wall, and pore */
    if (p.wallLayers > 0)
    {
	if (p.poreWidth > 0)
	{
            initWallWithPore();
            initPolyInPore();
	}
        else
        {
            initWall();
            initPoly();
	}
    }
    else // No wall, but still initialize the polymer
        initPoly();

    /* Initialize cells/particle lists */
    initCells();
    constructList();

    /* Sample initial quantities */ 
    p.nsamp = 0;
    if (p.eqTime == 0.0)
    {
        if (p.poreWidth > 0)
            calcNseg(ti);

        calcReRg();
        if (p.ydens)
            updateDens();
        if (p.yaveTemp)
            averageTemp();
        if (p.ymonPos)
            printInfo('p','m');
		
        p.nsamp++;
    }

    /* MAIN SIMULATION LOOP */
    int step = 1;
    for (p.elapsedTime=p.dt;p.elapsedTime<=p.tMax;p.elapsedTime+=p.dt,step++)
    {	
        updateSys();

        /* Sample quantites */
        if (0==step%p.freqSamp && p.elapsedTime>=p.eqTime)
        {
            if (p.poreWidth > 0)
                calcNseg(ti);

            calcReRg();
            if (p.ydens)
                updateDens();
            if (p.yaveTemp)
                averageTemp();
            if (p.ymonPos)
                printInfo('p','m');
			
            p.nsamp++;
	}

	/* Print histograms and flush output file buffers */
	if (0 == step%p.freqOut)
        {
            if (p.ydens)
                printDens();
            
            flushFiles();
        }
    }

    /* Print hists at end of simulation */
    if (p.ydens)
        printDens();

    freeMemory();

    /* Print run time */
    double tf = (double)clock()/CLOCKS_PER_SEC;
    fprintf(f.log,"\n%.2f\truntime(s)\n",tf-ti);
    closeFiles();

    exit(0);
}
