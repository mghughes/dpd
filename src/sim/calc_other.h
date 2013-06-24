/*
 * Calculation function declarations
 * (trans coord, temperature, part position, density)
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 24/06/13, Matthieu Hughes
 */

#ifndef _CALC_H
#define _CALC_H

void	calcNseg(double ti);
void	averageTemp(void);
void	printInfo(char choice, char particleType);
void    updateDens(void);
void    printDens(void);

#endif /* _CALC_H */
