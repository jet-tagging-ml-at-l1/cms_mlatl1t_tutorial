#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# create CMSSW area if it doesn't exist, and cmsrel
[ ! -d CMSSW_13_3_0_pre3 ] && cmsrel CMSSW_13_3_0_pre3
cd CMSSW_13_3_0_pre3
cmsenv
cd $SCRIPT_DIR

# put the HLS tools on the PATH
export PATH=$SCRIPT_DIR/bin:$PATH