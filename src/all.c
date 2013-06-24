/*
 *  Includes all modules of dpd simulation program
 *	For link-time optimization on pre-4.6 gcc just compile this file
 *
 *  Copyright (C) Matthieu Hughes, 2013
 *  matthieuhughes.1@gmail.com
 *
 *  last update: 05/06/13, Matthieu Hughes
 */

#include "main.c"

#include "init/cell.c"
#include "init/io.c"
#include "init/mem.c"
#include "init/param_init.c"
#include "init/part_init.c"

#include "sim/calc_other.c"
#include "sim/calc_rerg.c"
#include "sim/sim_helper.c"
#include "sim/sim_update.c"
