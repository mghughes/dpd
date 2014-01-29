/*  
 *  Calculates average time autocorrelation function of R_e or R_g
 *	Ouputs one file for each component (x,y,z)
 *      Average these afterward if desired
 * 
 *  Copyright (C) Matthieu Hughes 2013
 *  matthieuhughes.1@gmail.com
 *
 *  last update: 24/06/13
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#define PI 3.141592654

/* Error message and exit if memory can't be allocated */
void memoryError(void)
{
    fprintf(stderr,"Error: can't allocate memory\n");
    exit(1);
}

/* For allocating memory */
void allocate2dArray(double ***array, int n, int m)
{
    int i;

    (*array) = malloc(n*sizeof(int*));
    if ((*array) == NULL)
        memoryError();

    for (i=0;i<n;i++)
    {
        (*array)[i] = malloc(m*sizeof(double));
        if ((*array)[i] == NULL)
            memoryError();
    }
}

/* For freeing memory */
void free2dArray(double ***array, int n, int m)
{
    int i;
    for (i=0;i<n;i++)
        free( (*array)[i] );
   
    free( (*array) );
}

/* Error message if file can't be opened */
void fileOpenError(char *str)
{
    fprintf(stderr,"Could not open file %s\n",str);
}

/* MAIN FUNCTION
 * 
 * Command line arguments:
 * [1] str filename
 * [2] int npts_corr
 * [3] int exclude
 */
int main(int argc,char *argv[])
{
    /* Parse command-line arguments */
    char *filename, *exclude_s;
    filename = argv[1];
    exclude_s = argv[2];


    /*
     * ---------------------
     * VARIABLE DECLARATIONS
     * ---------------------
     */

    /* IO files */
    FILE *dpdIn, 
         *runIn,
         *reIn,
         *reOut;

    /* Strings to store filenames */
    char remon[30];

    /* Parameters */
    int 
        npts,
        exclude = atoi(exclude_s),
        njobs,
        filesPerJob,
        freqSamp;

    double 
        dt,
        dtSamp,
        tmax,
        eqTime;

    /* Loop counters/data arrays */
    int
        d,
        i,
        j,
        m,
        n,
        count,
        paramCount = 0;

    double 
        reav[3],
        re2av[3],
        retemp[3],
        re2temp[3],
        **re;


    /*
     * ----------------
     * READ INPUT FILES 
     * ----------------
     */ 
        
    /* Scan from dpd.inp */
    if ((dpdIn = fopen("dpd.inp","r")) == NULL)
    {
        fileOpenError("dpd.inp");
        exit(1);
    }

    // force strengths
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // Aff
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // Amm
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // Amf_cis
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // Amf_trans
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // sigma
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // gamma
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // sigma_pore
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // gamma_pore
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // K
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // fDrive

    // particle info
    paramCount += fscanf(dpdIn,"%*d\t%*s\n");   // Nmon
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // density
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // L[0]
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // L[1]
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // L[2]

    // wall info
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // wallDensity
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // wallLayers
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");   // poreWidth

    // run info
    paramCount += fscanf(dpdIn,"%lf\t%*s\n",&dt);
    paramCount += fscanf(dpdIn,"%lf\t%*s\n",&tmax);
    paramCount += fscanf(dpdIn,"%lf\t%*s\n",&eqTime);
    paramCount += fscanf(dpdIn,"%d\t%*s\n",&freqSamp);
    paramCount += fscanf(dpdIn,"%*d\t%*s\n");   // freqOut

    // what to print
    paramCount += fscanf(dpdIn,"%*d\t%*s\n");   // ydens
    paramCount += fscanf(dpdIn,"%*d\t%*s\n");   // yaveTemp
    paramCount += fscanf(dpdIn,"%*d\t%*s\n");   // ymonPos 
		
    // from run.inp
    if ((runIn = fopen("run.inp","r"))==NULL)
    {
        fileOpenError("run.inp");
        exit(1);
    }

    paramCount += fscanf(runIn,"%*f\t%*s\n");   // version
    paramCount += fscanf(runIn,"%d\t%*s\n",&njobs);
    paramCount += fscanf(runIn,"%d\t%*s\n",&filesPerJob);
    paramCount += fscanf(runIn,"%*s\n");       // folderName

    assert(paramCount==6); 
    
    fclose(dpdIn);
    fclose(runIn);


    /* 
     * ----------
     * INITIALIZE
     * ----------
     */

    /* Find npts */
    dtSamp = dt*freqSamp;

    if (eqTime < 0.0001)
        npts = (int)(tmax/dtSamp+1);
    else if (eqTime < dtSamp)
        npts = (int)(tmax/dtSamp);
    else
        npts = (int)((tmax/dtSamp)-floor(eqTime/dtSamp)-1);

    /* Allocate memory for re 2D arrays */
    allocate2dArray(&re,npts,3);

    /* Initialize values to 0 */
    for (d=0;d<3;d++)
        reav[d] = re2av[d] = retemp[d] = re2temp[d] = 0;
    
    /*
     * -----------------------
     * READ RE DATA FROM FILES
     * -----------------------
     */

    char outFilename[20];

    sprintf(outFilename, "%s_ave", filename);	

    if ((reOut=fopen(outFilename,"w"))==NULL)
        fileOpenError(outFilename);

    count=0;
    /* Loop through jobs */
    for (n=0; n<njobs; n++)
    {
	for (m=0;m<filesPerJob;m++)
	{
            /* Reset temp arrays */
            for (d=0;d<3;d++)
                retemp[d] = re2temp[d] = 0;		
	        
            /* Concatenate file names */
            if (filesPerJob == 1)
                sprintf(remon,"%s-%02d",filename,n+1);
            else
                sprintf(remon,"%s-%02d-%02d",filename,n+1,m+1);
	    
            /* Open File */
            if ((reIn = fopen(remon, "r")) == NULL)
  	    {
                fileOpenError(remon);
                continue;
  	    }
  	   
            /* Read data */
            count++;	
	    int scans=0;

	    for (i=0;i<npts;i++)
	    {
	        scans+=fscanf(reIn,"%*f %lf %lf %lf\n",
                    &re[i][0], &re[i][1], &re[i][2]);
	    }
	    if (scans!=3*npts)
	        fprintf(stderr,"error: scanned number != npts in file %s",
                    remon);

	    /* Exclude equilibration time */
	    for (i=exclude; i<npts; i++)
	    {
                for (d=0;d<3;d++)
                {
		    retemp[d] += re[i][d];
		    re2temp[d] += re[i][d] * re[i][d];
                }				
	    }
	    
            /* Average over single file */
            for (d=0;d<3;d++)
            {

	        retemp[d] /= (npts-exclude);
	        re2temp[d] /= (npts-exclude);
            }

	    /* Increment total 
	    for (d=0;d<3;d++)
            {
                reav[d] += retemp[d];
                re2av[d] += re2temp[d];
            }*/

            fprintf(reOut, "%lf\n", sqrt(re2temp[0] + re2temp[1] + re2temp[2]));

	} // end filesPerJob loop
    } // end nfiles loop


    /* causing weird fatal error
    free2dArray(&corrtemp,npts_corr,3);
    free2dArray(&re,npts_corr,3);
    */

    /*
     * -------------------------------
     * CALCULATE CORRELATION FUNCTIONS 
     * -------------------------------
     */

    /* Averages over all files 
    for (d=0;d<3;d++)
    {
        reav[d] /= count;
        re2av[d] /= count;
    }*/


    /* Print correlation functions to three files 
    char outFilename[20];

    sprintf(outFilename, "%s2_ave", filename);	

    if ((re2Out=fopen(outFilename,"w"))==NULL)
        fileOpenError(outFilename);

    fprintf(re2Out, "%f", (re2av[0] + re2av[1] + re2av[2])/3);*/

    /* causing weird fatal error 
       free2dArray(&corr,npts_corr,3);
    */
    exit(0);
}
