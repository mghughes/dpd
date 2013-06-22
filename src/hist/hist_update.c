/*
 * Definitions of histogram update (sample) functions
 * (z dens, xy dens, radial distribution function, structure factor)
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 21/06/13, Matthieu Hughes
 *		removed struct and rdf stuff
 */

#include "hist_update.h"
#include "../struct/hist_struct.h"
#include "../struct/param_struct.h"
#include "../struct/part_struct.h"
#include "../sim/sim_helper.h"
#include <math.h>

/*
 * -----------------------------
 * Driver for histogram updates
 * -----------------------------
 */
void updateHists(void)
{
    int i;
    particle *part_i;

    part_i = part;
    for (i=0;i<p.Ntot-p.Nwall;i++,part_i++)
    {
        if (p.ydens)
            updateDens(i,part_i);
        if (p.ydensxy)
            updateDensxy(i,part_i);

    }
}

/*
 * -----------------------------
 * Update density (z) histogram
 * -----------------------------
 */
void updateDens(int i, particle *part_i)
{
    double dbin_dens;
    int bin_dens;

    dbin_dens = part_i->r[2]/h.binWidthDens;
    bin_dens = (int)floor(dbin_dens);
    if (i >= p.Nmon)
        h.dens[bin_dens]++;
}

/*
 * -----------------------------
 * Update xy density histograms
 * -----------------------------
 */
void updateDensxy(int i, particle *part_i)
{
    double dbin_densx,dbin_densy;
    int bin_densx,bin_densy;
    dbin_densx = part_i->r[0]/h.binWidthDensxy;
    dbin_densy = part_i->r[1]/h.binWidthDensxy;
    bin_densx = (int)floor(dbin_densx);
    bin_densy = (int)floor(dbin_densy);

    if (i >= p.Nmon)
    {
        double zpos = part_i->r[2];
        double upperWall = p.L_HALF[2]+p.wallDist;
        double lowerWall = p.L_HALF[2];
        if (zpos > lowerWall-1.0  &&  zpos < upperWall+1.0)
            h.densxy1[bin_densx][bin_densy]++;
        else if (zpos > lowerWall-3.0  &&  zpos < lowerWall-1.0)
            h.densxy2[bin_densx][bin_densy]++;
        else if (zpos > upperWall+1.0  &&  zpos < upperWall+3.0)
            h.densxy2[bin_densx][bin_densy]++;
        else if (zpos > lowerWall-5.0  &&  zpos < lowerWall-3.0)
            h.densxy3[bin_densx][bin_densy]++;
        else if (zpos > upperWall+3.0  &&  zpos < upperWall+5.0)
            h.densxy3[bin_densx][bin_densy]++;
        else
            h.densxy4[bin_densx][bin_densy]++;
    }
}
