# Minimal Build And Run Commands

## 1. Build `gpgpu-sim`

```bash
cd /anvil/projects/x-nairr250138/sitao/gpu/ECE575-FinalProject
source setup_environment
make -j8
```

## 2. Build `gpu-app-collection`

```bash
cd /anvil/projects/x-nairr250138/sitao/gpu/ECE575-FinalProject
source setup_environment
cd gpu-app-collection
source ./src/setup_environment
make all -i -j8 -C ./src
```

## 3. Download benchmark data

```bash
cd /anvil/projects/x-nairr250138/sitao/gpu/ECE575-FinalProject
source setup_environment
cd gpu-app-collection
source ./src/setup_environment
make data -C ./src
```

## 4. Run a hand-picked list of benchmarks

Go to the launcher directory:

```bash
cd /anvil/projects/x-nairr250138/sitao/gpu/ECE575-FinalProject
source setup_environment
cd gpu-app-collection
source ./src/setup_environment
cd ../util/job_launching
./run_selected.sh -Y my_jobs.yml
```

Run a selected list locally:

```bash
./run_selected.sh \
  -B rodinia-3.1:backprop-rodinia-3.1,rodinia-3.1:bfs-rodinia-3.1 \
  -C QV100-2B_INSN \
  -N my-local-run \
  -l local \
  -c 2 \
  -r sim_run_11.8_my_local_run
```

Or use a YAML file:

```bash
cp util/job_launching/selected_jobs.example.yml util/job_launching/my_jobs.yml
cd util/job_launching
./run_selected.sh -Y my_jobs.yml
```

Run a selected list with Slurm:

```bash
./run_selected.sh \
  -B rodinia-3.1:backprop-rodinia-3.1,rodinia-3.1:bfs-rodinia-3.1 \
  -C QV100-2B_INSN \
  -N my-sbatch-run \
  -l sbatch \
  -r sim_run_11.8_my_sbatch_run
```

## 5. Check job status

Local or Slurm:

```bash
python3 job_status.py -N my-local-run -r sim_run_11.8_my_local_run
python3 job_status.py -N my-sbatch-run -r sim_run_11.8_my_sbatch_run
```

Print local `procman` status:

```bash
python3 procman.py -p
```

## 6. Where stdout is

Each prepared task directory contains:

- `slurm.sim`
- `justrun.sh`
- `run.out`
- `run.err`

For local `procman` runs, live stdout/stderr are first written to `/tmp/...o<jobid>` and `/tmp/...e<jobid>`, then copied back into the task directory when the job ends.

For Slurm runs, `run.out` and `run.err` point to the job-specific output files after the job finishes.

## 7. Important note for local `procman`

In my Codex sandbox, the queued jobs were created correctly but the detached `procman` manager process did not stay alive after the launching shell exited. When that happens, start the manager once yourself:

```bash
python3 procman.py -S -c 2
```

On a normal interactive shell, you may not need this manual step.

## 8. Benchmark list format

Use a comma-separated list for `-B`:

```bash
rodinia-3.1:backprop-rodinia-3.1,rodinia-3.1:bfs-rodinia-3.1
```

Examples:

- one task: `-B rodinia-3.1:backprop-rodinia-3.1`
- two tasks: `-B rodinia-3.1:backprop-rodinia-3.1,rodinia-3.1:bfs-rodinia-3.1`
- another pair: `-B rodinia-3.1:kmeans-rodinia-3.1,rodinia-3.1:srad_v2-rodinia-3.1`

You can also put the same information in a YAML file:

```yaml
benchmarks:
  - rodinia-3.1:backprop-rodinia-3.1
  - rodinia-3.1:bfs-rodinia-3.1
configs:
  - QV100-2B_INSN
launch_name: my-local-run
run_dir: sim_run_11.8_my_local_run
launcher: local
cores: 2
```
