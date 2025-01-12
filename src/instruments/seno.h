#ifndef INSTRUMENT_DUMB
#define INSTRUMENT_DUMB

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class Seno: public upc::Instrument {
    EnvelopeADSR adsr;
    unsigned int index;
	float A;
    std::vector<float> tbl;
    /// \TODO Igual le faltan más parámetross
  public:
    Seno(const std::string &param = "");
    void command(long cmd, long note, long velocity=1); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif
