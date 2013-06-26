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
    
    /* Particle array */
    part = malloc(p.Ntot*sizeof(particle));
    if (part == NULL)
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

    /* CellList array */
    cell = malloc(p.Ncell*sizeof(cellList));
    if (cell == NULL)
        mallocError("part");

    for (i=0;i<p.Ncell;i++)
    {
        cell[i].index = i;
        cell[i].head = NULL;
        for (d=0;d<27;d++)
            cell[i].neighbour[d] = NULL;
    }

    /* Density histogram */
    if (p.ydens)
    {
        p.dens = (int*)malloc(p.NbinDens*sizeof(int));
        if (p.dens == NULL)
            mallocError("dens");
        
        for (i=0;i<p.NbinDens;i++)
            p.dens[i] = 0;
    }

    /* Arrays for monomer positions */
    if (poreWidth > 0)
    {
        p.prevMonPos = (int*)malloc(p.Nmon*sizeof(int));
        if (p.prevMonPos == NULL)
            mallocError("preMonPos");
        p.currentMonPos = (int*)malloc(p.Nmon*sizeof(int));
        if (p.currentMonPos == NULL)
            mallocError("currentMonPos");
    
        for (i=0;i<p.Nmon;i++)
            p.prevMonPos[i] = p.currentMonPos[i] = 0;
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
 * ------------------------------------------
 */
void freeMemory(void)
{
    free(part);
    free(cell);
    free(p.dens);
    free(p.currentMonPos);
    free(p.prevMonPos);
}

