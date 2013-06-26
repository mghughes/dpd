/*
 * Definitions for misc. simulation helper functions
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 04/06/13, Matthieu Hughes
 */


#include "sim_helper.h"
#include "../struct/part_struct.h"
#include "../struct/param_struct.h"

/*
 * -------------------------------------------------------------------------
 * Periodic boundary condition (PBC) calculation for interparticle distances
 * -------------------------------------------------------------------------
 */
void pbcDiff(double *diff,int d)
{
    if ((*diff) > p.L_HALF[d])
        (*diff) -= p.L[d];
    else if ((*diff) < -p.L_HALF[d])
        (*diff) += p.L[d];
}

/*
 * -------------------------------------
 * PBC calculation for particle position
 * -------------------------------------
 */
void pbcPos(particle *part_i, int d)
{
    if (part_i->r[d] >= p.L[d])
        part_i->r[d] -= p.L[d];
    else if (part_i->r[d] < 0)
        part_i->r[d] += p.L[d];
}

/*
 * ------------------------------------------------
 * Bounce particles back if they penetrate the wall
 * ------------------------------------------------
 */
void bounceback(void)
{
    particle *part_i;
    part_i = part;
    int i,d;
    for (i=0;i<p.Ntot-p.Nwall;i++,part_i++)
    {
        if (isInWall(part_i))
        {
            /* Don't bounce back if it's in the pore */
            if (p.poreWidth<=0 || !isInPore(part_i))
            {
                for (d=0;d<3;d++)
                    part_i->v[d] = -part_i->v[d];
            }
        }
    }
}

/*
 * --------------------------------------------
 * Check if a given particle is in wall region
 * 
 * Returns 1 (true) if it is
 * --------------------------------------------
 */
int isInWall(particle *part_i)
{
    int inWall = 0;

    if (part_i->r[2] > p.L_HALF[2] 
     && part_i->r[2] < p.L_HALF[2]+(p.wallLayers-1)*p.wallDist)
    {
        inWall = 1;
    }

    return inWall;
}

/*
 * ------------------------------------------------
 * Check if a given particle is in the pore region
 *
 * Returns 1 (true) if it is
 * ------------------------------------------------
 */
int isInPore(particle *part_i)
{
    int inPore = 0;

    if (part_i->r[0] > p.xPoreBoundary[0] 
     && part_i->r[0] < p.xPoreBoundary[1])
    {
        if (part_i->r[1] > p.yPoreBoundary[0] 
         && part_i->r[1] < p.yPoreBoundary[1])
        {
            if (part_i->r[2] > p.zPoreBoundary[0] 
             && part_i->r[2] < p.zPoreBoundary[1])
            {
                inPore = 1;
            }
        }
    }

    return inPore;
}

/*
 * ---------------------------------------------------------------
 * Determine which value of A (conservative force strength) to use
 * -- mon-mon,fluid-fluid, mon-fluid, wall
 * ---------------------------------------------------------------
 */
double Atype(int i, int j)
{
    double A;

    if (j>=p.Ntot-p.Nwall || i>=p.Ntot-p.Nwall)
        A = p.Awall;
    else if (j<p.Nmon && i<p.Nmon)
        A = p.Amm;
    else if (j<p.Nmon || i<p.Nmon)
        A = p.Amf;
    else
        A = p.Aff;

    return A;
}
