/*
 *  Definitions of main simulation loop functions
 *
 *  Copyright (C) Matthieu Hughes, 2013
 *  matthieuhughes.1@gmail.com
 *
 *  last update: 04/06/13, Matthieu Hughes
 */

#include "sim_update.h"
#include "../struct/part_struct.h"
#include "../struct/cell_struct.h"
#include "../struct/param_struct.h"
#include "../init/cell.h"
#include "sim_helper.h"
#include "../struct/file_struct.h"
#include <math.h>
#include <stdlib.h>

/*
 * -------------------------
 * Driver for system update
 * -------------------------
 */
void updateSys(void)
{
    updatePosVel(ALLFORCES);
    calcForces(ALLFORCES);

    /* Correct velocities and fDiss */
    updatePosVel(ONLYDISS);
    calcForces(ONLYDISS);

    /* Bounceback if any particle penetrates wall */
    if (p.wallLayers > 0)
        bounceback();

    constructList();
}

/*
 * ----------------------------------------------------------------------
 * Update position and velocity of all particles
 * according to a modified velocity-verlet algorithm
 *
 * See Matthieu Hughes' undergraduate thesis (UPEI, 2013) and references
 * therein for details of algorithm
 * ----------------------------------------------------------------------
 */
void updatePosVel(int dissOnly)
{
    int i,d;
    particle *part_i;
    part_i = part;

    for (i=0;i<p.Ntot-p.Nwall;i++,part_i++) 
    {
        /* During equilibration stage of translocation,
	 * keep middle monomer fixed in center of pore
	 *
	 * NOTE: this may be causing a segfault because monomers connected to middle
	 * one feel spring force pulling/pushing it away/towards middle
	 * (antisymmetric interparticle force)
	 */
        if (p.poreWidth>0 && i==p.Nmon/2 && p.elapsedTime<=p.eqTime) 
	{
            for (d=0;d<3;d++)
                part_i->fDiss[d] = part_i->fRand[d] = part_i->fCons[d] = part_i->fHarm[d] = part_i->fDrive[d] = 0.0;
            continue;
        }

        for (d=0;d<3;d++) 
	{
            /* Update velocities */
            part_i->v[d] += 0.5*p.dt*(part_i->fHarm[d]+part_i->fDrive[d]+part_i->fCons[d]+part_i->fDiss[d]+part_i->fRand[d]);

            /* Reset dissipative force */
            part_i->fDiss[d] = 0.0;

            if (!dissOnly) 
	    {
                /* Update position */
                part_i->r[d] += part_i->v[d]*p.dt;
                pbcPos(part_i, d);

                /* Reset other forces */
                part_i->fCons[d] = 0.0;
                part_i->fRand[d] = 0.0;
                part_i->fHarm[d] = 0.0;
		part_i->fDrive[d] = 0.0;
            }
        }
    }
}

/*
 * ---------------------------------------------------
 * Loop through particle pairs with linked list
 * and call dpdForce to calculate interparticle force
 *
 * Also calls harmForce to calculate spring forces
 * ---------------------------------------------------
 */
void calcForces(int dissOnly)
{
    int i,j,ci,cj;
    particle *part_i,*part_j;
    cellList *cell_i,*cell_j;

    cell_i = cell;
    for (ci=0;ci<p.Ncell;ci++,cell_i++)
    {
        for (cj=0;cj<27;cj++)
        {
            cell_j = cell_i->neighbour[cj];
            if (ci < cell_j->index) // avoid double-counting
            {
                part_i = cell_i->head;
                while (part_i != NULL)
                {
                    i = part_i->partNum;
                    part_j = cell_j->head;
                    while (part_j != NULL)
                    {
                        j = part_j->partNum;

                        if (i<p.Ntot-p.Nwall || j<p.Ntot-p.Nwall)
                            dpdForces(part_i,part_j,dissOnly);

                        part_j = part_j->next;
                    }
                    part_i = part_i->next;
                }
            }
	    else if (ci == cell_j->index) // neighbour==current
            {
                part_i = cell_i->head;
                while (part_i != NULL)
                {
                    i = part_i->partNum;
                    part_j = cell_j->head;
                    while (part_j != NULL)
                    {
                        j = part_j->partNum;
                        if (i<j)
                        {
                            if (i<p.Ntot-p.Nwall || j<p.Ntot-p.Nwall)
                                dpdForces(part_i,part_j,dissOnly);
                        }
                        part_j = part_j->next;
                    }
                    part_i = part_i->next;
                }
            }
        } // end neighbour loop
    } // end main loop

    if (!dissOnly && p.Nmon!=0)
        harmForce();
}

/* 
 * ------------------------------------------------------------------
 * Calculates dpd pairwise forces (conservative, dissipative, random)
 * for two particles
 * ------------------------------------------------------------------
 */
void dpdForces(particle *part_i, particle *part_j, 
               int dissOnly)
{
    double dr[3],dv[3];
    double fRand,fCons,fDiss;
    double recr,r,r2,oneMinr,rdotv,randNum;
    int i,j,d;
    r2 = rdotv = 0.0;

    i = part_i->partNum;
    j = part_j->partNum;

    /* Find interparticle distance */
    for (d=0;d<3;d++)
    {
        dr[d] = part_i->r[d]-part_j->r[d];
        pbcDiff(&dr[d],d);
        dv[d] = part_i->v[d]-part_j->v[d];
        r2 += dr[d]*dr[d];
    }

    if (r2>1.0)
        return;

    r = sqrt(r2);
    recr = 1.0/r;
    oneMinr = 1.0-r;

    /* Find dissipative force */
    for (d=0;d<3;d++)
        rdotv += dr[d]*dv[d];
    rdotv *= recr;
    fDiss = -p.GAMMA*oneMinr*oneMinr*rdotv;

    if (dissOnly)
    {
        /* Update fDiss for the particles and return */
        for (d=0;d<3;d++)
        {
            if (i<p.Ntot-p.Nwall)
                part_i->fDiss[d] += fDiss*dr[d]*recr;
            if (j<p.Ntot-p.Nwall)
                part_j->fDiss[d] -= fDiss*dr[d]*recr;
        }
        return;
    }

    /* Find random force */
    randNum = p.root3*(2.0*((double)random()/(double)0x7fffffff)-1.0);
    fRand = p.SIGMA*oneMinr*randNum*p.recRootdt;

    /* Find conservative force */
    fCons = Atype(i,j)*oneMinr;

    /* Update the three forces for the particles */
    for (d=0;d<3;d++)
    {
        if (i<p.Ntot-p.Nwall)
        {
            part_i->fCons[d] += fCons*dr[d]*recr;
            part_i->fRand[d] += fRand*dr[d]*recr;
            part_i->fDiss[d] += fDiss*dr[d]*recr;
        }
        if (j<p.Ntot-p.Nwall)
        {
            part_j->fCons[d] -= fCons*dr[d]*recr;
            part_j->fRand[d] -= fRand*dr[d]*recr;
            part_j->fDiss[d] -= fDiss*dr[d]*recr;
        }
    }
}

/*
 * ---------------------------------------------------------------------------
 * Calculate harmonic (spring) force for adjacent monomers
 * 
 * Uses FENE potential - See Matthieu Hughes' undergrad thesis (UPEI 2013) 
 * and references therein for details
 *
 * Also adds driving force to any monomer in the pore
 *
 * If polymer if tethered (wallLayers>0 && poreWidth<=0), first monomer feels
 * spring force with wall as well as with second monomer
 * ---------------------------------------------------------------------------
 */
void harmForce(void)
{
    particle *part_i,*part_j;
    double 
        dr[3],
        denom,
        r,
        r2=0.0,
        recr,
        diff1,
        fHarm;
    int i,d;

    /* If tethered polymer, start loop below at 0 not 1 
     * to include spring force with wall
     */
    part_j = part;
    if (p.wallLayers>0 && p.poreWidth<=0) 
    {
        part_i = part;
        i = 0;
    }
    else
    {
        part_i = &part[1];
        i = 1;

	/* Driving force for 1st monomer */
        if (p.poreWidth > 0 && p.fDrive > 0)
	{
	    if (isInPore(part_j))
                part_j->fDrive[2] = p.fDrive;
	}
    }

    for (;i<p.Nmon;i++,part_i++)
    {
	/* Driving force for other monomer (not #0) */
	if (p.poreWidth > 0 && p.fDrive > 0)
	{
	    if (isInPore(part_i))
		part_i->fDrive[2] = p.fDrive;
	}

	/* Find interparticle distance */
        r2 = 0.0;

        for (d=0;d<3;d++)
        {
            if (i == 0)
                dr[d] = part_i->r[d] - p.L_HALF[d];
            else
                dr[d] = part_i->r[d] - part_j->r[d];

            pbcDiff(&dr[d],d);
            r2 += dr[d]*dr[d];
        }
        r = sqrt(r2);

	/* Print to log file if monomers get too far apart
	 * 
	 * This has been the cause of some segfaults 
	 */
	if (r >= p.RMAXHARM)
        {
            fprintf(f.log,"max harm:\tpart %d part %d\t%f\n",i,i-1,p.elapsedTime);
            fHarm = -p.MAXFORCEHARM;
            recr = 1.0/r;
        }
        else
        {
            recr = 1.0/r;
            diff1 = r-p.REQ;
            denom = 1.0-(diff1*diff1)*p.recdiff2sq;
            fHarm = -p.K*diff1/denom;
        }

	/* Update harmonic force for the particles */
        for (d=0;d<3;d++)
        {
            part_i->fHarm[d] += fHarm*dr[d]*recr;
            if (i != 0)
                part_j->fHarm[d] -= fHarm*dr[d]*recr;
        }

        if (i != 0)
            part_j++;
    }
}
