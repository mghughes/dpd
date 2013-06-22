/*
 * IO function declarations
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 04/06/13, Matthieu Hughes
 */

#ifndef _IO_H
#define _IO_H

void	openFiles(void);
void	fileOpenError(char *filename);
void	closeFiles(void);
void	flushFiles(void);

#endif /* _IO_H */
