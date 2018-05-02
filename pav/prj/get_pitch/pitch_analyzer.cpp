#include <iostream>
#include <math.h>
#include "pitch_analyzer.h"
using namespace std;
float PI=3.1415926535;
/*get_pitch input_file.wav input_file.f0  |cut -f1 > input_file.pot*/

namespace upc {
  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {

    for (unsigned int l=0; l< r.size(); ++l) {
      //TODO: Compute autocorrelation r[l]
			for(unsigned int n=l; n<x.size(); n++){
				r[l] = r[l] + x[n]*x[n-l];
			}
    }

    if (r[0] == 0.0F) //to avoid log() and divide zero 
      r[0] = 1e-10; 
  }

  void PitchAnalyzer::set_window(Window win_type) {
    if (frameLen == 0)
      return;

    window.resize(frameLen);

    switch (win_type) {
    case HAMMING:
      // TODO: implement the hamming window
			for(unsigned int n=0;n<frameLen;n++){		
				window[n]=0.53836-0.46164*cos((2*PI*n)/(frameLen-1));    
				}

			break;
    case RECT:
    default:
      window.assign(frameLen, 1);
    }
  }

  bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm) const {
    //TODO
    //Implement a rule to decide if the sound is voiced or not
    //You can use this features (pot, r1norm, rmaxnorm),
    //or compute and use other ones
		
		if(pot>-12 && r1norm>0.88){
			return false;
			}		


    return true;
  }

  float PitchAnalyzer::compute_pitch(vector<float> & x) const {
    if (x.size() != frameLen)
      return -1.0F;

    //Window input frame
    for (unsigned int i=0; i<x.size(); ++i)
      x[i] *= window[i];

    vector<float> r(npitch_max);

    //Compute correlation
    autocorrelation(x, r);

    vector<float>::const_iterator iR = r.begin(), iRMax = iR;

    //TODO: Find max value. iR: iterator to maxvalue
    /*
      - after the first negative value,
      - in the pitch_min, pitch_max range
    */
		/*cout << "adf*/
    unsigned int lag = 1;
		for(unsigned int m=1; m<npitch_min+1; m++){
			//if(r[m]<r[m-1] && r[m]<r[m+1])
			if(r[m]<0){
				lag = m;
				m=npitch_min+1;
			}
		}
	//trobar maxim
	  for(unsigned int M=lag; M<npitch_max-1; M++){
			//if(r[M]>r[M-1] && r[M]>r[M+1] )
			if(r[M]>r[lag])
				lag = M;
		}
		
		/*
    if (iRMax != r.end()) //normal case
      lag = iRMax - iR;
		*/

    float pot = 10 * log10(r[0]);

    //You can print these (and other) features, look at them using wavesurfer
    //Based on that, implement a rule for unvoiced

    if (r[0] > 0.0F)
      cout << pot << '\t' << r[1]/r[0] << '\t' << r[lag]/r[0] << endl;

    if (unvoiced(pot, r[1]/r[0], r[lag]/r[0]) or lag == 0)
      return 0;
    else
      return (float) samplingFreq/(float) lag;
  }


  void PitchAnalyzer::set_f0_range(float min_F0, float max_F0) {
    npitch_min = (unsigned int) samplingFreq/max_F0;
    if (npitch_min < 2)
      npitch_min = 2;  // samplingFreq/2

    npitch_max = 1 + (unsigned int) samplingFreq/min_F0;

    //frameLen should include at least 2*T0
    if (npitch_max > frameLen/2)
      npitch_max = frameLen/2;
  }

}
