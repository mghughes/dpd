/*
 * Cell list struct definition
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 04/06/13, Matthieu Hughes
 *		removed vecIndex from cellList struct
 */

#ifndef _CELL_STRUCT_H
#define _CELL_STRUCT_H

#include "part_struct.h"

struct cellList
{
    int	                index;
    struct particle     *head;             
    struct cellList     *neighbour[27];
};

typedef struct cellList cellList;
extern cellList *cell;

#endif /* _CELL_STRUCT_H */

