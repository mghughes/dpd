/*
 * Re and Rg calculation function declarations
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 24/06/13, Matthieu Hughes
 *      split up calc into calc_rerg and calc_other
 */

#ifndef _CALC_RERG_H
#define _CALC_RERG_H

void    calcReRg(void);
void    getRelativePos(double (*rmon)[3], double *CM,
                       double *CMcis, double *CMtrans);
void    printReRg(double (*rmon)[3], double *CM);
void    printReRg_cisTrans(double (*rmon)[3],
                           double *CMcis, double *CMtrans);

#endif /* _CALC_RERG_H */
