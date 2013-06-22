/*
 * Cell list function definitions
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 21/06/13, Matthieu Hughes
 *		removed vecIndex stuff
 */

#include "../struct/cell_struct.h"
#include "cell.h"
#include "../struct/part_struct.h"
#include "../struct/param_struct.h"
#include <stdlib.h>

/*
 * ------------------------------------------------------------------------
 * Inititialize index and neighbour values for the array of cell structures
 * ------------------------------------------------------------------------
 */
void initCells(void)
{
    int ix,iy,iz,jx,jy,jz;
    int i=0,j=0,index_j;

    cellList *cell_i;
    cell_i = cell;
    for (iz=0;iz<(int)p.L[2];iz++){
    for (iy=0;iy<(int)p.L[1];iy++){
    for (ix=0;ix<(int)p.L[0];ix++,i++,cell_i++)
    {
	/* Assign cell index */
        cell_i->index = i;

        /* Loop through neighbours */
        j = 0;
        for (jx=ix-1;jx<=ix+1;jx++){
        for (jy=iy-1;jy<=iy+1;jy++){
        for (jz=iz-1;jz<=iz+1;jz++,j++)
        {
            index_j = (jx+(int)p.L[0])%(int)p.L[0]
                     +(jy+(int)p.L[1])%(int)p.L[1]*(int)p.L[0]
                     +(jz+(int)p.L[2])%(int)p.L[2]*(int)p.L[0]*(int)p.L[1];

            cell_i->neighbour[j] = &cell[index_j];
        }}}
    }}}
    
    /*
    // print neighbours - for debugging purposes
    cellList *cell_j;
    cell_i = cell;
    for (i=0;i<p.Ncell;i++)
    {
        printf("cell %03d %02d %02d %02d neighbours\n",cell_i->index,cell_i->vecIndex[0],cell_i->vecIndex[1],cell_i->vecIndex[2]);

        for (j=0;j<27;j++)
        {
            cell_j = cell_i->neighbour[j];
            printf("\tcell %03d %02d %02d %02d\n",cell_j->index,cell_j->vecIndex[0],cell_j->vecIndex[1],cell_j->vecIndex[2]);

        }
        cell_i++;
    }
    */
}

/*
 * ---------------------------------------------------
 * Construct the linked list of particles in each cell
 * ---------------------------------------------------
 */
void constructList(void)
{
    int i, index;
    cellList *cell_i;
    particle *part_i;

    /* Clear heads of cells */
    cell_i = cell;
    for (i=0;i<p.Ncell;i++,cell_i++)
        cell_i->head = NULL;

    /* put particles in cells (at head of list) */
    part_i = part;
    for (i=0;i<p.Ntot;i++,part_i++)
    {
        index = getCellIndex(part_i);
        cell_i = &cell[index];

	part_i->next = ((cell_i->head==NULL) ? NULL : cell_i->head);
        cell_i->head = part_i;

        /*
        // print positions and cells (debugging purposes)
        if ((int)(p.elapsedTime/p.dt) % p.freqOut == 0)
        {
            printf("%.2f %.2f %.2f ", part_i->r[0],part_i->r[1],part_i->r[2]);
            printf("%03d\n",index);
        }
	*/
    }
}

/*
 * ---------------------------------------------
 * Calculate the index of cell a particle is in
 * ---------------------------------------------
 */
int getCellIndex(particle *part_i)
{
    int 
        cellNum[3] = {0},
        ind = 0,
        d;

    for (d=0;d<3;d++)
        cellNum[d] = (int)(part_i->r[d]/p.cellDist[d]);

    ind  = cellNum[0];
    ind += cellNum[1]*(int)p.L[0];
    ind += cellNum[2]*(int)p.L[1]*(int)p.L[0];

    return ind;
}
