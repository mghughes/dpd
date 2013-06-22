/*
 * Definitions of particle initialization functions
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 21/06/13, Matthieu Hughes
 *		cleaned up dead code
 */

#include "../struct/part_struct.h"
#include "part_init.h"
#include "../struct/param_struct.h"
#include "../sim/sim_helper.h"
#include <stdlib.h>

/*
 * ------------------------------------------
 * Initialize fluid positions and velocities
 * ------------------------------------------
 */
void initFluid(void)
{
    int i,d;
    double vMax = 1.0;
    double vel;
    struct particle *part_i,*part_ii;

    /* Assign random initial positions */
    part_i = &part[p.Nmon];
    for (i=p.Nmon;i<p.Ntot-p.Nwall;i++,part_i++)
    {

        for (d=0;d<3;d++)
        {
            part_i->r[d] = p.L[d]*((double)random()/(double)0x7fffffff);
        }

	/* Reposition if particle is in wall */
        if (p.wallLayers>0)
        {
            while (isInWall(part_i))
            {
                part_i->r[2] = p.L[2]*((double)random()/(double)0x7fffffff);
            }
        }
    }

    /* Assign random initial velocities on [-vMax,vMax] */
    part_i = &part[p.Nmon];
    for (i=p.Nmon;i<p.Ntot-p.Nwall;i++,part_i++)
    {
        if (0==i%2) // every second particle
        {
            for (d=0;d<3;d++)
            {
                vel = vMax*(2.0*((double)random()/(double)0x7fffffff)-1.0);

                /* Two particles get equal and opposite velocity
                 * (for momentum conservation)
                 */
                part_i->v[d] = vel;
                if ((i+1)>=p.Ntot) // If i is the last particle, give it 0 velocity
                    part_i->v[d] = 0.0;
                else
                {
                    part_ii = &part[i+1];
                    part_ii->v[d] = -vel;
                }
            }

        }
    }
}

/*
 * --------------------------------------------------------------------------
 * Initialize wall particles on square lattice.
 *
 * Use this function when no pore is present, otherwise use initWallWithPore
 *
 * 1st wall is placed at z=L_HALF[2] and spans xy plane 
 * -- other layers build up in z direction --
 * --------------------------------------------------------------------------
 */
void initWall(void)
{
    int i,j,k;
    struct particle *part_i;

    part_i = &part[p.Ntot-p.Nwall];
    for (k=0;k<p.wallLayers;k++) // loop through layers (z direction)
    {
        for (j=0;j<p.Nwall_y;j++) // loop y direction
        {
            for (i=0;i<p.Nwall_x;i++,part_i++) // loop x direction
            {
                part_i->r[0] = i*p.wallDist;
                part_i->r[1] = j*p.wallDist;
                part_i->r[2] = p.L_HALF[2] + k*p.wallDist;
            }
        }
    }
}

/*
 * ---------------------------------------------------------------
 * Initialize wall particles on square lattice
 * but leave hole for a (square) nanopore of a specified size.
 * 
 * Use this function when pore is present, otherwise use initWall
 * ---------------------------------------------------------------
 */
void initWallWithPore(void)
{
    /* Take out wall particles to make a pore */
    int numPoreParts_1D = (int)(p.poreWidth/p.wallDist + 0.5) - 1;
    p.Ntot -= p.wallLayers*numPoreParts_1D*numPoreParts_1D;
    p.Nwall -= p.wallLayers*numPoreParts_1D*numPoreParts_1D;
    double xWallDist = (p.L[0]-p.poreWidth+p.wallDist)/(p.Nwall_x-numPoreParts_1D);
    double yWallDist = (p.L[1]-p.poreWidth+p.wallDist)/(p.Nwall_y-numPoreParts_1D);

    /* calculate pore boundaries */
    p.xPoreBoundary[0] = p.L_HALF[0] - 0.5*p.poreWidth;
    p.xPoreBoundary[1] = p.L_HALF[0] + 0.5*p.poreWidth;
    p.yPoreBoundary[0] = p.L_HALF[1] - 0.5*p.poreWidth;
    p.yPoreBoundary[1] = p.L_HALF[1] + 0.5*p.poreWidth;
    p.zPoreBoundary[0] = p.L_HALF[2];
    p.zPoreBoundary[1] = p.L_HALF[2] + (p.wallLayers-1)*p.wallDist;
    double xPoreDist = (p.xPoreBoundary[1]-p.xPoreBoundary[0])/(numPoreParts_1D+1);
    double yPoreDist = (p.yPoreBoundary[1]-p.yPoreBoundary[0])/(numPoreParts_1D+1);

    /* Place particles on lattice */
    int i,j,k,d;
    struct particle *part_i;

    part_i = &part[p.Ntot-p.Nwall];
    double currentPos[3] = {p.xPoreBoundary[1],p.yPoreBoundary[0],p.zPoreBoundary[0]};
    for (k=0;k<p.wallLayers;k++) // loop in z direction
    {
        currentPos[0] = p.xPoreBoundary[1];
        currentPos[1] = p.yPoreBoundary[0];

	/* Place all particles on wall lattice except those in line with 
	 * pore in y direction, as so:
	 * 
	 *  ^ y
         *  |
	 *   -->x
	 *
	 * o  o  o  o     o  o  o  o		     
	 * 			 
	 * o  o  o  o--o--o  o  o  o
	 *	    |	  |
	 *	    |	  |	
	 * o  o  o  o--o--o  o  o  o
	 *
	 * o  o  o  o     o  o  o  o
	 *
         * o  o  o  o     o  o  o  o
	 */
        for (j=0;j<p.Nwall_y;j++) // loop in y direction
        {
            for (i=0;i<p.Nwall_x-numPoreParts_1D;i++,part_i++) // loop in x direction
            {
                for (d=0;d<3;d++)
                    part_i->r[d] = currentPos[d];
                
		currentPos[0] += xWallDist;
                
                if (currentPos[0] >= p.L[0]) // PBC in x direction
                    currentPos[0] -= p.L[0];
            }

            currentPos[0] = p.xPoreBoundary[1];
            currentPos[1] += ((j<=numPoreParts_1D) ? yPoreDist : yWallDist);
			
            if (currentPos[1] >= p.L[1]) // PBC in y direction
                currentPos[1] -= p.L[1];
        } 
		
	currentPos[1] = p.yPoreBoundary[1];
        currentPos[0] = p.xPoreBoundary[0] + xPoreDist;

	/* Place all other particles
	 * i.e. those in line with pore in y direction
	 */
        for (j=0;j<p.Nwall_y-numPoreParts_1D;j++) // loop in y direction
        {
            for (i=0;i<numPoreParts_1D;i++,part_i++) // loop in x direction
            {
                for (d=0;d<3;d++)
                    part_i->r[d] = currentPos[d];
                
                currentPos[0] += xPoreDist;
                
                if (currentPos[0] >= p.L[0]) // x PBC
                    currentPos[0] -= p.L[0];
            } 

            currentPos[0] = p.xPoreBoundary[0] + xPoreDist;
            currentPos[1] += yWallDist;

            if (currentPos[1] >= p.L[1]) // y PBC
                currentPos[1] -= p.L[1];
        } 

        currentPos[2] += p.wallDist;
    } // end loop in z direcion
}

/*
 * -----------------------------------------------------------------------------
 * Initialize monomer positions and velocities.
 * 
 * Use this function when nanopore is not present, otherwise use initPolyInPore
 * -----------------------------------------------------------------------------
 */
void initPoly(void)
{
    int i,d,sign=1;
    double randDisp = 0.05;
    double monDist = 0.4;
    struct particle *part_i,*part_ii;

    /* Place first monomer just below center of box (below wall if present) */
    part_i = part;
    i = part_i->partNum;

    part_i->r[0] = p.L_HALF[0];
    part_i->r[1] = p.L_HALF[1];
    part_i->r[2] = p.L_HALF[2]-0.7;
    for (d=0;d<3;d++)
	part_i->v[d] = 0.0;	

    /* Place other monomers */
    part_i++;
    for (i=1;i<p.Nmon;i++,part_i++)
    {
        /* Give current monomer same position and velocity as previous monomer */
        part_ii=&part[i-1];
        for (d=0;d<3;d++)
        {
            part_i->r[d] = part_ii->r[d];
            part_i->v[d] = part_ii->v[d];
        }

        /* Displace current monomer by monDist in all directions */
        part_i->r[2] -= sign*monDist;
        part_i->r[0] -= monDist;
        part_i->r[1] -= monDist;

        /* Reverse direction of the chain if particle has entered wall */
        if (p.wallLayers>0 && isInWall(part_i))
        {
            part_i->r[2] += 2*sign*monDist;
            sign = -sign;
        }

        // PBCs
        for (d=0;d<3;d++)
            pbcPos(part_i, d);
    }

    /* Randomly displace monomers from lattice by an amount on [-randDisp,randDisp] */
    part_i = part;
    for (i=0;i<p.Nmon;i++,part_i++)
    {
        for (d=0;d<3;d++)
        {
            part_i->r[d] += randDisp*(2.0*((double)random()/(double)0x7fffffff)-1.0);
            pbcPos(part_i, d);
        }
    }
}

/* 
 * ------------------------------------------------------------------------
 * Initialize monomer positions and velocities (polymer placed in nanopore)
 * 
 * Use this function if pore is present, otherwise use initPoly
 * ------------------------------------------------------------------------
 */
void initPolyInPore(void)
{
    int i,d,sign;
    double randDisp = 0.05;
    double monDist = 0.4;
    particle *part_i,*part_j;

    /*
     * Allocate memory for monomer position array (for nseg calculation)
     
     */
    p.prevMonPos = malloc(p.Nmon*sizeof(int));
    p.currentMonPos = malloc(p.Nmon*sizeof(int));

    /* Put middle monomer in center of pore */
    part_j = &part[p.Nmon/2];
    part_j->r[0] = 0.5*(p.xPoreBoundary[0]+p.xPoreBoundary[1]);
    part_j->r[1] = 0.5*(p.yPoreBoundary[0]+p.yPoreBoundary[1]);
    part_j->r[2] = 0.5*(p.zPoreBoundary[0]+p.zPoreBoundary[1]);
    p.prevMonPos[part_j->partNum] = 1;

    /* Place monomers above/below middle one until chain is out of the pore */
    part_i = part_j+1;
    for (i=0;i<p.wallLayers/2;i++,part_i++,part_j++) // above
    {
        part_i->r[0] = part_j->r[0];
        part_i->r[1] = part_j->r[1];
        part_i->r[2] = part_j->r[2] + p.wallDist;

        p.prevMonPos[part_i->partNum] = ((isInPore(part_i)) ? 1 : 2);
    }

    part_j = &part[p.Nmon/2];
    part_i = part_j-1;
    for (i=0;i<p.wallLayers/2;i++,part_i--,part_j--) // below
    {
        part_i->r[0] = part_j->r[0];
        part_i->r[1] = part_j->r[1];
        part_i->r[2] = part_j->r[2] - p.wallDist;

        p.prevMonPos[part_i->partNum] = ((isInPore(part_i)) ? 1 : 0);
    }


    /* Place other monomers in zig-zag (like in initPoly function) */
    
    /* Upper half of the chain */
    part_j = &part[p.Nmon/2+p.wallLayers/2];
    part_i = part_j+1;
    sign = 1;
    for (i=p.Nmon/2+p.wallLayers/2+1;i<p.Nmon;i++,part_i++,part_j++)
    {
        for (d=0;d<3;d++)
            part_i->r[d] = part_j->r[d];

        part_i->r[0] += monDist;
        part_i->r[1] += monDist;
        part_i->r[2] += sign*monDist;

        /* Reverse chain direction if particle is in wall */
        if (isInWall(part_i) && i!=p.Nmon/2+p.wallLayers/2+1)
        {
            part_i->r[2] -= 2*sign*monDist;
            sign = -sign;
        }

        for (d=0;d<3;d++)
            pbcPos(part_i,d);

        p.prevMonPos[i] = 2;
    }

    /* Lower half of the chain */
    part_j = &part[p.Nmon/2-p.wallLayers/2];
    part_i = part_j-1;
    sign = 1;
    for (i=p.Nmon/2-p.wallLayers/2-1;i>=0;i--,part_i--,part_j--)
    {
        for (d=0;d<3;d++)
            part_i->r[d] = part_j->r[d];

        part_i->r[0] -= monDist;
        part_i->r[1] -= monDist;
        part_i->r[2] -= sign*monDist;

        /* Reverse direction if particle has entered wall */
        if (isInWall(part_i) && i!=p.Nmon/2+p.wallLayers/2-1)
        {
            part_i->r[2] += 2*sign*monDist;
            sign = -sign;
        }

        for (d=0;d<3;d++)
            pbcPos(part_i,d);

        p.prevMonPos[i] = 0;
    }

    /* Randomly displace monomers by an amount on [-randDisp,randDisp] */
    part_i = part;
    for (i=0;i<p.Nmon;i++)
    {
        for (d=0;d<3;d++)
        {
            part_i->r[d] += randDisp*(2.0*((double)random()/(double)0x7fffffff)-1.0);
            pbcPos(part_i, d);
        }
        part_i++;
    }
}
