# Metadata Path Debug Context

## Current Summary

- Ongoing compression experiments should use only the non-sector L2 config:
  `QV100_NONSECTOR-2B_INSN`, sourced from
  `configs/tested-cfgs/SM7_QV100_NONSECTOR/gpgpusim.config`.
- This non-sector-only policy is intentional: compression is modeled at cacheline
  granularity, so future debug and benchmark runs should avoid the sector-L2
  configuration except as historical reference.
- The metadata cache merge-waiter limit is currently relaxed to `256`
  (`A:256:256`) to keep metadata from dominating the results. This is a relaxed
  assumption for debugging and evaluation, not a final tuned hardware choice.
- Metadata-MSHR/merge tuning still needs a dedicated follow-up study after the
  main compression-path bottlenecks are removed.
- Backprop debug run directories are grouped under:
  `util/job_launching/debug_backprop_compression_path_runs/`

## Purpose

This note captures the current debugging context for the compression-path slowdown
investigation, so work can resume cleanly after context loss or memory compaction.

## Standard Workflow

### 1. Request a shared interactive node

Default node request for rebuild/debug work:

```bash
sinteractive -p shared --cpus-per-task=32 -t 08:00:00
```

### 2. Rebuild `gpgpu-sim`

Canonical rebuild command from `BUILD_AND_DATA_COMMANDS.md`:

```bash
cd /anvil/projects/x-nairr250138/sitao/gpu/ECE575-FinalProject
source setup_environment
make -j8
```

Practical note:
- if the shared node runs out of memory with `make -j8`, retry with fewer jobs
  (for example `make -j2`), but keep `make -j8` as the documented default.

## Current Question

Why does the compressed run with idealized de/compressor settings still have much
 lower IPC than the no-compression non-sector baseline?

Primary suspicion at the moment:
- post-metadata compressed data handling and any remaining non-sector-path
  artifacts

## Reference Runs

### Baseline: non-sector, no compression

Path:
- `util/job_launching/debug_backprop_compression_path_runs/sim_run_11.8_backprop_nonsector_nocomp_final/backprop-rodinia-3.1/65536/QV100_NONSECTOR-2B_INSN/run.out`

Important second-kernel numbers:
- `gpu_sim_cycle = 34821`
- `gpu_tot_sim_cycle = 55764`
- `gpu_ipc = 1716.4695`
- `gpu_stall_dramfull = 683330`
- `W0_Scoreboard = 6856755`
- `L2_total_cache_reservation_fails = 818705`

### Compressed rebuild: 128KB metadata cache

Path:
- `util/job_launching/debug_backprop_compression_path_runs/sim_run_11.8_backprop_rebuild_compression_metadata128KB/backprop-rodinia-3.1/65536/QV100-2B_INSN/run.out`

Important second-kernel numbers:
- `gpu_sim_cycle = 60717`
- `gpu_tot_sim_cycle = 81642`
- `gpu_ipc = 984.3896`
- `gpu_stall_dramfull = 295561`
- `W0_Scoreboard = 14695460`
- `L2_total_cache_reservation_fails = 188942`

Cycle deltas versus baseline:
- kernel delta: `60717 - 34821 = 25896`
- total delta: `81642 - 55764 = 25878`

## Important Findings So Far

### 1. This run is really compressed

Compression-control macros in `src/gpgpu-sim/l2cache.cc` are currently:
- `SAMPLING_FIFO_CAPACITY 1024`
- `SAMPLING_DURATION 15000`

So this is not the disabled-compression mode.

### 2. Metadata cache size matters

Increasing metadata cache capacity from the original 8KB config to 128KB reduced:
- metadata miss rate
- metadata reservation fails
- total cycles

But compression is still much slower than the no-compression non-sector baseline.

### 3. Metadata miss rate is not the main smoking gun

In the 128KB run:
- metadata accesses: `237830`
- metadata misses: `17348`
- metadata miss rate: `0.0729`
- metadata reservation fails: `890452`

So the metadata path is not mainly suffering from miss latency alone.
It is suffering from repeated retry / structural contention.

### 4. Metadata reservation fails are on the same scale as the cycle gap

128KB metadata run:
- total metadata reservation fails: `890452`
- average per partition: `27826.625`

This is numerically close to the second-kernel cycle delta vs baseline:
- `25896`

This does not prove a one-to-one relationship, but it strongly suggests the
metadata path is a major contributor to slowdown.

### 5. Metadata reservation fails are overwhelmingly merge-pressure failures

Using the fail-breakdown instrumentation on:
- `util/job_launching/debug_backprop_compression_path_runs/sim_run_11.8_backprop_rebuild_compression_metadata128KB_failbreakdown/backprop-rodinia-3.1/65536/QV100-2B_INSN/run.out`

Totals across the second-kernel metadata summaries:
- `Reservation_fails = 890452`
- `LINE_ALLOC_FAIL = 5`
- `MISS_QUEUE_FULL = 0`
- `MSHR_ENRTY_FAIL = 0`
- `MSHR_MERGE_ENRTY_FAIL = 890447`
- `MSHR_RW_PENDING = 0`

So the metadata bottleneck is not:
- lack of brand-new MSHR entries
- miss queue capacity
- line allocation

It is almost entirely:
- too many concurrent requests trying to merge onto already-outstanding metadata
  misses, exceeding the metadata cache's `m_mshr_max_merge`

Relevant metadata cache config in this run:
- `-gpgpu_cache:huffman_metadata N:128:128:8,L:B:m:W:L,A:256:4,32:0,32`

Interpretation:
- metadata cache is `NORMAL`
- line size is `128B`
- `mshr_entries = 256`
- `mshr_max_merge = 4`

This matters because metadata address generation uses:
- `mf->m_metadata_addr = mf->m_original_addr >> 7`

So many compressed-data lines can map into the same `128B` metadata cache line,
creating heavy merge pressure on a small number of outstanding metadata misses.

### 6. One metadata request does not block the L2 front-end immediately

The L2 is non-blocking at miss acceptance:
- read misses are accepted into MSHRs in `baseline_cache::send_read_request`
- the subpartition front-end pops the incoming request once the miss is accepted

However, metadata does add extra service stages and can still reduce lower-level
throughput through queueing, retries, and held partition credits.

## Current Instrumentation

Existing custom metadata counters are in:
- `src/gpgpu-sim/l2cache.h`
- `src/gpgpu-sim/l2cache.cc`
- `src/gpgpu-sim/gpu-sim.cc`

Current printed metadata summary includes:
- `Access`
- `Miss`
- `Miss_rate`
- `Pending_hits`
- `Reservation_fails`
- `Routed`
- `Fixed_latency_returns`
- `Read_recompressions`
- `Ready_but_dramfull_stalls`
- queue maxima

## Definitions of Reservation-Fail Reasons

These come from `src/gpgpu-sim/gpu-cache.h`:
- `LINE_ALLOC_FAIL`: all candidate cache lines are reserved
- `MISS_QUEUE_FULL`: the cache miss queue cannot accept another miss request
- `MSHR_ENRTY_FAIL`: no free MSHR entry exists for a new miss
- `MSHR_MERGE_ENRTY_FAIL`: same MSHR exists, but merge capacity is exhausted
- `MSHR_RW_PENDING`: write/read ordering hazard in pending MSHR state

## Next Debug Goal

Break down metadata `Reservation_fails` by fail reason, per partition, to answer:
- Is metadata bottlenecked by MSHR count?
- Miss queue size?
- Line allocation?
- Merge pressure?

That should tell us what structure is actually saturating.

## Follow-Up Experiment Plan

Next two sensitivity tests on the metadata cache MSHR knobs:

1. Increase merge capacity only
- change metadata cache config from `A:256:4` to `A:256:256`
- purpose: test whether removing merge pressure largely removes the slowdown

2. Swap entries and merge capacity
- change metadata cache config from `A:256:4` to `A:4:256`
- purpose: test whether the design really needs many distinct metadata MSHR
  entries, or whether the main issue is almost entirely merge capacity

Reference interpretation:
- if `A:256:256` improves a lot, merge pressure is a primary bottleneck
- if `A:4:256` is still good, distinct-entry count is overprovisioned
- if `A:4:256` collapses badly, then both merge capacity and entry count matter

## Follow-Up Experiment Results

Reference files:
- baseline non-sector no-compression:
  `util/job_launching/debug_backprop_compression_path_runs/sim_run_11.8_backprop_nonsector_nocomp_final/backprop-rodinia-3.1/65536/QV100_NONSECTOR-2B_INSN/run.out`
- original metadata config `A:256:4`:
  `util/job_launching/debug_backprop_compression_path_runs/sim_run_11.8_backprop_rebuild_compression_metadata128KB_failbreakdown/backprop-rodinia-3.1/65536/QV100-2B_INSN/run.out`
- experiment `A:256:256`:
  `util/job_launching/debug_backprop_compression_path_runs/sim_run_11.8_backprop_rebuild_compression_metadata128KB_mshr256_merge256/backprop-rodinia-3.1/65536/QV100-2B_INSN/run.out`
- experiment `A:4:256`:
  `util/job_launching/debug_backprop_compression_path_runs/sim_run_11.8_backprop_rebuild_compression_metadata128KB_mshr4_merge256/backprop-rodinia-3.1/65536/QV100-2B_INSN/run.out`

Second-kernel summary:

| Case | Metadata config | `gpu_sim_cycle` | `gpu_tot_sim_cycle` | `gpu_ipc` |
| --- | --- | ---: | ---: | ---: |
| Baseline | no metadata | 34821 | 55764 | 1716.4695 |
| Original compressed | `A:256:4` | 60717 | 81642 | 984.3896 |
| Merge-expanded | `A:256:256` | 39013 | 59938 | 1532.0325 |
| Entry-swapped | `A:4:256` | 39142 | 60067 | 1526.9834 |

Key metadata totals:

| Case | Reservation fails | `LINE_ALLOC` | `MSHR_ENTRY` | `MSHR_MERGE` | Ready-but-dramfull |
| --- | ---: | ---: | ---: | ---: | ---: |
| `A:256:4` | 890452 | 5 | 0 | 890447 | 1245 |
| `A:256:256` | 13137 | 13137 | 0 | 0 | 20976 |
| `A:4:256` | 405748 | 0 | 405748 | 0 | 24404 |

Interpretation:
- increasing merge from `4` to `256` removes the original dominant bottleneck
  (`MSHR_MERGE_ENRTY_FAIL`) and cuts second-kernel time from `60717` to `39013`
- swapping to `A:4:256` keeps performance almost the same (`39142`), so the
  design does not appear to need a large number of distinct metadata MSHR
  entries for this workload
- after merge pressure is removed, the failure mode shifts:
  - `A:256:256` mostly hits `LINE_ALLOC_FAIL`
  - `A:4:256` mostly hits `MSHR_ENRTY_FAIL`
- both variants are still slower than the no-compression baseline, so metadata
  MSHR merge pressure was a major bottleneck, but not the only remaining source
  of slowdown
