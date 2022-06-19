#ifndef VIBRA
#define VIBRA

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class Vibra: public upc::Instrument {
    EnvelopeADSR adsr;
    float index1, index2;
    float A;
    std::vector<float> tbl;
    float fi, //frec señal inicial (Fs/N)
          f0,
          fd, //factor diezmado
          Fm, //fm en hercios
          I;  
    /// \TODO Igual le faltan más parámetross
  public:
    Vibra(const std::string &param = "");
    void command(long cmd, long note, long velocity=1); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif
