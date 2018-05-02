#!/bin/bash
if [[ $# != 3 ]]; then
   echo "$0 lpc_order input.wav output.mcp"
   exit 1
fi

# TODO
# This is a very trivial feature extraction.
# Please, read sptk documentation and some papers,
# and apply a better front end to represent the speech signal

lpc_order=$1
inputfile=$2
outputfile=$3

base=/tmp/wav2lpcc$$  # temporal file  
sox $inputfile $base.raw 
sptk x2x +sf < $base.raw | sptk frame -l 400 -p 80 | sptk window -l 400 -L 400 |\
          sptk lpc -l 400 -m $lpc_order > $base.lp

# Our array files need a header with number or cols and number of rows:
ncol=$((lpc_order+1)) # lpc p =>  (gain a1 a2 ... ap) 

nrow=`sptk x2x +fa < $base.lp | wc -l | perl -ne 'print $_/'$ncol', "\n";'`

echo $nrow $ncol | sptk x2x +aI > $outputfile # $4 output file
cat $base.lp >> $outputfile
\rm -f $base.raw $base.lp
exit
