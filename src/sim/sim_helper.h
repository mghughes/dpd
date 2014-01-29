/*
 * Declarations for misc. simulation helper functions
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 05/10/13, Taylor Dunn
 *    added gammaType and sigmaType functions
 */

#ifndef _SIM_HELPER_H
#define _SIM_HELPER_H

#include "../struct/part_struct.h"

void	pbcDiff(double *diff, int d);
void	pbcPos(particle *part_i, int d);
void	bounceback(void);
int	isInWall(particle *part_i);
int	isInPore(particle *part_i);
double	Atype(particle *part_i, particle *part_j);
double	gammaType(particle *part_i, particle *part_j);
double	sigmaType(particle *part_i, particle *part_j);

#endif /* _SIM_HELPER_H */
