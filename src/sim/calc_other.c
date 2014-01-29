/*
 * Calculation function definitions
 * (trans coord, temperature, part position, density)
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 20/11/13, Taylor Dunn
 *      added code to find number of solvent particles in pore at end of translocation
 *
 */

#include "calc_other.h"
#include "../struct/part_struct.h"
#include "../struct/param_struct.h"
#include "../struct/file_struct.h"
#include "sim_helper.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * --------------------------------------------------------
 * Finds number of monomers that are cis, trans, and inPore
 * and prints NmonCis, NmonTrans.
 * 
 * Exits program when translocation is complete
 * --------------------------------------------------------
 */
void calcNseg(double ti)
{
    int i;
    p.NmonTrans=0, p.NmonCis=0, p.NmonPore=0;
    particle *part_i;

    part_i = part;
    for (i=0;i<p.Nmon;i++,part_i++)
    {
        /* find out where monomers are/which have translocated */
        if (isInPore(part_i))
        {
            p.currentMonPos[i] = 1;
            p.NmonPore++;
        }
        else // not in pore
        {

            if (p.prevMonPos[i]==0) // mon was cis so it stays cis
            {
                p.currentMonPos[i] = 0;
		p.NmonCis++;
            }
            else if (p.prevMonPos[i]==2) // mon was trans so it stays trans
            {
                p.currentMonPos[i] = 2;
                p.NmonTrans++;
            }
            else // was in pore but isn't anymore
            {
                if (isInWall(part_i))
                {
                    p.currentMonPos[i] = 1;
                    p.NmonPore++;
                }
                else if (part_i->r[2] < p.L_HALF[2]) // is now cis
                {
                    p.currentMonPos[i] = 0;
                    p.NmonCis++;
                }
                else // is now trans
                {
                    p.currentMonPos[i] = 2;
                    p.NmonTrans++;
                }
            }
        }
    }

    /* Exit if polymer has translocated completely */
    if (p.NmonTrans==p.Nmon || p.NmonCis==p.Nmon)
    {
	fprintf(f.log,"\n%s\ttransType\n",((p.NmonCis==p.Nmon)?"cis":"trans"));
        fprintf(f.log,"%.2f\ttransTime\n",p.elapsedTime-p.eqTime);
			
        double tf = (double)clock()/CLOCKS_PER_SEC;
        fprintf(f.log,"%.2f\truntime (s)\n",tf-ti);

        // At the end of translcoation, find number of solvent particles in the pore
        int count = 0;
        for (i=p.Nmon;i<p.Ntot-p.Nwall;i++,part_i++) {
          if (isInPore(part_i)) count++;
        }
        fprintf(f.log,"%d\tsolvent particles in the pore after simulation\n",count);

        exit(0);
    }
    else // Print NmonCis and NmonTrans to nseg file
    {
        fprintf(f.nseg,"%.2f %d %d\n",p.elapsedTime,p.NmonCis,p.NmonTrans);
        for (i=0;i<p.Nmon;i++)
            p.prevMonPos[i] = p.currentMonPos[i];
    }
}

/*
 * ----------------------------------------------------------
 * Calculates average kinetic temperature of fluid particles 
 * -- kT=mean-square velocity/3 of fluid particles --
 * ----------------------------------------------------------
 */
void averageTemp(void)
{
    double ave = 0.0;
    int i,d;
    particle *part_i;
    part_i = &part[p.Nmon];
    for (i=p.Nmon;i<p.Ntot-p.Nwall;i++)
    {
        for (d=0;d<3;d++)
            ave += part_i->v[d]*part_i->v[d];
        part_i++;
    }
    ave /= ((double)(p.Ntot-p.Nwall-p.Nmon)*(double)3);
    fprintf(f.tempAve,"%f ",ave);
}

/*
 * -----------------------------------------------------------------
 * Prints velocities or positions of particles
 * 
 * Choices - v for vel, p for pos
 * Particle types - m for monomer, f for fluid
 *
 * TODO rewrite this to be more consistent with other IO functions:
 *      ie. don't create new file every time function is called
 * -----------------------------------------------------------------
 */
void printInfo(char choice, char particleType)
{
    FILE *fpOut; // file array for putting data
    char filename[15];
    int startLoop,endLoop;
    if (particleType == 'f')
    {
        startLoop = p.Nmon;
        endLoop = p.Ntot;
    }
    else
    {
        startLoop = 0;
        endLoop = p.Nmon;
    }

    /* create files for  outputs */
    int i,d;
    particle *part_i;
    part_i = &part[startLoop];
	for (i=startLoop;i<endLoop;i++)
    {
        if (choice=='v')
            sprintf(filename,"vel%02d.mon",i);
        else if (choice=='p')
            sprintf(filename,"pos%02d.mon",i);
        fpOut = fopen(filename,"a+");
        fprintf(fpOut,"%f\t",p.elapsedTime);
        for (d=0;d<3;d++)
        {
            if (choice=='v')
                fprintf(fpOut,"%f\t",part_i->v[d]);
            else if (choice=='p')
                fprintf(fpOut,"%f\t",part_i->r[d]);
        }
        fprintf(fpOut,"\n");
        fclose(fpOut);
        part_i++;
    }
}

/*
 * -----------------------------
 * Update density (z) histogram
 * -----------------------------
 */
void updateDens(void)
{
    double dbin_dens;
    int i, bin_dens;
    particle *part_i;

    part_i = part;
    for (i=p.Nmon;i<p.Ntot-p.Nwall;i++,part_i++)
    {
        dbin_dens = part_i->r[2]/p.binWidthDens;
        bin_dens = (int)floor(dbin_dens);
            
        p.dens[bin_dens]++;
    }
}

/*
 * ------------------------------------------
 * Normalize and print density (z) histogram
 * ------------------------------------------
 */
void printDens(void)
{
    int i;
    double z,density;

    rewind(f.dens);
    for (i=0;i<p.NbinDens;i++)
    {
        z = (double)(i+0.5)*p.binWidthDens;
        density = (double)p.dens[i]/(p.nsamp+p.L[0]*p.L[1]*p.binWidthDens);
        fprintf(f.dens,"%f %f\n",z,density);
    }
}
