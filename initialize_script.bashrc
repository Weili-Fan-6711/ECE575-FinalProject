#! /bin/sh
export CUDA_INSTALL_PATH=/usr/local/cuda-11.8
export PATH="$HOME/.local/bin:/usr/bin:/bin:$CUDA_INSTALL_PATH/bin:$PATH"
export CUOBJDUMP_SIM_FILE=on
export GENCODE_FLAGS=-lcudart
