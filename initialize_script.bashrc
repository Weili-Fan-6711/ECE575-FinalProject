#! /bin/sh
export CUDA_INSTALL_PATH=/usr/local/cuda
export PATH=$CUDA_INSTALL_PATH/bin
export PATH="/usr/bin:$PATH"
export CUOBJDUMP_SIM_FILE=on
export GENCODE_FLAGS=-lcudart
export PATH="/bin:$PATH"
