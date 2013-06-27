/*	
 *	Merges nseg and re_cis/re_trans outputs
 *		to get re_cis/re_trans vs N
 * 
 *	Copyright (C) Matthieu Hughes 2013
 *	matthieuhughes.1@gmail.com
 *
 *	last update: 24/06/13
 *
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Error message and exit if memory can't be allocated */
void memoryError(void)
{
    fprintf(stderr,"Can't allocate memory");
    exit(1);
}

/* For allocating memory */
void allocateIntArray(int **array, int n)
{
    (*array) = malloc(n*sizeof(int));
    
    if ((*array) == NULL)
        memoryError();
}
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
void fileOpenError(char *filename)
{
    fprintf(stderr,"Cannot open file %s\n",filename);
}

int main()
{

    /*
     * ---------------------
     * VARIABLE DECLARATIONS
     * --------------------- 
     */

    /* IO files */
    FILE 
        *dpdIn,
        *runIn,
        *nsegIn,
        *recisIn,
        *retransIn,
        *cisOut,
        *transOut;

    /* Strings to store filenames */
    char 
        recis[30],
        retrans[30],
        nsegpart[30];

    /* Parameters */
    int 
        npts, 
        njobs,
        filesPerJob, 
        freqSamp,  
        N, 
        index_c,
        index_t;
    
    double 
        dt, 
        dtSamp, 
        tmax, 
        eqTime; 

    /* Loop counters/data arrays */
    int 
        d,
        i,
        m,
        n,
        *count_c,
        *count_t,
        *nseg_c,
        *nseg_t;
    
    double 
        **re_c,
        **re_t,
        **re_av_c,
        **re_av_t;


    /* 
     * ----------------
     * READ INPUT FILES
     * ----------------
     */

    int paramCount = 0;

    /* Scan from dpd.inp */
    if ((dpdIn = fopen("dpd.inp","r")) == NULL)
        fileOpenError("dpd.inp");
	
    /* Force strengths */
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");	// Aff
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");	// Amm
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");	// Amf
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");	// sigma
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");	// gamma
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");	// K
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");	// fDrive

    /* Particle info */
    paramCount += fscanf(dpdIn,"%d\t%*s\n",&N);
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");	// density
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");	// L[0]
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");	// L[1]
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");	// L[2]

    /* Wall info */
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");	// wallDensity
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");	// wallLayers
    paramCount += fscanf(dpdIn,"%*f\t%*s\n");	// poreWidth

    /* Run info */
    paramCount += fscanf(dpdIn,"%lf\t%*s\n",&dt);
    paramCount += fscanf(dpdIn,"%lf\t%*s\n",&tmax);
    paramCount += fscanf(dpdIn,"%lf\t%*s\n",&eqTime);
    paramCount += fscanf(dpdIn,"%d\t%*s\n",&freqSamp);
    paramCount += fscanf(dpdIn,"%*d\t%*s\n");	// freqOut

    /* What to print */
    paramCount += fscanf(dpdIn,"%*d\t%*s\n");	// ydens
    paramCount += fscanf(dpdIn,"%*d\t%*s\n");	// yaveTemp
    paramCount += fscanf(dpdIn,"%*d\t%*s\n");	// ymonPos

    /* Scan from run.inp */
    if ((runIn = fopen("run.inp","r"))==NULL)
	fileOpenError("run.inp");
    
    paramCount += fscanf(runIn,"%*f\t%*s\n");	// version
    paramCount += fscanf(runIn,"%d\t%*s\n",&njobs);
    paramCount += fscanf(runIn,"%d\t%*s\n",&filesPerJob);
    paramCount += fscanf(runIn,"%*s\n");	// folderName
   
    printf("%d njobs\n%d filesPerJob\n",njobs,filesPerJob);
    assert(paramCount==7);	

    fclose(dpdIn);
    fclose(runIn);


    /*
     * ----------
     * INITIALIZE
     *-----------
     */

    /* Find npts */
    dtSamp = dt*freqSamp;
    
    if (eqTime < 0.0001)
    	npts = (int)(tmax/dtSamp+1);
    else if (eqTime < dtSamp)
    	npts = (int)(tmax/dtSamp);
    else
    	npts = (int)((tmax/dtSamp)-floor(eqTime/dtSamp)-1);

    /* Allocate memory for arrays */
    allocateIntArray(&nseg_c,npts);
    allocateIntArray(&nseg_t,npts);
    allocateIntArray(&count_c,N);
    allocateIntArray(&count_t,N);
    
    allocate2dArray(&re_c,npts,3);
    allocate2dArray(&re_t,npts,3);
    allocate2dArray(&re_av_c,N,3);
    allocate2dArray(&re_av_t,N,3);

    /* Initialize values to 0 */
    for (i=0; i<N; i++)
    {
        count_c[i] = count_t[i] = 0;
        for (d=0;d<3;d++)
            re_av_c[i][d] = re_av_t[i][d] = 0;
    }


    /*
     * ---------------------
     * READ RE AND NSEG DATA
     * ---------------------
     */

    /* Cycle through each file */
    for (n=0; n<njobs; n++)
    {
    	for (m=0;m<filesPerJob;m++)
    	{
            /* Concatenate file names */
	    if (filesPerJob == 1)
            {
                sprintf(recis, "re_cis/re_cis-%02d",n+1);
	        sprintf(retrans,"re_trans/re_trans-%02d",n+1);
	        sprintf(nsegpart, "nseg/nseg-%02d",n+1);
            }
            else
	    {
                sprintf(recis,"re_cis/re_cis-%02d-%02d",n+1,m+1);
                sprintf(retrans,"re_trans/re_trans-%02d-%02d",n+1,m+1);
                sprintf(nsegpart,"nseg/nseg-%02d-%02d",n+1,m+1);
	    }

            /* Continue if file is missing */
	    if ((recisIn = fopen(recis, "r")) == NULL)
	    {
		fileOpenError(recis);
	        continue;
	    }
	    if ((retransIn = fopen(retrans,"r"))==NULL)
	    {
	        fileOpenError(retrans);
	        continue;
	    }
	    if ((nsegIn = fopen(nsegpart,"r"))==NULL)
	    {
	        fileOpenError(nsegpart);
	        continue;
	    }
	    
            printf("successfully read %s\n",recis);
            /* Read data */
	    for (i=0; i<npts; i++)
	    {
	        fscanf(recisIn,"%*f %lf %lf %lf\n",
                       &re_c[i][0],&re_c[i][1],&re_c[i][2]);
		fscanf(retransIn,"%*f %lf %lf %lf\n",
                       &re_t[i][d],&re_t[i][d],&re_t[i][d]);
		fscanf(nsegIn,"%*f %d %d\n",
                       &nseg_c[i],&nseg_t[i]);
				
		index_c = nseg_c[i];
		index_t = nseg_t[i];
		if (index_c > 1)
		{
                    for (d=0;d<3;d++)
                        re_av_c[index_c][d] += re_c[i][d]*re_c[i][d];
		    
                    count_c[index_c]++;
		}
		if (index_t > 1)
		{
                    for (d=0;d<3;d++)
                        re_av_t[index_t][d] += re_t[i][d]*re_t[i][d];
                    
                    count_t[index_t]++;
		}
	    }
		
            fclose(recisIn);	
            fclose(retransIn);
	    fclose(nsegIn);

	} // end filesperjob loop
    } //end nfiles loop

    free2dArray(&re_c,npts,3);
    free2dArray(&re_t,npts,3);
    free(nseg_c);
    free(nseg_t);


    /*
     * --------------------------
     * CALCULATE RENSEG AND PRINT 
     * --------------------------
     */

    /* Open output files */
    if ((cisOut = fopen("re2nseg_cis", "w"))==NULL)
        fileOpenError("re2nseg_cis");
    if ((transOut = fopen("re2nseg_trans","w"))==NULL)
    	fileOpenError("re2nseg_trans");

    /* Print Re^2 vs. N for cis and trans */
    for (i=2; i<N; i++) // <=N or <N?
    {
        if (count_c[i] > 0)
	{
            for (d=0;d<3;d++)
                re_av_c[i][d] /= count_c[i];
            fprintf(cisOut,"%d %f %f %f\n",i,
                    re_av_c[i][0],re_av_c[i][1],re_av_c[i][2]);
	}

	if (count_t[i] > 0)
        {
            for (d=0;d<3;d++)
                re_av_t[i][d] /= count_t[i];
            fprintf(transOut,"%d %f %f %f\n",i,
                    re_av_t[i][0],re_av_t[i][1],re_av_t[i][2]);
        }
    
    }

    free2dArray(&re_av_c,N,3);
    free2dArray(&re_av_t,N,3);
    free(count_c);
    free(count_t);

    fclose(cisOut);
    fclose(transOut);

    exit(0);
}
