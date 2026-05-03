# Rodinia Effective Compression Comparison (updated experiment set)

Columns are curated experiment settings. Values are the per-kernel `average effective compression ratio` lines taken directly from each `kernel_name` block in `run.out`. `NA` means the kernel had zero compression requests. `missing_runout` means the task directory existed but no `run.out` was present. `not_in_batch` means that experiment did not include that task at all.

## Task Status

| Task | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO1024SP64TP4 TS256 | Shan+Age FIFO1024SP64TP4 TS256 CODEdelayed | Shan+Age FIFO1024SP64TP4 TS512 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun | NoComp baseline |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| backprop-rodinia-3.1/65536 | ok (2 kernels) | ok (2 kernels) | ok (2 kernels) | ok (2 kernels) | ok (2 kernels) | ok (2 kernels) | ok (2 kernels) | ok (2 kernels) | ok (2 kernels) | ok (2 kernels) | ok (2 kernels) |
| bfs-rodinia-3.1/__data_graph1MW_6_txt | ok (24 kernels) | ok (24 kernels) | ok (24 kernels) | ok (24 kernels) | ok (24 kernels) | ok (24 kernels) | ok (24 kernels) | ok (24 kernels) | ok (24 kernels) | ok (24 kernels) | ok (24 kernels) |
| hotspot-rodinia-3.1/1024_2_2___data_temp_1024___data_power_1024_output_out | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) |
| kmeans-rodinia-3.1/_o__i___data_28k_4x_features_txt | ok (17 kernels) | ok (17 kernels) | ok (17 kernels) | ok (17 kernels) | ok (17 kernels) | ok (17 kernels) | ok (17 kernels) | ok (17 kernels) | ok (17 kernels) | ok (17 kernels) | ok (17 kernels) |
| kmeans-rodinia-3.1/_o__i___data_819200_txt | missing_runout (0 kernels) | missing_runout (0 kernels) | missing_runout (0 kernels) | missing_runout (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | missing_runout (0 kernels) | not_in_batch (0 kernels) |
| kmeans-rodinia-3.1/_o__i___data_kdd_cup | ok (5 kernels) | missing_runout (0 kernels) | missing_runout (0 kernels) | missing_runout (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | ok (5 kernels) | not_in_batch (0 kernels) |
| lavaMD-rodinia-3.1/_boxes1d_10 | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | not_in_batch (0 kernels) | ok (1 kernels) |
| lud-rodinia-3.1/_i___data_2048_dat | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | ok (18 kernels) | ok (18 kernels) | ok (18 kernels) | ok (18 kernels) | ok (18 kernels) | not_in_batch (0 kernels) | ok (18 kernels) |
| srad_v2-rodinia-3.1/2048_2048_0_127_0_127_0_5_2 | ok (4 kernels) | ok (4 kernels) | ok (4 kernels) | ok (4 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | ok (4 kernels) | not_in_batch (0 kernels) |
| streamcluster-rodinia-3.1/10_20_256_65536_65536_1000_none_output_txt_1 | ok (17 kernels) | ok (17 kernels) | missing_runout (0 kernels) | ok (17 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | ok (17 kernels) | not_in_batch (0 kernels) |

## Benchmark-Level Average Raw Compression Ratio

Values are the final `average raw compression ratio` reported near the end of each benchmark `run.out`. `missing_runout` means the task directory existed but no completed `run.out` was present. `not_in_batch` means that experiment did not include that task.

| Task | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO1024SP64TP4 TS256 | Shan+Age FIFO1024SP64TP4 TS256 CODEdelayed | Shan+Age FIFO1024SP64TP4 TS512 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun | NoComp baseline |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| backprop-rodinia-3.1/65536 | 4.788349 | 4.789603 | 4.788229 | 4.444496 | 4.788112 | 4.788133 | 4.721620 | 4.788134 | 4.567181 | 4.788229 | 0.000000 |
| bfs-rodinia-3.1/__data_graph1MW_6_txt | 2.661282 | 2.696635 | 2.449202 | 2.428815 | 2.424085 | 2.439792 | 2.445603 | 2.439812 | 2.415224 | 2.449202 | 0.000000 |
| hotspot-rodinia-3.1/1024_2_2___data_temp_1024___data_power_1024_output_out | 2.906764 | 2.791841 | 2.834606 | 1.539978 | 2.806340 | 2.826779 | 2.824562 | 2.818884 | 1.531313 | 2.834606 | 0.000000 |
| kmeans-rodinia-3.1/_o__i___data_28k_4x_features_txt | 5.637382 | 5.616723 | 5.130208 | 4.958211 | 5.127523 | 5.124251 | 5.124047 | 5.127523 | 4.957246 | 5.130208 | 0.000000 |
| kmeans-rodinia-3.1/_o__i___data_819200_txt | missing_runout | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | missing_runout | not_in_batch |
| kmeans-rodinia-3.1/_o__i___data_kdd_cup | 5.962909 | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 5.336401 | not_in_batch |
| lavaMD-rodinia-3.1/_boxes1d_10 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 2.872125 | 2.872957 | 2.860773 | 2.870725 | 2.210539 | not_in_batch | 0.000000 |
| lud-rodinia-3.1/_i___data_2048_dat | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000793 | 1.001751 | 1.001497 | 1.000764 | 1.000608 | not_in_batch | 0.000000 |
| srad_v2-rodinia-3.1/2048_2048_0_127_0_127_0_5_2 | 1.005084 | 1.000968 | 1.002986 | 1.000092 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.002986 | not_in_batch |
| streamcluster-rodinia-3.1/10_20_256_65536_65536_1000_none_output_txt_1 | 1.098476 | 1.034300 | missing_runout | 1.076761 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.092397 | not_in_batch |

## Benchmark-Level End-to-End GPU Cycles

Values are the final printed `gpu_tot_sim_cycle` from each completed `run.out`.

| Task | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO1024SP64TP4 TS256 | Shan+Age FIFO1024SP64TP4 TS256 CODEdelayed | Shan+Age FIFO1024SP64TP4 TS512 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun | NoComp baseline |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| backprop-rodinia-3.1/65536 | 107,345 | 107,223 | 107,364 | 107,202 | 107,385 | 107,385 | 107,378 | 107,385 | 107,202 | 107,364 | 48,346 |
| bfs-rodinia-3.1/__data_graph1MW_6_txt | 1,433,977 | 1,432,701 | 1,436,186 | 1,433,994 | 1,442,683 | 1,437,539 | 1,434,360 | 1,427,272 | 1,450,546 | 1,436,186 | 770,249 |
| hotspot-rodinia-3.1/1024_2_2___data_temp_1024___data_power_1024_output_out | 84,827 | 84,499 | 84,383 | 84,615 | 84,418 | 84,817 | 84,539 | 84,339 | 84,567 | 84,383 | 59,155 |
| kmeans-rodinia-3.1/_o__i___data_28k_4x_features_txt | 3,906,469 | 3,907,064 | 3,906,676 | 3,906,667 | 3,906,097 | 3,906,177 | 3,906,027 | 3,906,097 | 3,907,239 | 3,906,676 | 3,844,992 |
| kmeans-rodinia-3.1/_o__i___data_819200_txt | missing_runout | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | missing_runout | not_in_batch |
| kmeans-rodinia-3.1/_o__i___data_kdd_cup | 4,270,002 | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 4,269,151 | not_in_batch |
| lavaMD-rodinia-3.1/_boxes1d_10 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 321,864 | 321,864 | 321,776 | 321,864 | 321,881 | not_in_batch | 321,679 |
| lud-rodinia-3.1/_i___data_2048_dat | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1,386,894 | 1,387,812 | 1,387,308 | 1,385,486 | 1,387,936 | not_in_batch | 913,786 |
| srad_v2-rodinia-3.1/2048_2048_0_127_0_127_0_5_2 | 2,116,399 | 2,231,003 | 2,117,790 | 2,117,761 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 2,117,790 | not_in_batch |
| streamcluster-rodinia-3.1/10_20_256_65536_65536_1000_none_output_txt_1 | 12,008,656 | 12,011,587 | missing_runout | 12,010,983 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 12,010,538 | not_in_batch |

## GPU Cycle Speedup vs NoComp Baseline

Values are `NoComp baseline cycles / experiment cycles`. Values below `1.0` mean the compression-enabled run was slower than the no-compression baseline.

| Task | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO1024SP64TP4 TS256 | Shan+Age FIFO1024SP64TP4 TS256 CODEdelayed | Shan+Age FIFO1024SP64TP4 TS512 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| backprop-rodinia-3.1/65536 | 0.450380 | 0.450892 | 0.450300 | 0.450980 | 0.450212 | 0.450212 | 0.450241 | 0.450212 | 0.450980 | 0.450300 |
| bfs-rodinia-3.1/__data_graph1MW_6_txt | 0.537142 | 0.537620 | 0.536316 | 0.537135 | 0.533900 | 0.535811 | 0.536998 | 0.539665 | 0.531006 | 0.536316 |
| hotspot-rodinia-3.1/1024_2_2___data_temp_1024___data_power_1024_output_out | 0.697361 | 0.700067 | 0.701030 | 0.699108 | 0.700739 | 0.697443 | 0.699736 | 0.701396 | 0.699505 | 0.701030 |
| kmeans-rodinia-3.1/_o__i___data_28k_4x_features_txt | 0.984263 | 0.984113 | 0.984211 | 0.984213 | 0.984357 | 0.984336 | 0.984374 | 0.984357 | 0.984069 | 0.984211 |
| kmeans-rodinia-3.1/_o__i___data_819200_txt | missing_runout | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | missing_runout |
| kmeans-rodinia-3.1/_o__i___data_kdd_cup | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch |
| lavaMD-rodinia-3.1/_boxes1d_10 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 0.999425 | 0.999425 | 0.999699 | 0.999425 | 0.999372 | not_in_batch |
| lud-rodinia-3.1/_i___data_2048_dat | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 0.658872 | 0.658436 | 0.658676 | 0.659542 | 0.658378 | not_in_batch |
| srad_v2-rodinia-3.1/2048_2048_0_127_0_127_0_5_2 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch |
| streamcluster-rodinia-3.1/10_20_256_65536_65536_1000_none_output_txt_1 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch |

## backprop-rodinia-3.1/65536

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO1024SP64TP4 TS256 | Shan+Age FIFO1024SP64TP4 TS256 CODEdelayed | Shan+Age FIFO1024SP64TP4 TS512 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z22bpnn_layerforward_CUDAPfS_S_S_ii | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA |
| 2 | _Z24bpnn_adjust_weights_cudaPfiS_iS_S_ | 2.623847 | 2.624143 | 2.623749 | 2.624095 | 2.624956 | 2.624956 | 2.596862 | 2.624956 | 2.624095 | 2.623749 |

## bfs-rodinia-3.1/__data_graph1MW_6_txt

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO1024SP64TP4 TS256 | Shan+Age FIFO1024SP64TP4 TS256 CODEdelayed | Shan+Age FIFO1024SP64TP4 TS512 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z6KernelP4NodePiPbS2_S2_S1_i | NA | NA | NA | NA | NA | NA | NA | NA | NA | NA |
| 2 | _Z7Kernel2PbS_S_S_i | 4.000000 | 4.000000 | 4.000000 | 4.000000 | 4.000000 | 4.000000 | 4.000000 | 4.000000 | 4.000000 | 4.000000 |
| 3 | _Z6KernelP4NodePiPbS2_S2_S1_i | 3.999393 | 3.999425 | 3.999329 | 3.999329 | 3.999329 | 3.999329 | 3.999329 | 3.999329 | 3.999329 | 3.999329 |
| 4 | _Z7Kernel2PbS_S_S_i | 3.999393 | 3.999425 | 3.999329 | 3.999329 | 3.999329 | 3.999329 | 3.999329 | 3.999329 | 3.999329 | 3.999329 |
| 5 | _Z6KernelP4NodePiPbS2_S2_S1_i | 3.994431 | 3.994600 | 3.994261 | 3.994208 | 3.994272 | 3.994272 | 3.994261 | 3.994272 | 3.994208 | 3.994261 |
| 6 | _Z7Kernel2PbS_S_S_i | 3.994431 | 3.994600 | 3.994261 | 3.994208 | 3.994272 | 3.994272 | 3.994261 | 3.994272 | 3.994208 | 3.994261 |
| 7 | _Z6KernelP4NodePiPbS2_S2_S1_i | 3.968567 | 3.969459 | 3.967647 | 3.967075 | 3.967812 | 3.967812 | 3.967636 | 3.967812 | 3.967075 | 3.967647 |
| 8 | _Z7Kernel2PbS_S_S_i | 3.968567 | 3.969459 | 3.967647 | 3.967075 | 3.967812 | 3.967812 | 3.967636 | 3.967812 | 3.967075 | 3.967647 |
| 9 | _Z6KernelP4NodePiPbS2_S2_S1_i | 3.835799 | 3.837979 | 3.831384 | 3.828629 | 3.831221 | 3.831348 | 3.830007 | 3.831248 | 3.828629 | 3.831384 |
| 10 | _Z7Kernel2PbS_S_S_i | 3.835799 | 3.837979 | 3.831384 | 3.828629 | 3.831221 | 3.831348 | 3.830007 | 3.831248 | 3.828629 | 3.831384 |
| 11 | _Z6KernelP4NodePiPbS2_S2_S1_i | 3.418551 | 3.418565 | 3.386397 | 3.387210 | 3.387539 | 3.385555 | 3.382962 | 3.385362 | 3.387289 | 3.386397 |
| 12 | _Z7Kernel2PbS_S_S_i | 3.424533 | 3.424965 | 3.392903 | 3.393014 | 3.393854 | 3.392109 | 3.389514 | 3.391766 | 3.393543 | 3.392903 |
| 13 | _Z6KernelP4NodePiPbS2_S2_S1_i | 2.968925 | 2.983373 | 2.611941 | 2.604016 | 2.645484 | 2.669740 | 2.642694 | 2.601852 | 2.622363 | 2.611941 |
| 14 | _Z7Kernel2PbS_S_S_i | 3.009366 | 3.041508 | 2.658992 | 2.648540 | 2.685776 | 2.703093 | 2.679123 | 2.641932 | 2.662538 | 2.658992 |
| 15 | _Z6KernelP4NodePiPbS2_S2_S1_i | 2.446220 | 2.665534 | 2.127260 | 2.104934 | 2.094668 | 2.125585 | 2.126741 | 2.133450 | 2.112056 | 2.127260 |
| 16 | _Z7Kernel2PbS_S_S_i | 2.471478 | 2.651606 | 2.172987 | 2.133128 | 2.131175 | 2.160127 | 2.153696 | 2.168924 | 2.139139 | 2.172987 |
| 17 | _Z6KernelP4NodePiPbS2_S2_S1_i | 1.960547 | 2.059704 | 1.800174 | 1.800934 | 1.781982 | 1.796183 | 1.797736 | 1.802397 | 1.794590 | 1.800174 |
| 18 | _Z7Kernel2PbS_S_S_i | 1.979803 | 2.059476 | 1.823621 | 1.818196 | 1.796159 | 1.814080 | 1.815468 | 1.816385 | 1.811627 | 1.823621 |
| 19 | _Z6KernelP4NodePiPbS2_S2_S1_i | 1.901529 | 1.958770 | 1.767766 | 1.757634 | 1.741413 | 1.756571 | 1.758698 | 1.754903 | 1.755517 | 1.767766 |
| 20 | _Z7Kernel2PbS_S_S_i | 1.917499 | 1.976412 | 1.780370 | 1.773681 | 1.755942 | 1.770461 | 1.774349 | 1.768921 | 1.767768 | 1.780370 |
| 21 | _Z6KernelP4NodePiPbS2_S2_S1_i | 1.931141 | 1.988935 | 1.794890 | 1.788684 | 1.771147 | 1.785812 | 1.789724 | 1.784485 | 1.782738 | 1.794890 |
| 22 | _Z7Kernel2PbS_S_S_i | 1.943814 | 2.001063 | 1.808224 | 1.802249 | 1.784606 | 1.799172 | 1.803282 | 1.798083 | 1.796034 | 1.808224 |
| 23 | _Z6KernelP4NodePiPbS2_S2_S1_i | 1.946849 | 2.003969 | 1.811402 | 1.805489 | 1.787830 | 1.802370 | 1.806521 | 1.801317 | 1.799212 | 1.811402 |
| 24 | _Z7Kernel2PbS_S_S_i | 1.946849 | 2.003969 | 1.811402 | 1.805489 | 1.787830 | 1.802370 | 1.806521 | 1.801317 | 1.799212 | 1.811402 |

## hotspot-rodinia-3.1/1024_2_2___data_temp_1024___data_power_1024_output_out

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO1024SP64TP4 TS256 | Shan+Age FIFO1024SP64TP4 TS256 CODEdelayed | Shan+Age FIFO1024SP64TP4 TS512 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z14calculate_tempiPfS_S_iiiiffffff | 2.359126 | 2.329744 | 2.325867 | 1.307439 | 2.314163 | 2.341837 | 2.324685 | 2.333952 | 1.307856 | 2.325867 |

## kmeans-rodinia-3.1/_o__i___data_28k_4x_features_txt

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO1024SP64TP4 TS256 | Shan+Age FIFO1024SP64TP4 TS256 CODEdelayed | Shan+Age FIFO1024SP64TP4 TS512 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z14invert_mappingPfS_ii | 3.867706 | 3.861812 | 3.622133 | 3.370165 | 3.619003 | 3.613495 | 3.613342 | 3.619003 | 3.369139 | 3.622133 |
| 2 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 3 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 4 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 5 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 6 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 7 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 8 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 9 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 10 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 11 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 12 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 13 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 14 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 15 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 16 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |
| 17 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.606204 | 3.606029 | 3.611542 | 3.371261 | 3.614612 |

## kmeans-rodinia-3.1/_o__i___data_819200_txt

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO1024SP64TP4 TS256 | Shan+Age FIFO1024SP64TP4 TS256 CODEdelayed | Shan+Age FIFO1024SP64TP4 TS512 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| - | - | missing_runout (0 kernels) | missing_runout (0 kernels) | missing_runout (0 kernels) | missing_runout (0 kernels) | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | missing_runout (0 kernels) |

## kmeans-rodinia-3.1/_o__i___data_kdd_cup

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO1024SP64TP4 TS256 | Shan+Age FIFO1024SP64TP4 TS256 CODEdelayed | Shan+Age FIFO1024SP64TP4 TS512 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z14invert_mappingPfS_ii | 3.948273 | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 3.858573 |
| 2 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.944579 | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 3.854994 |
| 3 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.941161 | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 3.851664 |
| 4 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.937741 | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 3.848370 |
| 5 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.937740 | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 3.848370 |

## lavaMD-rodinia-3.1/_boxes1d_10

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO1024SP64TP4 TS256 | Shan+Age FIFO1024SP64TP4 TS256 CODEdelayed | Shan+Age FIFO1024SP64TP4 TS512 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z15kernel_gpu_cuda7par_str7dim_strP7box_strP11FOUR_VECTORPdS4_ | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 2.025462 | 2.025462 | 2.019832 | 2.025462 | 1.767584 | not_in_batch |

## lud-rodinia-3.1/_i___data_2048_dat

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO1024SP64TP4 TS256 | Shan+Age FIFO1024SP64TP4 TS256 CODEdelayed | Shan+Age FIFO1024SP64TP4 TS512 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z12lud_diagonalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | NA | NA | NA | NA | NA | not_in_batch |
| 2 | _Z13lud_perimeterPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000000 | 1.000000 | 1.000000 | 1.000000 | 1.000000 | not_in_batch |
| 3 | _Z12lud_internalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000731 | 1.002582 | 1.002116 | 1.001351 | 1.000839 | not_in_batch |
| 4 | _Z12lud_diagonalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000732 | 1.002582 | 1.002115 | 1.001351 | 1.000839 | not_in_batch |
| 5 | _Z13lud_perimeterPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000723 | 1.002544 | 1.002085 | 1.001331 | 1.000827 | not_in_batch |
| 6 | _Z12lud_internalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.001026 | 1.002253 | 1.001879 | 1.001047 | 1.000700 | not_in_batch |
| 7 | _Z12lud_diagonalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.001026 | 1.002253 | 1.001879 | 1.001047 | 1.000700 | not_in_batch |
| 8 | _Z13lud_perimeterPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.001019 | 1.002238 | 1.001866 | 1.001040 | 1.000695 | not_in_batch |
| 9 | _Z12lud_internalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000854 | 1.001992 | 1.001853 | 1.000903 | 1.000636 | not_in_batch |
| 10 | _Z12lud_diagonalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000853 | 1.001992 | 1.001853 | 1.000903 | 1.000636 | not_in_batch |
| 11 | _Z13lud_perimeterPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000853 | 1.001982 | 1.001845 | 1.000900 | 1.000634 | not_in_batch |
| 12 | _Z12lud_internalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000764 | 1.001769 | 1.001636 | 1.000840 | 1.000643 | not_in_batch |
| 13 | _Z12lud_diagonalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000764 | 1.001768 | 1.001636 | 1.000840 | 1.000643 | not_in_batch |
| 14 | _Z13lud_perimeterPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000764 | 1.001763 | 1.001631 | 1.000841 | 1.000640 | not_in_batch |
| 15 | _Z12lud_internalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000760 | 1.001745 | 1.001495 | 1.000765 | 1.000618 | not_in_batch |
| 16 | _Z12lud_diagonalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000760 | 1.001745 | 1.001495 | 1.000765 | 1.000618 | not_in_batch |
| 17 | _Z13lud_perimeterPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000759 | 1.001741 | 1.001491 | 1.000763 | 1.000617 | not_in_batch |
| 18 | _Z12lud_internalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000740 | 1.001669 | 1.001429 | 1.000731 | 1.000591 | not_in_batch |

## srad_v2-rodinia-3.1/2048_2048_0_127_0_127_0_5_2

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO1024SP64TP4 TS256 | Shan+Age FIFO1024SP64TP4 TS256 CODEdelayed | Shan+Age FIFO1024SP64TP4 TS512 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z11srad_cuda_1PfS_S_S_S_S_iif | 1.000699 | 1.000062 | 1.000132 | 1.000057 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000132 |
| 2 | _Z11srad_cuda_2PfS_S_S_S_S_iiff | 1.001583 | 1.000149 | 1.000760 | 1.000025 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000760 |
| 3 | _Z11srad_cuda_1PfS_S_S_S_S_iif | 1.001457 | 1.000417 | 1.000698 | 1.000050 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000698 |
| 4 | _Z11srad_cuda_2PfS_S_S_S_S_iiff | 1.004261 | 1.000789 | 1.002483 | 1.000069 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.002483 |

## streamcluster-rodinia-3.1/10_20_256_65536_65536_1000_none_output_txt_1

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO1024SP64TP4 TS256 | Shan+Age FIFO1024SP64TP4 TS256 CODEdelayed | Shan+Age FIFO1024SP64TP4 TS512 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.120323 | 1.039695 | missing_runout | 1.113404 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.119112 |
| 2 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.082989 | 1.028318 | missing_runout | 1.076483 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.081809 |
| 3 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.071270 | 1.024693 | missing_runout | 1.065180 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.070302 |
| 4 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.065088 | 1.022896 | missing_runout | 1.058097 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.064232 |
| 5 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.060242 | 1.021840 | missing_runout | 1.054973 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.060876 |
| 6 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.058735 | 1.021143 | missing_runout | 1.052357 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.058142 |
| 7 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.056970 | 1.020641 | missing_runout | 1.050578 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.056568 |
| 8 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.056044 | 1.020260 | missing_runout | 1.049408 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.055222 |
| 9 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.054177 | 1.019976 | missing_runout | 1.048436 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.054279 |
| 10 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.053672 | 1.019735 | missing_runout | 1.047941 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.053124 |
| 11 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.053202 | 1.019555 | missing_runout | 1.047190 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.051924 |
| 12 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.052762 | 1.019392 | missing_runout | 1.046386 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.051683 |
| 13 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.052269 | 1.019257 | missing_runout | 1.045737 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.051172 |
| 14 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.051923 | 1.019137 | missing_runout | 1.045462 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.051006 |
| 15 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.051697 | 1.019032 | missing_runout | 1.044951 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.050129 |
| 16 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.051391 | 1.018940 | missing_runout | 1.044733 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.050011 |
| 17 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.050002 | 1.018431 | missing_runout | 1.043501 | not_in_batch | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.048633 |
