#include <iostream>
#include <math.h>
#include "sintesisFM.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

SintesisFM::SintesisFM(const std::string &param) 
  : adsr(SamplingRate, param) {
    // \TODO
  bActive = false;
  x.resize(BSIZE);
  /*
    You can use the class keyvalue to parse "param" and configure your instrument.
    Take a Look at keyvalue.h    
  */
  KeyValue kv(param);
  int N;

  if (!kv.to_int("N",N))
    N = 40; //default value
  if (!kv.to_float("I",I))
    I = 1; //default value 
  if (!kv.to_int("N1",N1))
    N1 = 4; //default value
  if (!kv.to_int("N2",N2))
    N2 = 7; //default value

  //fi = SamplingRate / N;
  //Fm = SamplingRate / fhz;
  I = 1. - pow(2, -I / 12.);
}

void SintesisFM::command(long cmd, long note, long vel) {
    /// \TODO diria que está bé pero
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index1 = 0;
    index2 = 0;
	  A = vel / 127.;
    float power = (note - 69.0) / 12.0;
    fc = 440 * pow(2.0,power);
    fm = (N2/N1) * fc;
  }
  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
}

const vector<float> & SintesisFM::synthesize() {
    /// \TODO
  if (not adsr.active()) {  // no hay sonido
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (not bActive)     // Tecla no pulsada
    return x;

  for (unsigned int i=0; i<x.size(); ++i) { //Mantenimiento
    x[i] = A * sin(index1 + I*sin(index2));

    index1 += 2 * M_PI * SamplingRate / fc;
    index2 += 2 * M_PI * SamplingRate / fm;

    while (index1 >= 2*M_PI)  index1 -= 2*M_PI;
    while (index2 >= 2*M_PI)  index2 -= 2*M_PI;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}
