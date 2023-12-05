
## Prerequisite

You will need the HLS for the model of part 2.

## 1.

Copy the NN-specific part of the hls4ml project to the `cms-hls4ml` repo. We _don't_ copy `ap_types` since we'll reference them from the externals.

```shell
[ ! -d part3/cms-hls4ml/L1TMLDemo/L1TMLDemo_v1/NN ] && mkdir part3/cms-hls4ml/L1TMLDemo/L1TMLDemo_v1/NN
cp -r L1TMLDemo/firmware/{*.h,*.cpp,weights,nnet_utils} part3/cms-hls4ml/L1TMLDemo/L1TMLDemo_v1/NN
```

## 2.

As of `hls4ml` `0.8.1`, when run outside of Vivado HLS, the C++ code loads the weights from txt files. We need to force compilation of the weights from the header file instead. 

This one liner will replace the `#define` that would cause the weights to be loaded from txt files with one that will load them from the header files when we compile instead.

```shell
find part3/cms-hls4ml/L1TMLDemo/L1TMLDemo_v1/NN \( -type d -name .git -prune \) -o -type f -print0 | xargs -0 sed -i 's/#ifndef __SYNTHESIS__/#ifdef __HLS4ML_LOAD_TXT_WEIGHTS__/'
```

## 3.

`make` the hls4ml emulator interface shared object

```shell
cd part3/cms-hls4ml/hls4mlEmulatorExtras
make
mkdir lib64
mv libemulator_interface.so lib64
```

## 4.

`make` the `L1TMLDemo` model shared object

```shell
cd part3/cms-hls4ml/L1TMLDemo
make
```

## 5.

CMSSW emulator...