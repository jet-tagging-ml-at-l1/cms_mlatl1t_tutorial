<img src="https://indico.cern.ch/event/1301711/attachments/2690003/4738925/ml@l1t_banner_small.png">

# CMS Machine Learning at Level 1 Trigger Tutorial

## Setup

This tutorial is designed to run on `lxplus8` using a Python environment from CMSSW. In addition we install some extra Python packages, and we use Xilinx's Vivado HLS software from a container on /cvmfs. This tutorial is designed to get you working on the notebooks with minimal setup. For real ML@L1T development work we recommend using a high-memory, high single-core-performance PC with the latest Xilinx tools.

**Note** with the recent switch to `lxplus9` as the alias for `lxplus`, you need to specify `lxplus8` to run this tutorial!

To set up the environment and launch the jupyter notebooks:

From your laptop, open a terminal and use your CERN computing account to `ssh` to `lxplus8`, e.g.:

```
ssh <user>@lxplus8.cern.ch
```

Note which `lxplus8` node your are connected to, e.g. `lxplus812.cern.ch`, we will need to refer to the exact server in a later step.
From the terminal on `lxplus8`, first clone this repository

```
git clone https://gitlab.cern.ch/fastmachinelearning/cms_mlatl1t_tutorial
cd cms_mlatl1t_tutorial
```

Setup the software environment and launch the Jupyter Hub with:

```
bash start_notebooks.sh # for bash
# source start_notebooks.sh # for zsh
```

Take note of the output:

```
    Or copy and paste one of these URLs:
        http://localhost:8888/?token=<long string>
     or http://127.0.0.1:8888/?token=<long string>
```

The port may or may not be `8888`, depending on other users of the machine. Note which port your are given.

Then from another terminal on your laptop, forward the port on lxplus to your laptop like this:

```
ssh -N -f -L <local port>:localhost:<lxplus port> <user>@lxplus8<node>.cern.ch
```

e.g.

```
ssh -N -f -L 8888:localhost:8888 <user>@lxplus812.cern.ch
```

You made need to change the local port (left hand side) if you see a message like this:
```
bind [127.0.0.1]:8888: Address already in use
channel_setup_fwd_listener_tcpip: cannot listen to port: 8888
Could not request local forwarding.
```

You made need to change the remote port (right hand side) if the output from `bash start_notebooks.sh` showed a different port number (use the one printed there).

## Authors and acknowledgment
@ssummers @thaarres @alobanov
