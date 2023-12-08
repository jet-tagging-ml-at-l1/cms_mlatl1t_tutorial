// FWCore includes
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

// File writing includes
#include "DataFormats/NanoAOD/interface/FlatTable.h"
//#include "TTree.h"
//#include "FWCore/ServiceRegistry/interface/Service.h"
//#include "CommonTools/UtilAlgos/interface/TFileService.h"

// L1T includes
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"
#include "DataFormats/L1Trigger/interface/Tau.h"
#include "DataFormats/L1Trigger/interface/Jet.h"
#include "DataFormats/L1Trigger/interface/EtSum.h"

// hls & hls4ml includes
#include "ap_fixed.h"
#include "hls4ml/emulator.h"

#include <iostream>

class L1TMLDemoProducer : public edm::global::EDProducer<> {
public:
  explicit L1TMLDemoProducer(const edm::ParameterSet& cfg);
  ~L1TMLDemoProducer();

private:
  virtual void beginJob() override;
  virtual void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override;
  virtual void endJob() override;

  edm::EDGetToken muToken;
  edm::EDGetToken egToken;
  edm::EDGetToken tauToken;
  edm::EDGetToken jetToken;
  edm::EDGetToken sumToken;

  unsigned nMu;
  unsigned nEG;
  unsigned nTau;
  unsigned nJet;
  unsigned nNNIn;

  typedef ap_fixed<16,6,AP_RND_CONV,AP_SAT> scale_t;
  typedef ap_fixed<16,6,AP_RND_CONV,AP_SAT> bias_t;
  // hls4ml emulator model path
  std::string model_so_path;
  std::vector<scale_t> scale;
  std::vector<bias_t> bias;

};

L1TMLDemoProducer::L1TMLDemoProducer(const edm::ParameterSet& cfg){
  // consume
  muToken = consumes<l1t::MuonBxCollection>(cfg.getParameter<edm::InputTag>("muToken"));
  egToken = consumes<l1t::EGammaBxCollection>(cfg.getParameter<edm::InputTag>("egToken"));
  tauToken = consumes<l1t::TauBxCollection>(cfg.getParameter<edm::InputTag>("tauToken"));
  jetToken = consumes<l1t::JetBxCollection>(cfg.getParameter<edm::InputTag>("jetToken"));
  sumToken = consumes<l1t::EtSumBxCollection>(cfg.getParameter<edm::InputTag>("etSumToken"));
  nMu = cfg.getParameter<unsigned>("nMu");
  nEG = cfg.getParameter<unsigned>("nEg");
  nTau = cfg.getParameter<unsigned>("nTau");
  nJet = cfg.getParameter<unsigned>("nJet");
  // total number of inputs to NN
  nNNIn = 2 + 3 * (nMu + nEG + nTau + nJet);

  // store the path to the .so file
  model_so_path = cfg.getParameter<std::string>("model_so_path");

  // get the scaler parameters and cast them to fixed point types
  std::vector<double> scale_double = cfg.getParameter<std::vector<double>>("scale");
  std::transform(scale_double.begin(), scale_double.end(), std::back_inserter(scale), [](double s){ return (scale_t)s; });
  // get the bias parameters and cast them to fixed point types
  std::vector<double> bias_double = cfg.getParameter<std::vector<double>>("bias");
  std::transform(bias_double.begin(), bias_double.end(), std::back_inserter(bias), [](double s){ return (bias_t)s; });

  // produce
  produces<nanoaod::FlatTable>("L1TMLDemo");

}

L1TMLDemoProducer::~L1TMLDemoProducer(){
}

void L1TMLDemoProducer::produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  using namespace edm;
  // get input collections
  // BXVector: first index is BX, second index is object
  edm::Handle<BXVector<l1t::Muon>> muons;
  edm::Handle<BXVector<l1t::EGamma>> egammas;
  edm::Handle<BXVector<l1t::Tau>> taus;
  edm::Handle<BXVector<l1t::Jet>> jets;
  edm::Handle<BXVector<l1t::EtSum>> sums;
  iEvent.getByToken(muToken, muons);
  iEvent.getByToken(egToken, egammas);
  iEvent.getByToken(tauToken, taus);
  iEvent.getByToken(jetToken, jets);
  iEvent.getByToken(sumToken, sums);

  // The unscaled inputs are hwInts
  // ap_fixed<14,13> is wide enough for all the ET, pT, eta, phi
  ap_fixed<14,13>* X_unscaled = new ap_fixed<14,13>[nNNIn];
  // initialize to zeros
  for(unsigned i = 0; i < nNNIn; i++){
    X_unscaled[i] = 0;
  }

  // fill the inputs
  unsigned ix = 0;
  // sums first, just find the MET
  for(unsigned i = 0; i < sums->size(0); i++){
    if(sums->at(0, i).getType() == l1t::EtSum::EtSumType::kMissingEt){
      X_unscaled[ix++] = sums->at(0,i).hwPt();
      X_unscaled[ix++] = sums->at(0,i).hwPhi();
    }
  }
  // jets next
  ix = 2 * ( 1 );
  for(unsigned i = 0; i < std::min(nJet, jets->size(0)); i++){
    X_unscaled[ix++] = jets->at(0, i).hwPt();
    X_unscaled[ix++] = jets->at(0, i).hwEta();
    X_unscaled[ix++] = jets->at(0, i).hwPhi();
  }
  // egammas next
  ix = 2 * ( 1 + nJet );
  for(unsigned i = 0; i < std::min(nEG, egammas->size(0)); i++){
    X_unscaled[ix++] = egammas->at(0, i).hwPt();
    X_unscaled[ix++] = egammas->at(0, i).hwEta();
    X_unscaled[ix++] = egammas->at(0, i).hwPhi();
  }
  // muons next
  ix = 2 * ( 1 + nJet + nEG );
  for(unsigned i = 0; i < std::min(nMu, muons->size(0)); i++){
    X_unscaled[ix++] = muons->at(0, i).hwPt();
    X_unscaled[ix++] = muons->at(0, i).hwEta();
    X_unscaled[ix++] = muons->at(0, i).hwPhi();
  }
  // taus next
  ix = 2 * ( 1 + nJet + nEG + nMu );
  for(unsigned i = 0; i < std::min(nTau, taus->size(0)); i++){
    X_unscaled[ix++] = taus->at(0, i).hwPt();
    X_unscaled[ix++] = taus->at(0, i).hwEta();
    X_unscaled[ix++] = taus->at(0, i).hwPhi();
  }

  ap_fixed<16,7,AP_RND,AP_SAT>* X_scaled = new ap_fixed<16,7,AP_RND,AP_SAT>[nNNIn];
  // scale the inputs
  for(unsigned i = 0; i < nNNIn; i++){
    X_scaled[i] = (X_unscaled[i] - bias[i]) * scale[i];
    //std::cout << X_scaled[i] << ",";
  }
  //std::cout << std::endl;

  // load the NN emulator object
  hls4mlEmulator::ModelLoader loader(model_so_path);
  std::shared_ptr<hls4mlEmulator::Model> model = loader.load_model();

  ap_fixed<13,2,AP_RND,AP_SAT> y; // output object

  // run the actual inference
  model->prepare_input(X_scaled);
  model->predict();
  model->read_result(&y);

  // write the result to the output
  // note cast from the ap_fixed emulated type to float for convenience
  auto out = std::make_unique<nanoaod::FlatTable>(1, "L1TMLDemo", false);
  std::vector<float> y_vec;
  y_vec.push_back(y);
  out->addColumn<float>("y", y_vec, "model prediction");
  iEvent.put(std::move(out), "L1TMLDemo");

}

void L1TMLDemoProducer::beginJob(){
}

void L1TMLDemoProducer::endJob(){
}

DEFINE_FWK_MODULE(L1TMLDemoProducer);
