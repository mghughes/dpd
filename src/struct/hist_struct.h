/*
 *  Definition of struct holding histogram parameters
 *  (z dens, xy dens, radial distribution function, structure factor)
 *
 *  Copyright (C) Matthieu Hughes, 2013
 *  matthieuhughes.1@gmail.com
 *
 *  last update: 04/06/13, Matthieu Hughes
 */

#ifndef _HIST_STRUCT_H
#define _HIST_STRUCT_H

struct histParams
{
    int
        NUMDENS,
        *dens,
        NUMDENSXY,
        **densxy1,
        **densxy2,
        **densxy3,
        **densxy4;

    double 
        maxBinDens,
        binWidthDens,
        maxBinDensxy,
        binWidthDensxy;

};

typedef struct histParams histParams;
extern histParams h;

#endif /* _HIST_STRUCT_H */
