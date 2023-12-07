#include "NN/L1TMLDemo_v1.h"
#include "emulator.h"
#include <any>
#include <array>

/*
HLS4ML Emulator Class
*/
class L1TMLDemo_emulator_v1 : public hls4mlEmulator::Model {

private:
    // Note: these need to match the defined model
    static const int N_INPUT=56;
    static const int N_OUTPUT=1;
    input_t _input[N_INPUT];
    result_t _result[N_OUTPUT];

public: 
  virtual void prepare_input(std::any input) {
    input_t *input_p = std::any_cast<input_t*>(input);
    
    // copy inputs
    for (int i = 0; i < N_INPUT; i++) {
      _input[i] = input_p[i];
    }
  }

  virtual void predict() {
    // call the NN HLS function to make predictions
    L1TMLDemo_v1(_input, _result);
  }
  
  virtual void read_result(std::any result) {
    // copy result
    result_t *result_p = std::any_cast<result_t*>(result);
    *result_p = *_result;
  }
  
};

// Create a new model object
extern "C" hls4mlEmulator::Model* create_model() {
    return new L1TMLDemo_emulator_v1;
}

// Destroy the model object
extern "C" void destroy_model(hls4mlEmulator::Model* m) {
    delete m;
}

