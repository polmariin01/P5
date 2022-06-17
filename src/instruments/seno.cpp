#include <iostream>
#include <math.h>
#include "seno.h"
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
  
  fi = SamplingRate / N;
  //Create a tbl with one period of a sinusoidal wave, ftbl = FS/N
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
    float power = (note - 69.0) / 12.0;
    float f0 = 440 * pow(2.0,power);
    fd = f0 / fi;
    cout << "Note: " << note << " pow: " << power << " F0: " << f0 << " FD: " << fd << "\n";
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
    float i_floor = index * fd;
    //cout << "Index nou: " << i_floor << " ";
    while (i_floor >= tbl.size())
        i_floor -= tbl.size();      //especie de modulo para que el indice sea un valor válido
    float fd_floor = i_floor - floor(i_floor);
    //cout << fd_floor << "\n";
    i_floor = floor(i_floor);

    x[i] = A * ((1-fd_floor) * tbl[i_floor] + fd_floor * tbl[i_floor+1]);
    index++;

    //if (index == tbl.size())
    //  index = 0;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}
