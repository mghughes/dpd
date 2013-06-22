/*
 * Declarations of histogram update (sample) functions
 * (z dens, xy dens, radial distribution function, structure factor)
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 21/06/13, Matthieu Hughes
 *		removed struct and rdf stuff
 */

#ifndef _HIST_UPDATE_H
#define _HIST_UPDATE_H

#include "../struct/part_struct.h"

void	updateHists(void);
void	updateDens(int i, particle *part_i);
void	updateDensxy(int i, particle *part_i);

#endif /* _HIST_UPDATE_H */
