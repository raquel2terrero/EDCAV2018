#include <iostream>
#include <fstream>
#include "wavfile_mono.h"
#include "pitch_analyzer.h"


#define FRAME_LEN 0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

/**
   Main program: 
   Arguments: 
   - input (wav) file
   - output (txt) file with f0 
     one value per line with the f0 for each frame 
     (or 0 for unvoiced frames)
*/

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " input_file.wav output_file.f0\n";
    return -1;
  }


  /// Read input sound file
  unsigned int rate;
  vector<float> x;
  int retv = readwav_mono(argv[1], rate, x);
  if (retv != 0) {
    cerr << "Error reading input file: %d\n" 
	 << "Error value: " << retv << endl;
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  ///Define analyzer
  PitchAnalyzer analyzer(n_len, rate, PitchAnalyzer::HAMMING, 50, 500);

  ///You can preprocess the input data x ....
  
  ///Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  //You can post-process the f0 values

  ///Write f0 contour into the output file
  ofstream os(argv[2]);
  if (!os.good()) {
    cerr << "Error opening output file " << argv[2] << endl;
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur


  return 0;
}
