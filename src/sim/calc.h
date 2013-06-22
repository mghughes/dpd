/*
 * Calculation function declarations
 * (Re, Rg, trans coord, temperature, part position)
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 04/06/13, Matthieu Hughes
 */

#ifndef _CALC_H
#define _CALC_H

void	calcReRg(void);
void	getRelativePos(double (*rmon)[3], double *CM,
                       double *CMcis, double *CMtrans);
void	printReRg(double (*rmon)[3], double *CM);
void	printReRg_cisTrans(double (*rmon)[3],
                           double *CMcis, double *CMtrans);
void	calcNseg(double ti);
void	averageTemp(void);
void	printInfo(char choice, char particleType);

#endif /* _CALC_H */
