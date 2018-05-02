#!/bin/bash
if [[ $# != 4 ]]; then
   echo "$0 numBand numcoef input.wav output.mcp"
   exit 1
fi

# TODO
# This is a very trivial feature extraction.
# Please, read sptk documentation and some papers,
# and apply a better front end to represent the speech signal

numBand=$1
numcoef=$2
inputfile=$3
outputfile=$4


base=/tmp/wav2lpcc$$  # temporal file  
sox $inputfile $base.raw # $3 => 3rd argument, input.wav
sptk x2x +sf < $base.raw | sptk frame -l 400 -p 80 | sptk window -l 400 -L 400 |\
          sptk mfcc -l 400 -m $numcoef -n $numBand > $base.cep

# Our array files need a header with number or cols and number of rows:
ncol=$((numcoef))
nrow=`sptk x2x +fa < $base.cep | wc -l | perl -ne 'print $_/'$ncol', "\n";'`
echo $nrow $ncol | sptk x2x +aI > $outputfile
cat $base.cep >> $outputfile
\rm -f $base.raw $base.cep
exit
