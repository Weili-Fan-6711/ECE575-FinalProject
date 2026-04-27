#! /bin/sh
SCRIPT_PATH="${BASH_SOURCE[0]:-$0}"
SCRIPT_DIR="$(cd "$(dirname "$SCRIPT_PATH")" && pwd)"
PROJECT_CUDA_INSTALL_PATH="$SCRIPT_DIR/.conda/envs/cuda-11.8.0-local"
PROJECT_CUDA_LEGACY_PATH="$SCRIPT_DIR/.conda/envs/cuda-11.8.0"

if [ -d "$PROJECT_CUDA_INSTALL_PATH" ]; then
	export CUDA_INSTALL_PATH="$PROJECT_CUDA_INSTALL_PATH"
elif [ -d "$PROJECT_CUDA_LEGACY_PATH" ]; then
	export CUDA_INSTALL_PATH="$PROJECT_CUDA_LEGACY_PATH"
else
	export CUDA_INSTALL_PATH="${CUDA_INSTALL_PATH:-/usr/local/cuda-11.8}"
fi

export PATH="$HOME/.local/bin:/usr/bin:/bin:$CUDA_INSTALL_PATH/bin:$PATH"
export CUOBJDUMP_SIM_FILE=on
export GENCODE_FLAGS=-lcudart
