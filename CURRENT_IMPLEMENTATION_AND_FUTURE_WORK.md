# Current Implementation And Future Work

## Current Implementation Notes

1. Metadata requests do not really go through full DRAM service right now.
   Because the metadata address mapping is only approximate, metadata misses are
   modeled with a fixed latency and then directly returned to the metadata cache.

2. Metadata cache MSHR constraints are currently relaxed for evaluation.
   The non-sector config presently uses a large merge-waiter limit
   (`A:256:256`) to keep metadata from becoming the dominant bottleneck.

3. Ongoing GPU compression experiments currently support only the non-sector L2
   configuration.
   Use `QV100_NONSECTOR-2B_INSN` from
   `configs/tested-cfgs/SM7_QV100_NONSECTOR/gpgpusim.config`.

4. Metadata routing currently reuses the original `mem_fetch` by modifying its
   address and flags.
   Ideally, the implementation should generate a separate metadata request
   instead of mutating the original request object in place.

## Future Work

1. Replace the fixed-latency metadata-return approximation with a cleaner model
   once metadata address mapping is made precise enough to support real DRAM
   service.

2. Revisit metadata cache MSHR sizing and merge limits with a more realistic
   hardware study after the main compression-path bottlenecks are removed.

3. Decide whether non-sector-only support is the final architecture policy, or
   whether sector-cache support should be reintroduced with a correct
   line-granular compressed-fetch model.

4. Refactor metadata handling so the original request and metadata request are
   separate objects, which should simplify correctness, accounting, and future
   debugging.
