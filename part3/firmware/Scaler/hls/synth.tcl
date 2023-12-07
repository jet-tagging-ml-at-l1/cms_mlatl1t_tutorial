open_project -reset scaler_prj
set_top scaler
add_files scaler.cpp
open_solution -reset "solution1"
set_part xcvu13p-flga2577-2-e
create_clock -period 2.5 -name default
csynth_design
quit