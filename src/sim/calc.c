/*
 * Calculation function definitions
 * (Re, Rg, trans coord, temperature, part position)
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 13/06/13, Matthieu Hughes
 *		calcNseg now prints runtime before exiting
 *		changed trans file name to nseg
 *			nseg prints NmonCis and NmonTrans
 *		writing total trans time is written to log file (not nseg)
 */

#include "calc.h"
#include "../struct/part_struct.h"
#include "../struct/param_struct.h"
#include "../struct/file_struct.h"
#include "sim_helper.h"
#include <math.h>
#include <stdlib.h>

/*
 * -----------------------------------------------------------------
 * Driver for calculating end to end distances and radii of gyration
 * (cis,trans,total)
 * -----------------------------------------------------------------
 */
void calcReRg(void)
{
    double 
        CM[3],              // center of mass of chain
        CMcis[3],           // CM of cis part of chain
        CMtrans[3],         // CM of trans part of chain
        rmon[p.Nmon][3];    // relative positions of monomers

    getRelativePos(rmon, CM, CMcis, CMtrans);
    printReRg(rmon, CM);
    if (p.poreWidth > 0)
        printReRg_cisTrans(rmon, CMcis, CMtrans);

    /* print z positions of monomers - diagnostic for trans
    struct particle *part_i;
    part_i = part;
    printf("%f ",elapsedTime);
    for (i=0;i<par.Nmon;i++)
    {
        printf("%f ",part_i->r[2]);
        part_i++;
    }
    printf("\n");
    */
}

/*
 * ---------------------------------------------------------------------------
 * Finds relative monomer positions and CMs so re and rg calcs don't use PBCs
 * ---------------------------------------------------------------------------
 */
void getRelativePos(double (*rmon)[3], double *CM, 
                    double *CMcis, double *CMtrans)
{
    int i,d;
    double dr[3],r[3];
    particle *part_i,*part_j;

    part_j = part;
    
    /* Head monomer */
    for (d=0;d<3;d++)
        CM[d] = CMcis[d] = r[d] = rmon[0][d] = part_j->r[d];
	
    /* Other monomers */
    part_i = &part[1];
    for (i=1;i<p.Nmon;i++,part_i++,part_j++)
    {
        for (d=0;d<3;d++)
        {
            dr[d] = part_i->r[d]-part_j->r[d];

            pbcDiff(&dr[d],d);

            /* Update relative positions and total chain center of mass */
            r[d] += dr[d];
            rmon[i][d] = r[d];
            CM[d] += r[d];
			
            /* Update center of mass for cis and trans parts */
            if (i < p.NmonCis)
                CMcis[d] += r[d];
            else if (i == p.Nmon-p.NmonTrans) // start of trans section
                CMtrans[d] = r[d];
            else if (i > p.Nmon-p.NmonTrans)
                CMtrans[d] += r[d];
        }
    }

    for (d=0;d<3;d++)
    {
        CM[d] *= p.recNmon;
        CMcis[d] /= p.NmonCis;
        CMtrans[d] /= p.NmonTrans;
    }
}

/* ------------------------------------------------------------------------
 * Calculate and print square radius of gyration (Rg^2) for each component
 * -- e.g. Rg^2_x = (1/N)*sum[(r_i_x - r_cm_x)^2)] --
 * and end-to-end distance for each component (Re)
 * ------------------------------------------------------------------------
 */
void printReRg(double (*rmon)[3], double *CM)
{
    int i,d;
    double rg[3] = {0};

    /* Find rg */
    for (i=0;i<p.Nmon;i++)
    {
        for (d=0;d<3;d++)
            rg[d] += rmon[i][d]-CM[d];
    }

    /* Print re, rg to their respective files */
    fprintf(f.rg,"%f ",p.elapsedTime);
    fprintf(f.re,"%f ",p.elapsedTime);
    for (d=0;d<3;d++)
    {
        fprintf(f.rg,"%f ",rg[d]*rg[d]*p.recNmon);
        fprintf(f.re,"%f ",rmon[p.Nmon-1][d]-rmon[0][d]);
    }
    fprintf(f.rg,"\n");
    fprintf(f.re,"\n");

}

/* -----------------------------------------------------------------
 * Calculate and print Re and Rg for cis and trans parts of polymer
 * -----------------------------------------------------------------
 */
void printReRg_cisTrans(double (*rmon)[3],
                        double *CMcis, double *CMtrans)
{
    int i,d;
    double 
        rg_cis[3] = {0},
        re_cis[3] = {0},
        rg_trans[3] = {0},
        re_trans[3] = {0};

    /* Find rg_cis, rg_trans */
    if (p.NmonCis !=0 && p.NmonCis != 1) // otherwise leave rg[d] at 0
    {
	for (i=0;i<p.NmonCis;i++)
	{
	    for (d=0;d<3;d++)
	        rg_cis[d] += rmon[i][d]-CMcis[d];
	}
    }
    if (p.NmonTrans != 0 && p.NmonTrans != 1)
    {
	for (i=p.Nmon-p.NmonTrans;i<p.Nmon;i++)
	{
	    for (d=0;d<3;d++)
		rg_cis[d] += rmon[i][d]-CMtrans[d];
        }
    }

    /* Find re_cis, re_trans */
    if (p.NmonCis != 0 && p.NmonCis != 1) // otherwise leave re[d] at 0
    {
        for (d=0;d<3;d++)
            re_cis[d] = rmon[p.NmonCis-1][d]-rmon[0][d];
    }
    if (p.NmonTrans != 0 && p.NmonTrans != 1)
    {
        for (d=0;d<3;d++)
            re_trans[d] = rmon[p.Nmon-1][d]-rmon[p.Nmon-p.NmonTrans][d];
    }

    /* Print rg_cis, rg_trans, re_cis, re_trans to respective files */
    fprintf(f.rg_cis,"%f ",p.elapsedTime);
    fprintf(f.rg_trans,"%f ",p.elapsedTime);
    fprintf(f.re_cis,"%f ",p.elapsedTime);
    fprintf(f.re_trans,"%f ",p.elapsedTime);
    for (d=0;d<3;d++)
    {
        fprintf(f.rg_cis,"%f ",rg_cis[d]*rg_cis[d]/p.NmonCis);
        fprintf(f.rg_trans,"%f ",rg_trans[d]*rg_trans[d]/p.NmonTrans);
        fprintf(f.re_cis,"%f ",re_cis[d]);
        fprintf(f.re_trans,"%f ",re_trans[d]);
    }
    fprintf(f.rg_cis,"\n");
    fprintf(f.rg_trans,"\n");
    fprintf(f.re_cis,"\n");
    fprintf(f.re_trans,"\n");
}

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

        exit(0);
    }
    else // Print NmonCis and NmonTrans to nseg file
    {
        fprintf(f.nseg,"%f %d %d\n",p.elapsedTime,p.NmonCis,p.NmonTrans);
        for (i=0;i<p.Nmon;i++)
            p.prevMonPos[i] = p.currentMonPos[i];
        /*printf("mon positions\n");
        for (i=0;i<par.Nmon;i++)
        {
            printf("%f %d %d\n",elapsedTime,i,currentMonPos[i]);
        }
        printInfo('p','m');
        */
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
