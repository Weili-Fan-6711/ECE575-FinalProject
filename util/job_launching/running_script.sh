#!/bin/bash

# For running this script, the configurations to be running have to be provided, and optionally a name to track the status of the jobs while running
# if no name is provided, a default name "test_$configuration" will be used

# For No security         -->   ./running_script N chosen_name 
# For Baseline security   -->   ./running_script B chosen_name
# For Salus               -->   ./running_script S chosen name

configuration=$1

if test "$configuration" = "N" 
then
# For System with No Security

name=${2:-"test_no_security"}

python3 ./run_simulations.py -B rodinia-3.1_v1 -C QV100-2B_INSN-V1 -N $name
python3 ./run_simulations.py -B parboil -C QV100-2B_INSN-V1 -N $name
python3 ./run_simulations.py -B pannotia_v1 -C QV100-2B_INSN-V1 -N $name
python3 ./run_simulations.py -B lonestargpu-2.0_v1 -C QV100-2B_INSN-V1 -N $name

python3 ./run_simulations.py -B rodinia-3.1_v2 -C QV100-2B_INSN-V2 -N $name
python3 ./run_simulations.py -B pannotia_v2 -C QV100-2B_INSN-V2 -N $name
python3 ./run_simulations.py -B lonestargpu-2.0_v2 -C QV100-2B_INSN-V2 -N $name

python3 ./run_simulations.py -B rodinia-3.1_v3 -C QV100-2B_INSN-V3 -N $name
python3 ./run_simulations.py -B lonestargpu-2.0_v3 -C QV100-2B_INSN-V3 -N $name

elif test "$configuration" = "B"
then

name=${2:-"test_baseline"}	

# For Baseline Security

python3 ./run_simulations.py -B rodinia-3.1_v1 -C QV100-2B_INSN-V1-BASELINE -N $name
python3 ./run_simulations.py -B parboil -C QV100-2B_INSN-V1-BASELINE -N $name
python3 ./run_simulations.py -B pannotia_v1 -C QV100-2B_INSN-V1-BASELINE -N $name
python3 ./run_simulations.py -B lonestargpu-2.0_v1 -C QV100-2B_INSN-V1-BASELINE -N $name

python3 ./run_simulations.py -B rodinia-3.1_v2 -C QV100-2B_INSN-V2-BASELINE -N $name
python3 ./run_simulations.py -B pannotia_v2 -C QV100-2B_INSN-V2-BASELINE -N $name
python3 ./run_simulations.py -B lonestargpu-2.0_v2 -C QV100-2B_INSN-V2-BASELINE -N $name

python3 ./run_simulations.py -B rodinia-3.1_v3 -C QV100-2B_INSN-V3-BASELINE -N $name
python3 ./run_simulations.py -B lonestargpu-2.0_v3 -C QV100-2B_INSN-V3-BASELINE -N $name

elif test "$configuration" = "S"
then

name=${2:-"test_salus"}

# For Salus

python3 ./run_simulations.py -B rodinia-3.1_v1 -C QV100-2B_INSN-V1-SALUS -N $name
python3 ./run_simulations.py -B parboil -C QV100-2B_INSN-V1-SALUS -N $name
python3 ./run_simulations.py -B pannotia_v1 -C QV100-2B_INSN-V1-SALUS -N $name
python3 ./run_simulations.py -B lonestargpu-2.0_v1 -C QV100-2B_INSN-V1-SALUS -N $name

python3 ./run_simulations.py -B rodinia-3.1_v2 -C QV100-2B_INSN-V2-SALUS -N $name
python3 ./run_simulations.py -B pannotia_v2 -C QV100-2B_INSN-V2-SALUS -N $name
python3 ./run_simulations.py -B lonestargpu-2.0_v2 -C QV100-2B_INSN-V2-SALUS -N $name

python3 ./run_simulations.py -B rodinia-3.1_v3 -C QV100-2B_INSN-V3-SALUS -N $name
python3 ./run_simulations.py -B lonestargpu-2.0_v3 -C QV100-2B_INSN-V3-SALUS -N $name
fi
