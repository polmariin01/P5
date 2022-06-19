#include <iostream>
#include <math.h>
#include "vibra.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

Vibra::Vibra(const std::string &param) 
  : adsr(SamplingRate, param) {
    // \TODO
  bActive = false;
  x.resize(BSIZE);
  /*
    You can use the class keyvalue to parse "param" and configure your instrument.
    Take a Look at keyvalue.h    
  */
  KeyValue kv(param);
  int N,fhz;

  if (!kv.to_int("N",N))
    N = 40; //default value
  if (!kv.to_float("I",I))
    I = 1; //default value 
  if (!kv.to_int("F",fhz))
    fhz = 8; //default value

  fi = SamplingRate / N;
  Fm = SamplingRate / fhz;
  I = 1. - pow(2, -I / 12.);
}

void Vibra::command(long cmd, long note, long vel) {
    /// \TODO diria que está bé pero
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index1 = 0;
    index2 = 0;
	  A = vel / 127.;
    f0 = 440. * (note - 69.) / 12.;
    fd = f0 / fi;

    //cout << "Note: " << note << " pow: " << power << " F0: " << f0 << " FD: " << fd << "\n";
  }
  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
}

const vector<float> & Vibra::synthesize() {
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

    index1 += 2 * M_PI * SamplingRate / f0;
    index2 += 2 * M_PI * Fm;

    while (index1 >= 2*M_PI)  index1 -= 2*M_PI;
    while (index2 >= 2*M_PI)  index2 -= 2*M_PI;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}
