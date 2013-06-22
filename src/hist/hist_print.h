/*
 * Declarations of histogram printing functions
 * (z dens, xy dens, radial distribution function, structure factor)
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 21/06/13, Matthieu Hughes
 *		removed struct and rdf stuff
 */

#ifndef _HIST_PRINT_H
#define _HIST_PRINT_H

#define PI 3.14159265

void	printHists(void);
void	printDens(void);
void	printDensxy(void);

#endif /* _HIST_PRINT_H */
