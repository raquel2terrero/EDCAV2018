#!/bin/bash
if [[ $# != 4 ]]; then
   echo "$0 mfcc_order ncoef input.wav output.mcp"
   exit 1
fi

# TODO
# This is a very trivial feature extraction.
# Please, read sptk documentation and some papers,
# and apply a better front end to represent the speech signal

mfcc_order=$1
ncoef=$2
inputfile=$3
outputfile=$4
#outputfiled=$5
#outputfiledd=$6


base=/tmp/wav2lpccdd$$  # temporal file  
sox $inputfile $base.raw # $3 => 3rd argument, input.wav
sptk x2x +sf < $base.raw | sptk frame -l 400 -p 80 |\
          sptk mfcc -l 400 -n $mfcc_order -m $ncoef | sptk delta -l $ncoef -r 2 2 2 > $base.mfc

# Our array files need a header with number or cols and number of rows:
ncol=$((3*ncoef))
nrow=`sptk x2x +fa < $base.mfc | wc -l | perl -ne 'print $_/'$ncol', "\n";'`

echo $nrow $ncol | sptk x2x +aI > $base.mfcfm
cat $base.mfc >> $base.mfcfm
mv $base.mfcfm $outputfile

#fmatrix_cut -f 1-$ncol $base.mfc $outputfile
#fmatrix_cut -f 15,16,17,18,19,20,21,22,23,24,25,26,27,28 $base.mfc  $outputfiled
#fmatrix_cut -f 29,30,31,32,33,34,35,36,37,38,39,40,41,42 $base.mfc  $outputfiledd
#cat $base.mfc >> $outputfile
#cat $base.mfc >> $outputfiled
#cat $base.mfc >> $outputfiledd
\rm -f $base.raw $base.mcf

exit
