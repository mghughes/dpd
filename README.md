dpd
===

A program which uses the dissipative particle dynamics (DPD) method to model the dynamics of polymers in explicit solvent.

Current funcionality includes:
    * Polymer translocation through a nanopore
    * Polymer tethered to a wall
    * Free polymer


Build
-----

Compile all.c in the src directory (requires link to math library). Other options in makefile don't currently work (will change this in the future).


Run
---
Run in a directory with input file dpd.inp. Or use sub.sh and run.sh to submit jobs to any Sun Grid Engine system.


Input parameters
----------------
            
            usual/
            suggested
variable    value       type    description

Aff         25.0        float   Fluid-fluid conservative force strength (25)
Amm         25.0        float   Monomer-monomer conservative force strength (25)
Amf         15.0        float   Monomer-fluid conservative force strength (15)
sigma       3.0         float   Random force strength (3)
gamma       4.5         float   Dissipative force strength (4.5)
K           40.0        float   Spring constant for FENE force (40)
fDrive      -           float   Nanopore driving force
Nmon        -           int     Length of polymer chain
density     3.0         float   Fluid density (3)
Lx          -           float   System size, x direction
Ly          -           float   System size, y direction
Lz          -           float   System size, z direction
wallDensity 3.0         float   density of wallParticles
                                    * Set to <=0 for no wall
wallLayers  2 or 3      int     Number of wall particle layers (2 or 3)
poreWidth               float   Nanopore width (square side length)
                                    * Set to <0 for no pore
dt          0.04        float   Timestep (0.04)
tMax        -           float   Simulation time
eqTime      -           float   Time for system to equilibrate
freqSamp    -           int     Sampling freqency for calculated quantities
freqOut     1000        int     Print frequency for sampled quantities
ydens       0           1/0     Set to 1 to sample fluid density vs. z
yaveTemp    0           1/0     Set to 1 to sample average temperature
ymonPos     0           1/0     Set to 1 to print monomer positions vs time

