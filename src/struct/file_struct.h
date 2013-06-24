/*
 * Definition of struct holding output files
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 13/06/13, Matthieu Hughes
 *		changed name of trans file to nseg
 */

#ifndef _FILE_STRUCT_H
#define _FILE_STRUCT_H

#include <stdio.h>

struct files
{
    FILE
        *rg,	    // total radius of gyration
        *re,	    // total end-to-end dist
	*re_cis,    // re of monomers that exited on cis side
	*rg_cis,    // rg of  "    "     "      "     "
        *re_trans,  // re of monomers that exited on trans side
        *rg_trans,  // rg "    "         "        "    "
		*nseg,	    // translocation
        *log,	    // log file
        *dens,	    // dens (z)
        *tempAve,   // average temperature
        *densxy1,   // dens (xy)
        *densxy2,   //  "
        *densxy3,   //  "
        *densxy4;   //  "

};

typedef struct files files;
extern files f;

#endif /* _FILE_STRUCT_H */
