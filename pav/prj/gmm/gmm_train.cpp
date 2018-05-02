#include <unistd.h> //getopt function, to parse options
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "gmm.h"
#include "filename.h"

using namespace std;
using namespace upc;

const string DEF_INPUT_EXT = "mcp";
const unsigned int DEF_ITERATIONS = 20;
const float DEF_THR = 1e-3;
const unsigned int DEF_NMIXTURES = 5;
const string DEF_GMMFILE = "output.gmc";

int read_data(const string & input_directory, const string & input_extension, 
	      const vector<string> &filenames, fmatrix &dat);

int usage(const char *progname, int err);

int read_options(int ArgC, const char *ArgV[], Directory &input_dir, Ext &input_ext, vector<string> &filenames, 
		 unsigned int &nmix, string &gmm_filename, 
		 unsigned int &niterations, unsigned int &ending_iterations, float &threshold, float &ending_threshold, 
		 int &init_method, unsigned int &verbose);

int main(int argc, const char *argv[]) {
  
  Directory input_dir;
  Ext input_ext(DEF_INPUT_EXT);
  vector<string> filenames;
  unsigned int nmix = DEF_NMIXTURES, verbose = 1;
  Filename gmm_filename(DEF_GMMFILE);
  unsigned int niterations=DEF_ITERATIONS, ending_iterations=DEF_ITERATIONS;
  float threshold=DEF_THR, ending_threshold=DEF_THR;
  int init_method=1;

  ///Read command line options
  int retv = read_options(argc, argv, input_dir, input_ext, filenames,
		      nmix, gmm_filename, 
		      niterations, ending_iterations, threshold, ending_threshold,
		      init_method, verbose);
  if (retv != 0)
    return usage(argv[0], retv);

  //Read data from filenames
  fmatrix data;
  read_data(input_dir, input_ext, filenames, data);
  cout << "DATA: " << data.nrow() << " x " << data.ncol() << endl;

  GMM gmm;
  //TODO: initialize GMM from data
  //You can use several options: random, vq, em_split ...
  //See the options and select a number for each option

  switch (init_method) {
  case 1: //random
	gmm.random_init(data, nmix);
    break;
  case 2: //vq
	gmm.vq_lbg(data, nmix, niterations, threshold, verbose);
    break;
  case 3: //em_split
	gmm.em_split(data, nmix, ending_iterations, threshold, verbose);
    break;
  default:
	gmm.random_init(data, nmix);
  }

  //TODO: Apply EM to estimate GMM parameters (complete the funcion in gmm.cpp)
	gmm.em(data, ending_iterations, ending_threshold, verbose);

  //Create directory, if it is needed
  gmm_filename.checkDir();
  //Save gmm
  ofstream ofs(gmm_filename.c_str(), ios::binary);
  ofs << gmm;
  
  bool show_gmm=false;
  if (show_gmm)
    gmm.print(cout);

  return 0;  
}

int usage(const char *progname, int err)  {
  cerr << "Usage: " << progname << " [options] list_of_train_files\n";
  cerr << "Usage: " << progname << " [options] -F train_file1 ...\n\n";
  
  cerr << "Options can be: \n"
       << "  -d dir\tDirectory of the input files (def. \".\")\n"
       << "  -e ext\tExtension of the input files (def. \"" << DEF_INPUT_EXT << "\")\n"
       << "  -m mix\tNumber of mixtures (def. " << DEF_NMIXTURES << ")\n"
       << "  -g name\tName of output GMM file  (def. " << DEF_GMMFILE << ")\n"
       << "  -n ite\tNumber of (intermediate) iterations of EM (def. " << DEF_ITERATIONS << ")\n"
       << "  -N ite\tNumber of final iterations of EM (def. " << DEF_ITERATIONS << ")\n"
       << "  -i init\tInitialization method (def. 0)\n"
       << "  -t thr\tLogProbability threshold of (intermediate) EM iterations (def. " << DEF_THR << ")\n"
       << "  -T thr\tLogProbability threshold of final EM iterations (def. " << DEF_THR << ")\n"
       << "  -v int\tBit code to control \"verbosity\"; eg: 5 => 00000101" << ")\n";
  return err;
}

int read_options(int ArgC, const char *ArgV[], Directory &input_dir, Ext &input_ext, vector<string> &filenames, 
		 unsigned int &nmix, string &gmm_filename, 
		 unsigned int &niterations, unsigned int &ending_iterations, float &threshold, float &ending_threshold, 
		 int &init_method, unsigned int &verbose) {

  char option;
  bool use_list = true;
  filenames.clear();

  //optarg and optind are global variables declared and set by the getopt() function

  while ((option = getopt(ArgC, (char **)ArgV, "d:e:m:g:n:N:t:T:i:v:F")) != -1) {
    switch (option) {
    case 'd': input_dir = optarg; break;
    case 'e': input_ext = optarg; break;
    case 'm': nmix = atoi(optarg); break;
    case 'g': gmm_filename = optarg; break;
    case 'n': niterations = atoi(optarg); break;
    case 'N': ending_iterations = atoi(optarg); break;
    case 't': threshold = atof(optarg); break;
    case 'T': ending_threshold = atof(optarg); break;
    case 'i': init_method = atoi(optarg); break;
    case 'v': verbose = atoi(optarg); break;
    case 'F': use_list=false; break;
    case '?': return -1;
    }
  }

  if (nmix == 0) {
    cerr << ArgV[0] << ": nmixtures must be > 0\n";
    return -2;
  }

  if (!input_dir.empty() and *input_dir.rbegin() != '/') input_dir += '/';
  if (!input_ext.empty() and input_ext[0] != '.') input_ext = '.' + input_ext;

  //advance argc and argv to skip read options
  ArgC -= optind;
  ArgV += optind;

  //Save name of files in vector 'filenames'
  if (use_list) {
    if (ArgC != 1) {
      cerr << "ERROR no list of files provided" << endl;
      return -2;
    }
    ifstream is(ArgV[0]);
    if (!is.good()) {
      cerr << "ERROR opening list of files: " << ArgV[0] << endl;
      return -3;
    }
    string s;
    while (is >> s)
      filenames.push_back(s);
  } else {
    for (int i=0; i<ArgC; ++i)
      filenames.push_back(ArgV[i]);
  }
  return 0;      
}


int read_data(const string & input_directory, const string & input_extension, 
	      const vector<string> &filenames, fmatrix &dat) {
  fmatrix dat1;
  for (unsigned int i=0; i<filenames.size(); ++i) {
    string path = input_directory + filenames[i] + input_extension;
    ifstream is(path.c_str(), ios::binary);
    if (!is.good()) {
      cerr << "Error reading file: " << path << endl;
      dat.reset();
      return -1;
    }
    if (i==0) {
      is >> dat;
    } else {
      is >> dat1;
      if (dat1.ncol() != dat.ncol()) {
	cerr << "Error in vector dimension: " << filenames[i] << dat1.ncol() 
	     << " (expected: " << dat.ncol() << ")\n";
	dat.reset();
	return -1;
      }
      int row = dat.nrow();
      dat.resize(dat.nrow()+dat1.nrow(), dat.ncol());
      for (unsigned int i=0; i<dat1.nrow(); ++i, ++row)
	for (unsigned int j=0; j<dat1.ncol(); ++j)
	  dat[row][j] = dat1[i][j];
    }
  }
  return 0;    
}
