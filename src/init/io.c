/*
 * IO function definitions
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 21/06/13, Matthieu Hughes
 *		removed struct and rdf stuff
 */

#include "../struct/file_struct.h"
#include "io.h"
#include "../struct/param_struct.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * ------------------
 * Open output files
 * ------------------
 */
void openFiles(void)
{
    if ((f.log=fopen("log","w"))==NULL)
        fileOpenError("log");
    if ((f.rg=fopen("rg","w"))==NULL)
        fileOpenError("rg");
    if ((f.re=fopen("re","w"))==NULL)
        fileOpenError("re");
    if ((f.nseg=fopen("nseg","w"))==NULL)
        fileOpenError("nseg");

	if (p.poreWidth > 0)
	{
	    if ((f.re_trans=fopen("re_trans","w"))==NULL)
	    	fileOpenError("re_trans");
	    if ((f.rg_trans=fopen("rg_trans","w"))==NULL)
	    	fileOpenError("rg_trans");
	    if ((f.re_cis=fopen("re_cis","w"))==NULL)
	    	fileOpenError("re_cis");
	    if ((f.rg_cis=fopen("rg_cis","w"))==NULL)
	    	fileOpenError("rg_cis");
	}

    if (p.ydens)
    {
        if ((f.dens=fopen("dens","w"))==NULL)
            fileOpenError("dens");
    }
    if (p.ydensxy)
    {
        if ((f.densxy1=fopen("densxy1","w"))==NULL)
            fileOpenError("densxy1");
        if ((f.densxy2=fopen("densxy2","w"))==NULL)
            fileOpenError("densxy2");
        if ((f.densxy3=fopen("densxy3","w"))==NULL)
            fileOpenError("densxy3");
        if ((f.densxy4=fopen("densxy4","w"))==NULL)
            fileOpenError("densxy4");
    }
    if (p.yaveTemp)
    {
        if ((f.tempAve=fopen("tempAve","w"))==NULL)
            fileOpenError("tempAve");
    }
}

/*
 * ---------------------------------------------------------------
 * Print error message and exit program if a file can't be opened
 * ---------------------------------------------------------------
 */
void fileOpenError(char *filename)
{
    fprintf(stderr,"Fatal error: cannot open file %s\n",filename);
    exit(1);
}

/*
 * -------------------
 * Close output files
 * -------------------
 */
void closeFiles(void)
{
    fclose(f.log);
    fclose(f.rg);
    fclose(f.re);
    fclose(f.nseg);

    if (p.poreWidth > 0)
    {
        fclose(f.re_cis);
	fclose(f.rg_cis);
        fclose(f.re_trans);
        fclose(f.rg_trans);
    }
    if (p.ydens)
        fclose(f.dens);
    if (p.ydensxy)
    {
        fclose(f.densxy1);
        fclose(f.densxy2);
        fclose(f.densxy3);
        fclose(f.densxy4);
    }
    if (p.yaveTemp)
    {
        fprintf(f.tempAve,"\n");
        fclose(f.tempAve);
    }
}

/*
 * -------------------
 * Flush output files
 * -------------------
 */
void flushFiles(void)
{
    fflush(f.log);
    fflush(f.rg);
    fflush(f.re);
    fflush(f.nseg);

    if (p.poreWidth > 0)
    {
        fflush(f.re_trans);
        fflush(f.rg_trans);
        fflush(f.re_cis);
        fflush(f.rg_cis);
    }
    if (p.ydens)
        fflush(f.dens);
    if (p.ydensxy)
    {
        fflush(f.densxy1);
        fflush(f.densxy2);
        fflush(f.densxy3);
        fflush(f.densxy4);
    }
    if (p.yaveTemp)
        fflush(f.tempAve);
}
