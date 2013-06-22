/*
 * Cell list function declarations
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 04/06/13, Matthieu Hughes
 */


#ifndef _CELL_H
#define _CELL_H

#include "../struct/part_struct.h"

void	initCells(void);
void	constructList(void);
int     getCellIndex(particle *part_i);

#endif /* CELL_H */
