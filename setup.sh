#!/bin/bash

CMSSW_VERSION=CMSSW_13_3_0_pre3

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export MLATL1T_DIR=$SCRIPT_DIR

# create CMSSW area if it doesn't exist, and cmsenv
[ ! -d CMSSW_13_3_0_pre3 ] && echo "ML@L1T Setup: cmsrel $CMSSW_VERSION" && \
  cmsrel $CMSSW_VERSION
cd $CMSSW_VERSION/src
cmsenv
[ ! -d L1Trigger ] && mkdir L1Trigger # instead of git cms-addpkg, since we don't actually add any packages from github
[ ! -d $SCRIPT_DIR/$CMSSW_VERSION/src/L1Trigger/L1TMLDemo ] && \
  echo "ML@L1T Setup: linking src/L1Trigger/L1TMLDemo" && \
  ln -s $SCRIPT_DIR/part3/cmssw/src/L1Trigger/L1TMLDemo $SCRIPT_DIR/$CMSSW_VERSION/src/L1Trigger

# clone main cms-hls4ml external repositories for standalone compilation of emulator for part 3
cd $SCRIPT_DIR/part3/cms-hls4ml
[ ! -d hls4mlEmulatorExtras ] && echo "ML@L1T Setup: cloning cms-hls4ml/hls4mlEmulatorExtras" && \
  git clone --quiet https://github.com/cms-hls4ml/hls4mlEmulatorExtras.git && \
  cd hls4mlEmulatorExtras &&
  git checkout -b v1.1.1 tags/v1.1.1 \
  && cd ..
[ ! -d hls ] && echo "ML@L1T Setup: cloning Xilinx/HLS_arbitrary_Precision_Types" && \
  git clone --quiet https://github.com/Xilinx/HLS_arbitrary_Precision_Types.git hls

cd $SCRIPT_DIR

# put the HLS tools on the PATH
echo "ML@L1T Setup: prepending $SCRIPT_DIR/bin to PATH"
export PATH=$SCRIPT_DIR/bin:$PATH

# install pyarrow for parquet file handling
pip3 install pyarrow