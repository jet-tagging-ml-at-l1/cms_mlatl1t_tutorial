pip install https://github.com/ipbus/ipbb/archive/dev/2023a.tar.gz
ipbb init $MLATL1T_DIR/part3/p2fwk-work
cd $MLATL1T_DIR/part3/p2fwk-work
# these are the framework packages required
ipbb add git https://gitlab.cern.ch/p2-xware/firmware/emp-fwk.git
ipbb add git https://gitlab.cern.ch/ttc/legacy_ttc.git -b v2.1
ipbb add git https://gitlab.cern.ch/cms-tcds/cms-tcds2-firmware.git -b v0_1_1
ipbb add git https://gitlab.cern.ch/HPTD/tclink.git -r fda0bcf
ipbb add git https://gitlab.cern.ch/dth_p1-v2/slinkrocket_ips.git -b v03.12
ipbb add git https://gitlab.cern.ch/dth_p1-v2/slinkrocket.git -b v03.12
ipbb add git https://github.com/ipbus/ipbus-firmware -b v1.9

# this is our project code
mkdir $MLATL1T_DIR/part3/p2fwk-work/src/L1TMLDemo
ln -rs $MLATL1T_DIR/part3/firmware/ $MLATL1T_DIR/part3/p2fwk-work/src/L1TMLDemo

# copy the synthesized Neural Network VHDL to the ipbb workspace
mkdir -p $MLATL1T_DIR/part3/p2fwk-work/src/L1TMLDemo/NN/firmware/hdl
mkdir -p $MLATL1T_DIR/part3/p2fwk-work/src/L1TMLDemo/NN/firmware/cfg
cp $MLATL1T_DIR/part2_outputs/L1TMLDemo_v1/L1TMLDemo_v1_prj/solution1/syn/vhdl/* $MLATL1T_DIR/part3/p2fwk-work/src/L1TMLDemo/NN/firmware/hdl/
# make an ipbb dependency file
for f in `ls $MLATL1T_DIR/part2_outputs/L1TMLDemo_v1/L1TMLDemo_v1_prj/solution1/syn/vhdl/`
do
  echo "src -l NNLib $f" >> $MLATL1T_DIR/part3/p2fwk-work/src/L1TMLDemo/NN/firmware/cfg/nn.dep
done

# copy thr synthesized Standard Scaler VHDL to the ipbb workspace
mkdir -p $MLATL1T_DIR/part3/p2fwk-work/src/L1TMLDemo/Scaler/firmware/hdl
mkdir -p $MLATL1T_DIR/part3/p2fwk-work/src/L1TMLDemo/Scaler/firmware/cfg
cp $MLATL1T_DIR/part3/firmware/Scaler/hls/scaler_prj/solution1/syn/vhdl/* $MLATL1T_DIR/part3/p2fwk-work/src/L1TMLDemo/Scaler/firmware/hdl/
for f in `ls $MLATL1T_DIR/part3/firmware/Scaler/hls/scaler_prj/solution1/syn/vhdl`
do
  echo "src $f" >> $MLATL1T_DIR/part3/p2fwk-work/src/L1TMLDemo/Scaler/firmware/cfg/scaler.dep
done