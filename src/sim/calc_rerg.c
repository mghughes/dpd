/*
 * Re and Rg calculation function definitions
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 24/06/13, Matthieu Hughes
 *      split calc.c into calc_rerg.c and calc_other.c
 *
 */


#include "calc_rerg.h"
#include "../struct/part_struct.h"
#include "../struct/param_struct.h"
#include "../struct/file_struct.h"
#include "sim_helper.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

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
    double 
        rg[3] = {0},
        re[3] = {0};

    /* Find rg */
    for (i=0;i<p.Nmon;i++)
    {
        for (d=0;d<3;d++)
            rg[d] += (rmon[i][d]-CM[d]) * (rmon[i][d]-CM[d]);
    }
    
    for (d=0;d<3;d++)
    {
        rg[d] *= p.recNmon;
        re[d] = rmon[p.Nmon-1][d]-rmon[0][d];
    }

    /* Print re, rg to their respective files */
    fprintf(f.rg,"%.2f %f %f %f\n",p.elapsedTime,rg[0],rg[1],rg[2]);
    fprintf(f.re,"%.2f %f %f %f\n",p.elapsedTime,re[0],re[1],re[2]);
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
                rg_cis[d] += (rmon[i][d]-CMcis[d]) * (rmon[i][d]-CMcis[d]);
        }
    }
    if (p.NmonTrans != 0 && p.NmonTrans != 1)
    {
        for (i=p.Nmon-p.NmonTrans;i<p.Nmon;i++)
        {
            for (d=0;d<3;d++)
                rg_trans[d] += (rmon[i][d]-CMtrans[d]) * (rmon[i][d]-CMtrans[d]);
        }
    }

    /* Find re_cis, re_trans and square rg_* */
    if (p.NmonCis != 0 && p.NmonCis != 1) // otherwise leave re[d] at 0
    {
        for (d=0;d<3;d++)
        {
            rg_cis[d] /= p.NmonCis;
            re_cis[d] = rmon[p.NmonCis-1][d]-rmon[0][d];
        }
    }
    if (p.NmonTrans != 0 && p.NmonTrans != 1)
    {
        for (d=0;d<3;d++)
        {
            rg_trans[d] /= p.NmonTrans;
            re_trans[d] = rmon[p.Nmon-1][d]-rmon[p.Nmon-p.NmonTrans][d];
        }
    }

    /* Print rg_cis, rg_trans, re_cis, re_trans to respective files */
    fprintf(f.rg_cis,"%.2f %f %f %f\n",p.elapsedTime,
            rg_cis[0],rg_cis[1],rg_cis[2]);
    fprintf(f.rg_trans,"%.2f %f %f %f\n",p.elapsedTime,
            rg_trans[0],rg_trans[1],rg_trans[2]);
    fprintf(f.re_cis,"%.2f %f %f %f\n",p.elapsedTime,
            re_cis[0],re_cis[1],re_cis[2]);
    fprintf(f.re_trans,"%.2f %f %f %f\n",p.elapsedTime,
            re_trans[0],re_trans[1],re_trans[2]);
    
} 
