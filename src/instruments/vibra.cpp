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
    I = 40; //default value 
  if (!kv.to_int("F",fhz))
    N = 40; //default value

  fi = SamplingRate / N;
  Fm = SamplingRate / fhz;
}

void Vibra::command(long cmd, long note, long vel) {
    /// \TODO diria que está bé pero
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index1 = 0;
    index2 = 0;
	  A = vel / 127.;
    float power = (note - 69.0) / 12.0;
    f0 = 440 * pow(2.0,power);
    fd = f0 / fi;

    //cout << "Note: " << note << " pow: " << power << " F0: " << f0 << " FD: " << fd << "\n";
  }
  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
  /// \aqui haurem de decidir Fm en funció del valor de note
  // note=0 -> f = 8.1758Hz, note=127 -> f = 12544Hz
  // note = 69 + 12 * log2(f0/440)
  // f0 = 440 * 2^((note-69)/12)
   //Frecuencia del senyal que volem generar
  //aquesta es la f a la que volem modular la tabla
  //Fs = 44100Hz (mostreig)
  // La frecuencia de la señal original tbl es Fs/N, en nuestro caso es 1102,5Hz
  //Pero mejor la ponemos en función de las dos variables

  //el factor de diezmado lo encontramos dividiento la f0 (queremos) entre la del señal original
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
