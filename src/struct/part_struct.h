/*
 * Definition of struct holding particle information
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 04/06/13, Matthieu Hughes
 */

#ifndef _PART_STRUCT_H
#define _PART_STRUCT_H

struct particle
{
    double  
        r[3],	    // positions {x,y,z}
        v[3],       // velocity
        fCons[3],   // conservative force
        fDiss[3],   // dissipative force
        fRand[3],   // random force
        fHarm[3],   // harmonic (spring) force
        fDrive[3];  // driving force (pore)

    int partNum;	    // particle number
    struct particle *next;  // next part in cell list

};

typedef struct particle particle;
extern particle *part;

#endif /* _PART_STRUCT_H */
