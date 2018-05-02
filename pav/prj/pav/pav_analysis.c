 #include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include "pav_analysis.h"

#define N 160

int sgn(float x) {
  if (x < 0)
    return -1;
  else 
    return 1;
}

float compute_power(const float *x, unsigned int M) {
  float pot=0.0;
  int i;
  for(i=0; i<=M-1; i++) {
    pot = pot + x[i]*x[i];
  }
  return 10.0*log10((1.0/M)*pot); 
}

float compute_am(const float *x, unsigned int M) {
  float am=0.0;
  int i;
  for(i=0; i<=M-1; i++) {
     am = am + fabs(x[i]);
  }
  return (1.0/M)*am;
}

float compute_zcr(const float *x, unsigned int M) {
  float zcr=0.0;
  int i, sgn_ant;
  sgn_ant = sgn(x[0]);
  for (i=1; i<M; i++) {
    if (sgn(x[i])!=sgn_ant) {
      zcr = zcr + 1.0;
      sgn_ant = sgn_ant*(-1);//Actualizamos el valor del signo anterior porque ha cambiado
    }
  }
  return (16000.0/2.0)*(1.0/(M-1))*zcr;
}


#undef TPM
#ifdef TPM
int main (int argc , const char *argv[]) {
  short buffer[N];
	if (argc!=2 && argc!=3) {
		fprintf(stderr,"%s: inputfile.wav [outputfile.txt]\n", argv[0]);
		return -1;
	}
	FILE *fpIn, *fpOut;
  fpIn = fopen(argv[1], "r");
	if (fpIn == NULL) {
		fprintf(stderr, "%s: error al abrir %s (%s)\n", argv[0], argv[1], strerror(errno));
		return -1;
	}
  if (argc==3) {
    fpOut = fopen(argv[2],"w");
  }
  else {
    fpOut = stdout;
  }
 
  fseek(fpIn, 44, SEEK_SET); //Descartamos cabecera 
  float x[N], pot, ampl, crc;
  int i, cont_tramo=0; 
  float norm_factor = 1.0/ (float) 0x8000;

	while (1) {
		if (fread(buffer, sizeof(*buffer), N, fpIn) < N) {
      break;
    }
		else {
      //Conversión de tipo y normalización
		  for (i=0; i<N; i++) {
        x[i] = (float) buffer[i]*norm_factor;
      }
      //Cálculo características
      pot = compute_power(x,N);
      ampl = compute_am(x,N);
      crc = compute_zcr(x,N);
      fprintf(fpOut, "%d\t%f\t%f\t%f\n", cont_tramo, pot, ampl, crc);
      cont_tramo++;
    }
	}

  fclose(fpIn);
  fclose(fpOut);
	return 0;

}

#endif /* TPM */
