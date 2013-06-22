/*
 * Definitions of memory management functions
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 04/06/13, Matthieu Hughes
 *		removed struct and rdf stuff
 */

#include "mem.h"
#include "../struct/param_struct.h"
#include "../struct/part_struct.h"
#include "../struct/cell_struct.h"
#include "../struct/hist_struct.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * -----------------------------------------------------------------
 * Allocate memory for arrays of particle structures and cell lists
 * 
 * Also initialize values to 0
 * -----------------------------------------------------------------
 */
void allocateMemory(void)
{
    int i,d;
    // particle array
    part = malloc(p.Ntot*sizeof(particle));
    if (part==NULL)
        mallocError("part");

    for (i=0;i<p.Ntot;i++)
    {
        part[i].partNum = i;
        part[i].next = NULL;

        for (d=0;d<3;d++)
        {
            part[i].r[d] = 0.0;
            part[i].v[d] = 0.0;
            part[i].fCons[d] = 0.0;
            part[i].fHarm[d] = 0.0;
            part[i].fDiss[d] = 0.0;
            part[i].fRand[d] = 0.0;
            part[i].fDrive[d] = 0.0;
	}
    }

    cell = malloc(p.Ncell*sizeof(cellList));
    if (cell==NULL)
        mallocError("part");

    for (i=0;i<p.Ncell;i++)
    {
        cell[i].index = i;
        cell[i].head = NULL;
        for (d=0;d<27;d++)
            cell[i].neighbour[d] = NULL;
    }
}

/*
 * ------------------------------------------------------------------
 * Print error message if memory can't be allocated and exit program
 * Exit program
 * ------------------------------------------------------------------
 */
void mallocError(char *str)
{
    fprintf(stderr,"Error allocating memory to %s\n",str);
    exit(2);
}

/*
 * ------------------------------------------
 * Free allocated memory
 * including what was allocated in initHists
 * ------------------------------------------
 */
void freeMemory(void)
{
    free(part);
    free(cell);
    free(h.dens);
    free(p.currentMonPos);
    free(p.prevMonPos);
}

