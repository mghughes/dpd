/*
 * Declarations of memory management functions
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 04/06/13, Matthieu Hughes
 */

#ifndef _MEM_H
#define _MEM_H

void    allocateMemory(void);
void	mallocError(char *str);
void	freeMemory(void);

#endif /* _MEM_H */

