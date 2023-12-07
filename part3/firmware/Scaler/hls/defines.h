#ifndef L1TMLDEMO_DEFINES_H_
#define L1TMLDEMO_DEFINES_H_

#include "ap_fixed.h"

// these types all match the CMSSW emulator
static const int N_INPUTS=56;
typedef ap_fixed<14,13> unscaled_t;
typedef ap_fixed<16,7,AP_RND,AP_SAT> scaled_t;
typedef ap_fixed<16,6> scale_t;
typedef ap_fixed<16,6> bias_t;

#endif