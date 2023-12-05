# Part 3: deployment

In this section you will learn how to deploy the model from part 2 in the CMSSW emulator, and in L1T FPGA hardware.

## CMSSW Emulation

Every algorithm in the L1T needs a bitwise emulation for performance studies, and for production. Most emulators are developed in C++ and are directly part of CMSSW. In the context of HLS, emulator code is usually required to be different to the firmware code. The Machine Learning tools are a bit different to 'normal' emulators and in this section you will create an emulator from the hls4ml model in part 2.

For hls4ml NN emulation, each project should have a repository in the [cms-hls4ml GitHub organisation](https://github.com/cms-hls4ml). There we have the [`hls4mlEmulatorExtras`](https://github.com/cms-hls4ml/hls4mlEmulatorExtras) package that provides the interface we will use from CMSSW.

We will go through the process of locally compiling and testing an NN emulator. To be fully integrated to CMSSW (e.g. to make it to a release) the NN also requires a 'spec' in [cms-dist](https://github.com/cms-sw/cmsdist/tree/IB/CMSSW_14_0_X/master) to be made a CMSSW external that will be compiled centrally. See the spec for [AXOL1TL](https://github.com/cms-sw/cmsdist/blob/IB/CMSSW_14_0_X/master/AXOL1TL.spec) and [CICADA](https://github.com/cms-sw/cmsdist/blob/IB/CMSSW_14_0_X/master/CICADA.spec).

### emulator interface

To define the emulator for our model, we need to define a class inheriting from the `hls4mlEmulator::Model`. This is the contents of `part3/cms-hls4ml/L1TMLDemo/L1TMLDemo_v1/L1TMLDemo_v1.cpp` that represents the minimal working example for this workflow:

```c++
#include "NN/L1TMLDemo_v1.h"
#include "emulator.h"
#include <any>
#include <array>

/*
L1TMLDemo_v1.cpp
*/
class L1TMLDemo_emulator_v1 : public hls4mlEmulator::Model {

private:
    input_t _input[N_INPUT_1_1];
    result_t _result[N_LAYER_6];

public: 
  virtual void prepare_input(std::any input) {
    input_t *input_p = std::any_cast<unscaled_t*>(input);
    
    // copy inputs
    for (int i = 0; i < N_INPUT_1_1; i++) {
      _input[i] = input_p[i];
    }
  }

  virtual void predict() {
    // call the NN HLS function to make predictions
    L1TMLDemo_v1(_input, _result);
  }
  
  virtual void read_result(std::any result) {
    // copy result
    std::array<result_t, N_LAYER_6> *result_p = std::any_cast<std::array<result_t, N_LAYER_6>*>(result);
    for (int i = 0; i < N_LAYER_6; i++) {
      result_p-[i] = _result[i];
    }
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
```

Note the function call `L1TMLDemo_v1(_input, _result);` that directly calls the HLS function of our hls4ml NN to run the emulation.

### emulator Makefile

A Makefile is required to compile the NN emulator. `part3/cms-hls4ml/L1TMLDemo/Makefile` and `part3/cms-hls4ml/L1TMLDemo/L1TMLDemo_v1/Makefile` are the Makefiles for our example model that can be used as a template for your real developments. This defines which `.cpp` files to compile, and also sets the include and library path.

The main product that we will use from our CMSSW example is the shared object library file `L1TMLDemo_v1.so`

In order to compile the `.so` locally, we need to clone the `hls4mlEmulatorExtras` and `hls` repositories. This is done when you `source setup.sh` for this tutorial.

### CMSSW

Using our NN emulator from CMSSW is relatively simple. In this example we create a simple analyzer that consumes some collections from the event, runs the emulator, and writes the NN output to a ROOT file. A real development might do more, especially if the NN is just one part of an algorithm.

#### Producer / Analyzer

In our usual producer or analyzer, we need to initialize and load the model in the producer/analyzer constructor. The analyzer for this section is defined in `part3/cmssw/src/L1Trigger/L1TMLDemo/plugins/l1tDemoMLAnalyzer.cc`. Below is a simplified `EDProducer` just highlighting the NN emulation specific code:

```c++
#include "hls4ml/emulator.h"

class MyProducer : public edm::EDProducer {
public:
 ... 

private:
  std::shared_ptr<hls4mlEmulator::Model> model;

MyProducer::MyProducer(const edm::ParameterSet& config){
  std::string model_so_path = config.getParameter<std::string>("model_so_path");
  hls4mlEmulator::ModelLoader loader(model_so_path);
  model = loader.load_model();
}

}; // class MyProducer
```

#### produce / analyze

Next, from the `produce` or `analyze` method that runs for each event, we need to consume some collections from the event and prepare the NN inputs, then run the emulated inference. In our example analyzer we write the result to a ROOT file, but a real development might do more, like adding the NN prediction to a product.

*Note* : the data type (some `ap_fixed<W,I,TRN,SAT>`) needs to match what is defined in the NN. A mismatch here will compile, but result in a runtime error!

```c++
void MyProducer::produce(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  ap_fixed<W0,I0> X[N]; // array for NN input data
  ap_fixed<W1,I1> y;    // variable for binary classifier NN output

  ... get data from iEvent and prepare the NN inputs as required
  
  // use the emulator
  model->prepare_input(X); // set the input to the emulator
  model->predict();        // run the emulated inference
  model->read_result(&y);  // get the output from the emulator

  ... do something with the result
}
```

## FPGA Deployment