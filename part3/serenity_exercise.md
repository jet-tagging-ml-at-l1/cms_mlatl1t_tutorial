# Building Phase 2 FPGA bitfile with NN

We will target a Serenity board, one of the CMS Phase 2 L1T boards. While some of the configuration is specific to the Serenity, some other Phase 2 L1T boards also use the `emp-fwk` and the same workflow.
Other boards like APx have a different workflow, but the concepts are similar.

This part of the tutorial will take too long to run through in the time available during the live session. 
The FPGA part used by the Serenity board is not installed on the Vivado installation we're using from `lxplus` either. Instead we will show a demonstration of the results. In general running synthesis and implementation for the Virtex Ultrascale+ FPGAs that will be used in the CMS Phase 2 L1T can take several hours or even days, and benefits from access to high single-core performance, high memory machines rather than shared resources like `lxplus`. We also recommend that you use more recent versions of Vivado than are available on the image used for the tutorial.

## Prerequisites
Accessing some of the repositories required for building firmware requires permissions. Join the `cms-cactus` and `cms-tcds2-users` e-groups before starting, and allow some hours for the synchronisation of the permissions databases.

You also need to have synthesized the hls4ml NN from part 2.

## 1. Create Standard Scaler HLS IP

We will deploy our NN as one module in the 'payload', and the Standard Scaler as a separate module. We've provided a Python script to read the values from the Standard Scaler and write some valid HLS code to do the scaling.

The VHDL interface of the hls4ml NN is also not very nice, the X data port is 'flattened' over the inputs, resulting in a 56 * 16 bit = 896 bit wide vector like this:

```vhdl
input_1_V : IN STD_LOGIC_VECTOR (895 downto 0);
```

HLS gives us control over this interface, so we will also use the Standard Scaler HLS as an 'adapter' from an array of 56 inputs to this 896 bit wide input.

This could also be achieved in VHDL with a `generate` statement, or by modifying the `#pragma hls interface` settings of the hls4ml function.

```shell
cd $MLATL1T_DIR/part3
python make_scaler_hls.py
```

This creates the file `$MLATL1T_DIR/part3/Scaler/firmware/hls/scales.h` with contents like:

```c++
#ifndef L1TMLDEMO_SCALES_H_
#define L1TMLDEMO_SCALES_H_
#include "defines.h"

static const scale_t scale[N_INPUTS] = {... scale values};
static const bias_t bias[N_INPUTS] = {... bias values};

#endif
```

Then we need to synthesize the scaler HLS:

```shell
cd $MLATL1T_DIR/part3/Scaler/firmware/hls
vivado_hls -f synth.tcl
```

## 2. Setup IPBus Builder workspace

This will clone several Gitlab repositories to the directory `$MLATL1T_DIR/part3/p2fwk-work/`

```shell
bash setup_fw_workspace.sh
```

After completion the output of `tree $MLATL1T_DIR/part3/p2fwk-work/ -L 2` should be:

```shell
$ tree . -L 2
$MLATL1T_DIR/part3/p2fwk-work/
├── proj
└── src
    ├── cms-tcds2-firmware
    ├── emp-fwk
    ├── ipbus-firmware
    ├── L1TMLDemo
    ├── legacy_ttc
    ├── slinkrocket
    ├── slinkrocket_ips
    └── tclink
```

## 3. Create `ipbb` project

```shell
cd $MLATL1T_DIR/part3/p2fwk-work/
cp src/emp-fwk/components/ttc/firmware/hdl/ipbus_decode_ipbus_tcds2_interface_accessor.vhd src/cms-tcds2-firmware/components/tcds2_interface/firmware/hdl/
ipbb proj create vivado L1TMLDemo L1TMLDemo:payload top_serenity.dep
#ipbb ipbus gendecoders
cd proj/L1TMLDemo
ipbb vivado generate-project --single
```

## 4. Build `ipbb` project

Launch the synthesis and implementation, this will take a few hours!

```shell
ipbb vivado synth -j8 impl -j8
ipbb vivado package
```

## 5. Make a pattern file
Format some hardware input data to hex-formatted columnar file representing data on optical links.
This file will be loaded into buffers (BRAMs) next to the transceiver in the FPGA fabric, mimiccing data arriving from CMS or another L1T board.

## 6. Run on Serenity