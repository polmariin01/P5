#ifndef SINTESISFM
#define SINTESISFM

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class SintesisFM: public upc::Instrument {
    EnvelopeADSR adsr;
    int N1, N2;
    float index1, index2;
    float A;
    std::vector<float> tbl;
    float fc,
          fm,
          I;  
  public:
    SintesisFM(const std::string &param = "");
    void command(long cmd, long note, long velocity=1); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif
