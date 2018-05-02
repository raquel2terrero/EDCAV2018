#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include "vad.h"

#define DEBUG_VAD 0x1

int main(int argc, const char *argv[]) {
  int verbose = 0; 
  /* To show internal state of vad 
     verbose = DEBUG_VAD; 
  */

  SNDFILE *sndfile_in, *sndfile_out = 0;
  SF_INFO sf_info;
  FILE *vadfile;
  int n_read, n_write, i, u=0, s=0, v=0;

  VAD_DATA *vad_data;
  VAD_STATE state, last_state;

  float *buffer, *buffer_zeros;
  int frame_size;        /* in samples */
  float frame_duration;  /* in seconds */
  float t, last_t, t_final;

  if (argc != 3 && argc != 4) {
    fprintf(stderr, "Usage: %s input_file.wav output.vad [output_file.wav]\n", 
            argv[0]);
    return -1;
  }

  /* Open input sound file */
  sndfile_in = sf_open(argv[1], SFM_READ, &sf_info);
  if (sndfile_in == 0) {
    fprintf(stderr, "Error opening input file: %s\n", argv[1]);
    return -1;
  }

  if (sf_info.channels != 1) {
    fprintf(stderr, "Error: the input file has to be mono: %s\n", argv[1]);
    return -2;
  }

  /* Open vad file */
  vadfile = fopen(argv[2], "wt");
  if (vadfile == 0) {
    fprintf(stderr, "Error opening output vad file: %s\n", argv[2]);
    return -1;
  }

  /* Open output sound file, with same format, channels, etc. than input */
  if (argc == 4) {
    sndfile_out = sf_open(argv[3], SFM_WRITE, &sf_info);
    if (sndfile_out == 0) {
      fprintf(stderr, "Error opening output wav file: %s\n", argv[3]);
      return -1;
    }
  }

  vad_data = vad_open(sf_info.samplerate);
  /* Allocate memory for buffer */
  frame_size   = vad_frame_size(vad_data);
  buffer       = (float *) malloc(frame_size * sizeof(float));
  buffer_zeros = (float *) malloc(frame_size * sizeof(float));
  for (i=0; i< frame_size; ++i) buffer_zeros[i] = 0.0F;

  frame_duration = (float) frame_size/ (float) sf_info.samplerate;
  t = last_t = 0;
  last_state = ST_UNDEF;

  while(1) { /* For each frame ... */
    n_read = sf_read_float(sndfile_in, buffer, frame_size);

    /* End loop when file has finished (or there is an error) */
    if  (n_read != frame_size)
      break;

    if (sndfile_out != 0) {
      /* TODO: copy all the samples into sndfile_out */
			n_write = sf_write_float(sndfile_out, buffer, frame_size);
    }

    state = vad(vad_data, buffer);

    if (verbose & DEBUG_VAD)
      vad_show_state(vad_data, stdout);


    if (sndfile_out != 0) {
      /* TODO: go back and write zeros if silence */
			if(state == ST_SILENCE ){
				sf_seek(sndfile_out, -n_write,SEEK_CUR);
				sf_write_float(sndfile_out, buffer_zeros, frame_size);
				}
    	}

		if (state==ST_UNDEF){
			/* Comptador undefs*/			
			u++;
			}

    if (state != last_state && state != ST_UNDEF) {
      if (t != last_t){
        /*fprintf(vadfile, "%f\t%f\t%s\n", last_t, t, state2str(last_state));*/
				

				if (state==ST_SILENCE){
					t_final= u*frame_duration;					
					fprintf(vadfile, "%f\t%f\t%s\n", last_t, t, state2str(last_state));
					u=0;
					}				
				if (state==ST_VOICE){
					fprintf(vadfile, "%f\t%f\t%s\n", last_t, t, state2str(last_state));
					u=0;
					}
							
			}
      last_state = state;
      last_t = t;
    }
    t += frame_duration;
		if (state!= ST_UNDEF){
			u=0;
			}
  }

  state = vad_close(vad_data);
  if (t != last_t)
    fprintf(vadfile, "%f\t%f\t%s\n", last_t, t, state2str(state));

  /* clean up: free memory, close open files */
  free(buffer);
  free(buffer_zeros);
  sf_close(sndfile_in);
  fclose(vadfile);
  if (sndfile_out) sf_close(sndfile_out);
  return 0;
}
