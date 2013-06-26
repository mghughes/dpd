/*
 * Declarations of main simulation loop functions
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 04/06/13, Matthieu Hughes
 */

#ifndef _SIM_UPDATE_H
#define _SIM_UPDATE_H

#include "../struct/part_struct.h"

/* For dissOnly flags */
#define ONLYDISS 1
#define ALLFORCES 0

void	updateSys(void);
void	updatePosVel(int dissOnly);
void	calcForces(int dissOnly);
void	dpdForces(particle *part_i, particle *part_j, 
            int dissOnly);
void	harmForce(void);

#endif /* _SIM_UPDATE_H */
