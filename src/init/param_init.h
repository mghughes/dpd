/*
 * Declarations for parameter initialization functions
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 04/06/13, Matthieu Hughes
 */

#ifndef _PARAM_INIT_H
#define _PARAM_INIT_H

#define EPSILON 0.00001     // for double equality comparisons

void	getParams(void);
void	assertParams(int count);
void	calcParams(void);
void	printParams(void);

#endif /* _PARAM_INIT_H */
