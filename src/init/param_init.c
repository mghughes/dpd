/*
 * Definitions for parameter initialization functions
 *
 * Copyright (C) Matthieu Hughes, 2013
 * matthieuhughes.1@gmail.com
 *
 * last update: 21/06/13, Matthieu Hughes
 *		removed struct and rdf stuff
 */

#include "param_init.h"
#include "../struct/param_struct.h"
#include "../struct/file_struct.h"
#include "io.h"
#include <stdio.h>
#include <assert.h>		// for assertParams function
#include <sys/types.h>	// for getpid function
#include <unistd.h>		//		"
#include <math.h>

/*
 * --------------------------------------------------
 * Read parameters from input file
 *
 * See param_struct.h for descriptions of parameters
 * --------------------------------------------------
 */
void getParams(void)
{
    // counter for number of parameters read
    int paramCount = 0;

    FILE *fpIn;
    if ((fpIn=fopen("dpd.inp","r"))==NULL)
        fileOpenError("dpd.inp");

    // force strengths
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.Aff);
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.Amm);
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.Amf);
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.SIGMA);
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.GAMMA);
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.K);
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.fDrive);

    // particle info
    paramCount += fscanf(fpIn,"%d\t%*s\n",&p.Nmon);
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.density);
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.L[0]);
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.L[1]);
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.L[2]);

    // wall info
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.wallDensity);
    paramCount += fscanf(fpIn,"%d\t%*s\n",&p.wallLayers);
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.poreWidth);

    // run info
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.dt);
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.tMax);
    paramCount += fscanf(fpIn,"%lf\t%*s\n",&p.eqTime);
    paramCount += fscanf(fpIn,"%d\t%*s\n",&p.freqSamp);
    paramCount += fscanf(fpIn,"%d\t%*s\n",&p.freqOut);

    // what to print
    paramCount += fscanf(fpIn,"%d\t%*s\n",&p.ydens);
    paramCount += fscanf(fpIn,"%d\t%*s\n",&p.ydensxy);
    paramCount += fscanf(fpIn,"%d\t%*s\n",&p.yaveTemp);
    paramCount += fscanf(fpIn,"%d\t%*s\n",&p.ymonPos);

    fclose(fpIn);

    assertParams(paramCount);
}

/*
 * -----------------------------------------
 * Ensure input parameters values are valid
 * -----------------------------------------
 */
void assertParams(int count)
{
    int numParams = 24; // *** CHANGE THIS IF YOU ADD/REMOVE INPUT PARAMS ***

    assert(count==numParams);
    assert(p.Aff >= 0);
    assert(p.Amf >= 0);
    assert(p.Amm >= 0);
    assert(p.SIGMA >= 0);
    assert(p.GAMMA >= 0);
    assert(p.K >= 0);
    assert(p.fDrive >= 0);

    assert(p.Nmon >= 0);
    assert(p.density >= 0);
    assert(p.L[0] >= 0);
    assert(p.L[1] >= 0);
    assert(p.L[2] >= 0);

    assert(p.wallDensity >= 0);

    assert(p.dt >= 0);
    assert(p.tMax >= 0);
    assert(p.eqTime >= 0);
    assert(p.freqSamp >= 0);
    assert(p.freqOut >= p.freqSamp);

    assert(p.ydens>=0 && p.ydens<2);
    assert(p.ydensxy>=0 && p.ydensxy<2);
    assert(p.yaveTemp>=0 && p.yaveTemp<2);
    assert(p.ymonPos>=0 && p.ymonPos<2);
}

/*
 * -------------------------------------------------
 * Calculate other parameters from input paremeters
 *
 * See param_struct.h for description of parameters
 * -------------------------------------------------
 */
void calcParams(void)
{
    int d;

    /* Misc. calculated quantities */
    p.root3 = sqrt(3.0);
    p.recRootdt = 1.0/sqrt(p.dt);

    /* Spring force stuff */
    p.RMAXHARM = 2.0;
    p.REQ = 0.7;
    p.MAXFORCEHARM = 50.0;
    p.diff2 = p.RMAXHARM-p.REQ;
    p.recdiff2sq = 1.0/(p.diff2*p.diff2);	

    /* Simulation box size/density/particle number stuff.
     * Some of this gets changed below if wall is present. 
     */
    for (d=0;d<3;d++)
    {
        p.L_HALF[d] = 0.5*p.L[d];
        p.cellDist[d] = p.L[d]/((int)p.L[d]);
    }
    p.Ncell = (int)p.L[0]*(int)p.L[1]*(int)p.L[2];
    p.Ntot = (int)(p.density*p.L[0]*p.L[1]*p.L[2]);
    p.Nfluid = p.Ntot - p.Nmon;
    p.recNmon = 1.0/p.Nmon;
 

    /*
     * Paremeters related to wall stuff.
     * If there is a pore, some of these get changed in initWallWithPore
     */
    if (p.wallLayers>0)
    {
        p.Nwall_x = (int)(p.L[0]*pow(p.wallDensity,1.0/3.0) + 0.5);
        p.Nwall_y = (int)(p.L[1]*pow(p.wallDensity,1.0/3.0) + 0.5);
        p.wallDist = p.L[0]/p.Nwall_x;
        p.Awall = 0.39*(p.density+0.1*p.Aff*p.density*p.density);
        p.Awall /= (0.0303*p.wallDensity*p.wallDensity+0.5617*p.wallDensity-0.8536);
        p.Nwall = p.wallLayers*p.Nwall_x*p.Nwall_y;

        p.Ntot = (int)(p.density*p.L[0]*p.L[1]*(p.L[2]-(p.wallLayers-1)*p.wallDist));
        p.Ntot += p.Nwall;
		
    }
    else
    {
        p.Nwall = 0;
        p.wallDist = p.Awall = 0.0;
    }

    /* Density histogram stuff */
    if (p.ydens)
    {
        p.NbinDens = 3000;
        p.binWidthDens = p.L[2]/p.NbinDens;
    }

}

/*
 * -----------------------------------
 * Print input parameters to log file
 * -----------------------------------
 */
void printParams(void)
{
    fprintf(f.log,"%.2f\tAff\n",p.Aff);
    fprintf(f.log,"%.2f\tAmm\n",p.Amm);
    fprintf(f.log,"%.2f\tAmf\n",p.Amf);
    fprintf(f.log,"%.2f\tAwall\n",p.Awall);
    fprintf(f.log,"%.2f\tsigma\n",p.SIGMA);
    fprintf(f.log,"%.2f\tgamma\n",p.GAMMA);
    fprintf(f.log,"%.2f\tK\n",p.K);
    fprintf(f.log,"%.2f\tfDrive\n",p.fDrive);

    fprintf(f.log,"\n%d\tNmon\n",p.Nmon);
    fprintf(f.log,"%.2f\tdensity\n",p.density);
    fprintf(f.log,"%.2f\tLx\n",p.L[0]);
    fprintf(f.log,"%.2f\tLy\n",p.L[1]);
    fprintf(f.log,"%.2f\tLz\n",p.L[2]);

    fprintf(f.log,"\n%.2f\twallDensity\n",p.wallDensity);
    fprintf(f.log,"%d\twallLayers\n",p.wallLayers);
    fprintf(f.log,"%.2f\tporeWidth\n",p.poreWidth);

    fprintf(f.log,"\n%.2f\tdt\n",p.dt);
    fprintf(f.log,"%.2f\ttMax\n",p.tMax);
    fprintf(f.log,"%.2f\teqTime\n",p.eqTime);
    fprintf(f.log,"%d\tfreqSamp\n",p.freqSamp);
    fprintf(f.log,"%d\tfreqOut\n",p.freqOut);

    fprintf(f.log,"\n%d\tydens\n",p.ydens);
    fprintf(f.log,"%d\tydensxy\n",p.ydensxy);
    fprintf(f.log,"%d\tyaveTemp\n",p.yaveTemp);
    fprintf(f.log,"%d\tymonPos\n",p.ymonPos);
}
