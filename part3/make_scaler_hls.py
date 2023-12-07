import pickle
import os

# read the standard scaler from part 1 and write the values to a C array for the HLS module

scaler_file = os.environ['MLATL1T_DIR'] + '/part1_outputs/hwScaler.pkl'
scaler = pickle.load(open(scaler_file, 'rb'))
scales = 1. / scaler.scale_
bias = scaler.mean_

template = '''
#ifndef L1TMLDEMO_SCALES_H_
#define L1TMLDEMO_SCALES_H_
#include "defines.h"

static const scale_t scale[N_INPUTS] = {{{}}};
static const bias_t bias[N_INPUTS] = {{{}}};

#endif
'''

with open(os.environ['MLATL1T_DIR'] + '/part3/firmware/Scaler/hls/scales.h', 'w') as f:
  scales_txt = ','.join(str(s) for s in scales)
  bias_txt = ','.join(str(b) for b in bias)
  header = template.format(scales_txt, bias_txt)
  f.write(header)