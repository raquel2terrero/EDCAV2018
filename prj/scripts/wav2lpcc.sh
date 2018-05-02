#!/bin/bash
if [[ $# != 4 ]]; then
   echo "$0 lpc_order ncoef input.wav output.mcp"
   exit 1
fi

# TODO
# This is a very trivial feature extraction.
# Please, read sptk documentation and some papers,
# and apply a better front end to represent the speech signal

lpc_order=$1
nceps=$2
inputfile=$3
outputfile=$4


base=/tmp/wav2lpcc$$  # temporal file  
sox $inputfile $base.raw # $3 => 3rd argument, input.wav
x2x +sf < $base.raw | frame -l 400 -p 80 | window -l 400 -L 512 |\
          lpc -l 400 -m $lpc_order | lpc2c -m $lpc_order -M $nceps > $base.cep

# Our array files need a header with number or cols and number of rows:
ncol=$((nceps+1))
nrow=`x2x +fa < $base.cep | wc -l | perl -ne 'print $_/'$ncol', "\n";'`
echo $nrow $ncol | x2x +aI > $outputfile
cat $base.cep >> $outputfile
\rm -f $base.raw $base.cep
exit
