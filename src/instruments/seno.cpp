#include <iostream>
#include <math.h>
#include "instrument_dumb.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

Seno::Seno(const std::string &param) 
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
  
  //Create a tbl with one period of a sinusoidal wave
  tbl.resize(N);
  float phase = 0, step = 2 * M_PI /(float) N;
  index = 0;
  for (int i=0; i < N ; ++i) {
    tbl[i] = sin(phase);
    phase += step;
  }
}


void Seno::command(long cmd, long note, long vel) {
    /// \TODO diria que está bé pero
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index = 0;
	A = vel / 127.;
  }
  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }


  /// \aqui haurem de decidir Fm en funció del valor de note
  // note=0 -> f = 8.1758Hz
  // note=127 -> f = 12544Hz
  //
  // note = 69 + 12 * log2(f0/440)
  // f0 = 440 * 2^((note-69)/12)
  long f0 = 440 * exp2((note-69)/12); //Frecuencia del senyal que volem generar
  //aquesta es la f a la que volem modular la tabla
  //Fs = 44100Hz (mostreig)
  // La frecuencia de la señal original tbl es Fs/N, en nuestro caso es 1102,5Hz
  //Pero mejor la ponemos en función de las dos variables

  //el factor de diezmado lo encontramos dividiento la f0 (queremos) entre la del señal original
  fd = f0 * N / SamplingRate;
}


const vector<float> & Seno::synthesize() {
    /// \TODO
  if (not adsr.active()) {  // no hay sonido
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (not bActive)     // Tecla no pulsada
    return x;

  for (unsigned int i=0; i<x.size(); ++i) { //Mantenimiento
    //Diezmamos la sinusoide
    i_floor = floor(index * fd);
    fd_floor = i_floor - index * fd;

    x[i] = A * (fd_floor * tbl[i_floor] + (1-fd_floor) * tbl[i_floor+1]);
    index++;

    if (index == tbl.size())
      index = 0;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}
