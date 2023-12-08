// FWCore includes
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

// File writing includes
#include "DataFormats/NanoAOD/interface/FlatTable.h"

// L1T includes
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"
#include "DataFormats/L1Trigger/interface/Tau.h"
#include "DataFormats/L1Trigger/interface/Jet.h"
#include "DataFormats/L1Trigger/interface/EtSum.h"

#include "L1Trigger/DemonstratorTools/interface/BoardDataWriter.h"

// hls & hls4ml includes
#include "ap_fixed.h"
#include "hls4ml/emulator.h"

#include <iostream>

class L1TMLDemoPatternWriter : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit L1TMLDemoPatternWriter(const edm::ParameterSet& cfg);
  ~L1TMLDemoPatternWriter();

private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() override;
  void pack_inputs(ap_fixed<14,13>* X_unscaled);
  void pack_outputs(ap_fixed<13,2,AP_RND,AP_SAT> y);

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

  bool write_patterns;
  //l1t::demo::BoardDataWriter inFileWriter;
  std::map<l1t::demo::LinkId, std::pair<l1t::demo::ChannelSpec, std::vector<size_t>>> outChannelSpec;

  l1t::demo::BoardDataWriter outFileWriter;

};

L1TMLDemoPatternWriter::L1TMLDemoPatternWriter(const edm::ParameterSet& cfg)
 : outChannelSpec{{{"y", 0}, {{1,0}, {0}}}}, 
   outFileWriter(l1t::demo::FileFormat::EMPv2,        // pattern file format
                 "L1TMLDemoPatterns_out",             // file name
                 "txt",                               // file extension
                 1,                                   // frames per BX
                 1,                                   // TMUX 
                 1024,                                // max lines per file
                 outChannelSpec)
 {
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

  // configure the board writer
  write_patterns = cfg.getParameter<bool>("write_patterns");
                                       
}

L1TMLDemoPatternWriter::~L1TMLDemoPatternWriter(){
}

void L1TMLDemoPatternWriter::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)  {
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

  // write the patterns
  if(write_patterns){
    pack_outputs(y);
  }

}

void L1TMLDemoPatternWriter::beginJob(){
}

void L1TMLDemoPatternWriter::endJob(){
  outFileWriter.flush();
}

void L1TMLDemoPatternWriter::pack_inputs(ap_fixed<14,13>* X_unscaled) {

}

void L1TMLDemoPatternWriter::pack_outputs(ap_fixed<13,2,AP_RND,AP_SAT> y) {
  ap_uint<64> bits = 0;
  bits(12,0) = y(12,0); // copy the bits not the value
  std::vector<ap_uint<64>> bits_v;
  bits_v.push_back(bits);
  l1t::demo::EventData eventDataOut;
  eventDataOut.add({"y", 0}, bits_v);
  outFileWriter.addEvent(eventDataOut);
}

DEFINE_FWK_MODULE(L1TMLDemoPatternWriter);
