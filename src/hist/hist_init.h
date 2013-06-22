/*
 * Declarations of histogram initialization functions
 * (z dens, xy dens, radial distribution function, structure factor)
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 21/06/13, Matthieu Hughes
 *		removed struct and rdf stuff
 */

#ifndef _HIST_INIT_H
#define _HIST_INIT_H

void	initHists(void);
void	initDens(void);
void	initDensxy(void);

#endif /* _HIST_INIT_H */
