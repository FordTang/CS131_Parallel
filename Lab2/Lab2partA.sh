#!/bin/bash
#
#$ -cwd
##$ -j y
#$ -S /bin/bash
#$ -M fordt1@uci.edu
#$ -pe openmpi 4
#$ -i partA.txt
#$ -o outputA.txt
#$ -e errorA.txt
#
#
# Use modules to setup the runtime environment
#
module load sge
module load openmpi/1.6
#
# Execute the run
#
mpirun -np $NSLOTS ./Lab2partA