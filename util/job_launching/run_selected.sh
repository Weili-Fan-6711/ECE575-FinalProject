#!/bin/bash

set -euo pipefail

usage() {
    cat <<'EOF'
Usage:
  ./run_selected.sh -B <benchmarks> -C <configs> [-N launch_name] [-r run_dir] [-l launcher] [-c cores]
  ./run_selected.sh -Y <selection.yml> [-B <benchmarks>] [-C <configs>] [-N launch_name] [-r run_dir] [-l launcher] [-c cores]

Examples:
  ./run_selected.sh \
    -B rodinia-3.1:backprop-rodinia-3.1 \
    -C QV100-2B_INSN \
    -N backprop-local \
    -l local \
    -c 1 \
    -r sim_run_11.8_backprop_local

  ./run_selected.sh \
    -B rodinia-3.1:backprop-rodinia-3.1,rodinia-3.1:srad_v2-rodinia-3.1 \
    -C QV100-2B_INSN \
    -N selected-sbatch \
    -l sbatch \
    -r sim_run_11.8_selected_sbatch

  ./run_selected.sh \
    -Y selected_jobs.yml

YAML format:
  benchmarks:
    - rodinia-3.1:backprop-rodinia-3.1
    - rodinia-3.1:bfs-rodinia-3.1
  configs:
    - QV100-2B_INSN
  launch_name: my-local-run
  run_dir: sim_run_11.8_my_local_run
  launcher: local
  cores: 2
EOF
}

benchmark_list=""
configs_list=""
launch_name="selected-run"
run_dir=""
launcher="local"
cores=""
selection_yml=""

load_selection_yml() {
    local yml_path="$1"
    python3 - "$yml_path" <<'PY'
import sys
import yaml

path = sys.argv[1]
with open(path, "r", encoding="utf-8") as f:
    data = yaml.load(f, Loader=yaml.FullLoader)

if not isinstance(data, dict):
    raise SystemExit("Selection YAML must contain a top-level mapping")

def normalize_list(value, field):
    if value is None:
        return ""
    if isinstance(value, str):
        return value
    if isinstance(value, list):
        return ",".join(str(item).strip() for item in value if str(item).strip())
    raise SystemExit(f"'{field}' must be a string or a list")

for key in ("benchmark_list", "benchmarks", "configs_list", "configs", "launch_name", "run_dir", "launcher", "cores"):
    value = data.get(key, "")
    if key in ("benchmarks", "configs", "benchmark_list", "configs_list"):
        value = normalize_list(value, key)
    elif value is None:
        value = ""
    else:
        value = str(value).strip()
    print(f"{key}={value}")
PY
}

while getopts ":B:C:N:r:l:c:Y:h" opt; do
    case "$opt" in
        B) benchmark_list="$OPTARG" ;;
        C) configs_list="$OPTARG" ;;
        N) launch_name="$OPTARG" ;;
        r) run_dir="$OPTARG" ;;
        l) launcher="$OPTARG" ;;
        c) cores="$OPTARG" ;;
        Y) selection_yml="$OPTARG" ;;
        h)
            usage
            exit 0
            ;;
        :)
            echo "Missing argument for -$OPTARG" >&2
            usage >&2
            exit 1
            ;;
        \?)
            echo "Unknown option: -$OPTARG" >&2
            usage >&2
            exit 1
            ;;
    esac
done

if [[ -n "$selection_yml" ]]; then
    if [[ ! -f "$selection_yml" ]]; then
        echo "Selection YAML not found: $selection_yml" >&2
        exit 1
    fi

    while IFS='=' read -r key value; do
        case "$key" in
            benchmark_list)
                if [[ -z "$benchmark_list" ]]; then
                    benchmark_list="$value"
                fi
                ;;
            benchmarks)
                if [[ -z "$benchmark_list" ]]; then
                    benchmark_list="$value"
                fi
                ;;
            configs_list)
                if [[ -z "$configs_list" ]]; then
                    configs_list="$value"
                fi
                ;;
            configs)
                if [[ -z "$configs_list" ]]; then
                    configs_list="$value"
                fi
                ;;
            launch_name)
                if [[ "$launch_name" == "selected-run" && -n "$value" ]]; then
                    launch_name="$value"
                fi
                ;;
            run_dir)
                if [[ -z "$run_dir" ]]; then
                    run_dir="$value"
                fi
                ;;
            launcher)
                if [[ "$launcher" == "local" && -n "$value" ]]; then
                    launcher="$value"
                fi
                ;;
            cores)
                if [[ -z "$cores" ]]; then
                    cores="$value"
                fi
                ;;
        esac
    done < <(load_selection_yml "$selection_yml")
fi

if [[ -z "$benchmark_list" || -z "$configs_list" ]]; then
    echo "Both benchmark list and config list are required. Use -B/-C or -Y." >&2
    usage >&2
    exit 1
fi

cmd=(python3 ./run_simulations.py
    -B "$benchmark_list"
    -C "$configs_list"
    -N "$launch_name"
    -l "$launcher")

if [[ -n "$run_dir" ]]; then
    cmd+=(-r "$run_dir")
fi

if [[ -n "$cores" ]]; then
    cmd+=(-c "$cores")
fi

"${cmd[@]}"
