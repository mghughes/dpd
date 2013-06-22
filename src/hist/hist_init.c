/*
 * Definitions of histogram initialization functions
 * (z dens, xy dens, radial distribution function, structure factor)
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 21/06/13, Matthieu Hughes
 *		removed rdf and struct stuff
 */

#include "hist_init.h"
#include "../struct/hist_struct.h"
#include "../struct/param_struct.h"
#include "../init/mem.h"
#include <stdlib.h>
#include <math.h>

/*
 * ------------------------------------
 * Driver for histogram initialization
 * ------------------------------------
 */
void initHists(void)
{
    if (p.ydens)
        initDens();
    if (p.ydensxy)
        initDensxy();
}

/*
 * ------------------------------------------------------------
 * Allocate memory and initialize values for density histogram
 * ------------------------------------------------------------
 */
void initDens(void)
{
    int i;

    h.NUMDENS = 3000; // number of bins
    h.maxBinDens = p.L[2];
    h.binWidthDens = h.maxBinDens/h.NUMDENS;

    h.dens = (int*)malloc(h.NUMDENS*sizeof(int));
    if (h.dens==NULL)
        mallocError("dens");

    for (i=0;i<h.NUMDENS;i++)
        h.dens[i] = 0;
}

/*
 * ------------------------------------------------------------------
 * Allocate memory and initialize values for xy density histograms
 * 
 * In four regions (divided by dist to wall):
 *      0-1
 *      1-3
 *      3-5
 *      5-L[2]
 * 
 * TODO maybe only use two regions 0-1 and other - since wall has no
 * effect when r>1
 * ------------------------------------------------------------------
 */
void initDensxy(void)
{
    int i,j;
    h.NUMDENSXY = 500; // number of bins in each direction (x,y)
    h.maxBinDensxy = p.L[0];
    h.binWidthDensxy = h.maxBinDensxy/h.NUMDENSXY;

    h.densxy1 = (int**)malloc(h.NUMDENSXY*sizeof(int*));
    h.densxy2 = (int**)malloc(h.NUMDENSXY*sizeof(int*));
    h.densxy3 = (int**)malloc(h.NUMDENSXY*sizeof(int*));
    h.densxy4 = (int**)malloc(h.NUMDENSXY*sizeof(int*));

    if (h.densxy1==NULL)
        mallocError("densxy1");
    if (h.densxy2==NULL)
        mallocError("densxy2");
    if (h.densxy3==NULL)
        mallocError("densxy3");
    if (h.densxy4==NULL)
        mallocError("densxy4");

    if (h.densxy1==NULL)
        mallocError("densxy1");
    if (h.densxy2==NULL)
        mallocError("densxy2");
    if (h.densxy3==NULL)
        mallocError("densxy3");
    if (h.densxy4==NULL)
        mallocError("densxy4");

    for (i=0;i<h.NUMDENSXY;i++)
    {
        h.densxy1[i] = (int*)malloc(h.NUMDENSXY*sizeof(int));
        h.densxy2[i] = (int*)malloc(h.NUMDENSXY*sizeof(int));
        h.densxy3[i] = (int*)malloc(h.NUMDENSXY*sizeof(int));
        h.densxy4[i] = (int*)malloc(h.NUMDENSXY*sizeof(int));
        if (h.densxy1[i]==NULL)
            mallocError("densxy1[i]");
        if (h.densxy2[i]==NULL)
            mallocError("densxy2[i]");
        if (h.densxy3[i]==NULL)
            mallocError("densxy3[i]");
        if (h.densxy4[i]==NULL)
            mallocError("densxy4[i]");
    }

    for (i=0;i<h.NUMDENSXY;i++)
    {
        for (j=0;j<h.NUMDENSXY;j++)
            h.densxy1[i][j] = h.densxy2[i][j] = h.densxy3[i][j] = h.densxy4[i][j] = 0;
    }
}
