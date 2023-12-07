#include "defines.h"
#include "scales.h"

void scaler(unscaled_t X_unscaled[N_INPUTS], scaled_t X_scaled[N_INPUTS]){
  #pragma HLS pipeline II=1
  #pragma HLS array_partition variable=X_unscaled complete   // leave this interface as an array
  #pragma HLS array_reshape variable=X_scaled complete dim=0 // make an hls4ml-style wide interface
  for(int i = 0; i < N_INPUTS; i++){
    X_scaled[i] = (X_unscaled[i] - bias[i]) * scale[i];
  }
}