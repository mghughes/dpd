/*
 * Definitions of histogram printing functions
 * (z dens, xy dens, radial distribution function, structure factor)
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 21/06/13, Matthieu Hughes
 *		removed struct and rdf stuff
 */

#include "hist_print.h"
#include "../struct/hist_struct.h"
#include "../struct/param_struct.h"
#include "../struct/file_struct.h"
#include <math.h>

/*
 * ----------------------------------------
 * Driver for histogram printing functions
 * ----------------------------------------
 */
void printHists(void)
{
    if (p.ydens)
        printDens();
    if (p.ydensxy)
        printDensxy();
}

/*
 * ------------------------------------------
 * Normalize and print density (z) histogram
 * ------------------------------------------
 */
void printDens(void)
{
    int i;

    rewind(f.dens);
    for (i=0;i<h.NUMDENS;i++)
    {
        fprintf(f.dens,"%f %f\n",((double)i+0.5)*h.binWidthDens,(double)h.dens[i]/(p.nsamp*p.L[0]*p.L[1]*h.binWidthDens));
    }
}

/* 
 * ----------------------------------------------
 * Normalize and print xy density 2-D histograms
 * ----------------------------------------------
 */
void printDensxy(void)
{
    int i,j;
    rewind(f.densxy1);
    rewind(f.densxy2);
    rewind(f.densxy3);
    rewind(f.densxy4);
    double norm1 = 1.0/(2.0*p.nsamp*h.binWidthDensxy*h.binWidthDensxy);
    double norm2 = 1.0/(4.0*p.nsamp*h.binWidthDensxy*h.binWidthDensxy);
    double norm3 = norm2;
    double norm4 = 1.0/(((double)p.L[2]-(p.L_HALF[2]+p.wallDist+5.0)+2.5)*p.nsamp*h.binWidthDensxy*h.binWidthDensxy);
    for (i=0;i<h.NUMDENSXY;i++)
    {
        for (j=0;j<h.NUMDENSXY;j++)
        {
            fprintf(f.densxy1,"%f %f ",((double)i+0.5)*h.binWidthDensxy,((double)j+0.5)*h.binWidthDensxy);
            fprintf(f.densxy2,"%f %f ",((double)i+0.5)*h.binWidthDensxy,((double)j+0.5)*h.binWidthDensxy);
            fprintf(f.densxy3,"%f %f ",((double)i+0.5)*h.binWidthDensxy,((double)j+0.5)*h.binWidthDensxy);
            fprintf(f.densxy4,"%f %f ",((double)i+0.5)*h.binWidthDensxy,((double)j+0.5)*h.binWidthDensxy);

            fprintf(f.densxy1,"%f\n",(double)h.densxy1[i][j]*norm1);
            fprintf(f.densxy2,"%f\n",(double)h.densxy2[i][j]*norm2);
            fprintf(f.densxy3,"%f\n",(double)h.densxy3[i][j]*norm3);
            fprintf(f.densxy4,"%f\n",(double)h.densxy4[i][j]*norm4);
        }
    }
}
