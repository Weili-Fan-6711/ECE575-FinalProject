# Rodinia Effective Compression Comparison (updated experiment set)

Columns are curated experiment settings. Values are the per-kernel `average effective compression ratio` lines taken directly from each `kernel_name` block in `run.out`. `NA` means the kernel had zero compression requests. `missing_runout` means the task directory existed but no `run.out` was present. `not_in_batch` means that experiment did not include that task at all.

## Task Status

| Task | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- |
| backprop-rodinia-3.1/65536 | ok (2 kernels) | ok (2 kernels) | ok (2 kernels) | ok (2 kernels) | ok (2 kernels) | ok (2 kernels) | ok (2 kernels) |
| bfs-rodinia-3.1/__data_graph1MW_6_txt | ok (24 kernels) | ok (24 kernels) | ok (24 kernels) | ok (24 kernels) | ok (24 kernels) | ok (24 kernels) | ok (24 kernels) |
| hotspot-rodinia-3.1/1024_2_2___data_temp_1024___data_power_1024_output_out | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) | ok (1 kernels) |
| kmeans-rodinia-3.1/_o__i___data_28k_4x_features_txt | ok (17 kernels) | ok (17 kernels) | ok (17 kernels) | ok (17 kernels) | ok (17 kernels) | ok (17 kernels) | ok (17 kernels) |
| kmeans-rodinia-3.1/_o__i___data_819200_txt | missing_runout (0 kernels) | missing_runout (0 kernels) | missing_runout (0 kernels) | missing_runout (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | missing_runout (0 kernels) |
| kmeans-rodinia-3.1/_o__i___data_kdd_cup | ok (5 kernels) | missing_runout (0 kernels) | missing_runout (0 kernels) | missing_runout (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | ok (5 kernels) |
| lavaMD-rodinia-3.1/_boxes1d_10 | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | ok (1 kernels) | ok (1 kernels) | not_in_batch (0 kernels) |
| lud-rodinia-3.1/_i___data_2048_dat | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | ok (18 kernels) | ok (18 kernels) | not_in_batch (0 kernels) |
| srad_v2-rodinia-3.1/2048_2048_0_127_0_127_0_5_2 | ok (4 kernels) | ok (4 kernels) | ok (4 kernels) | ok (4 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | ok (4 kernels) |
| streamcluster-rodinia-3.1/10_20_256_65536_65536_1000_none_output_txt_1 | ok (17 kernels) | ok (17 kernels) | missing_runout (0 kernels) | ok (17 kernels) | not_in_batch (0 kernels) | not_in_batch (0 kernels) | ok (17 kernels) |

## backprop-rodinia-3.1/65536

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z22bpnn_layerforward_CUDAPfS_S_S_ii | NA | NA | NA | NA | NA | NA | NA |
| 2 | _Z24bpnn_adjust_weights_cudaPfiS_iS_S_ | 2.623847 | 2.624143 | 2.623749 | 2.624095 | 2.624956 | 2.624095 | 2.623749 |

## bfs-rodinia-3.1/__data_graph1MW_6_txt

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z6KernelP4NodePiPbS2_S2_S1_i | NA | NA | NA | NA | NA | NA | NA |
| 2 | _Z7Kernel2PbS_S_S_i | 4.000000 | 4.000000 | 4.000000 | 4.000000 | 4.000000 | 4.000000 | 4.000000 |
| 3 | _Z6KernelP4NodePiPbS2_S2_S1_i | 3.999393 | 3.999425 | 3.999329 | 3.999329 | 3.999329 | 3.999329 | 3.999329 |
| 4 | _Z7Kernel2PbS_S_S_i | 3.999393 | 3.999425 | 3.999329 | 3.999329 | 3.999329 | 3.999329 | 3.999329 |
| 5 | _Z6KernelP4NodePiPbS2_S2_S1_i | 3.994431 | 3.994600 | 3.994261 | 3.994208 | 3.994272 | 3.994208 | 3.994261 |
| 6 | _Z7Kernel2PbS_S_S_i | 3.994431 | 3.994600 | 3.994261 | 3.994208 | 3.994272 | 3.994208 | 3.994261 |
| 7 | _Z6KernelP4NodePiPbS2_S2_S1_i | 3.968567 | 3.969459 | 3.967647 | 3.967075 | 3.967812 | 3.967075 | 3.967647 |
| 8 | _Z7Kernel2PbS_S_S_i | 3.968567 | 3.969459 | 3.967647 | 3.967075 | 3.967812 | 3.967075 | 3.967647 |
| 9 | _Z6KernelP4NodePiPbS2_S2_S1_i | 3.835799 | 3.837979 | 3.831384 | 3.828629 | 3.831221 | 3.828629 | 3.831384 |
| 10 | _Z7Kernel2PbS_S_S_i | 3.835799 | 3.837979 | 3.831384 | 3.828629 | 3.831221 | 3.828629 | 3.831384 |
| 11 | _Z6KernelP4NodePiPbS2_S2_S1_i | 3.418551 | 3.418565 | 3.386397 | 3.387210 | 3.387539 | 3.387289 | 3.386397 |
| 12 | _Z7Kernel2PbS_S_S_i | 3.424533 | 3.424965 | 3.392903 | 3.393014 | 3.393854 | 3.393543 | 3.392903 |
| 13 | _Z6KernelP4NodePiPbS2_S2_S1_i | 2.968925 | 2.983373 | 2.611941 | 2.604016 | 2.645484 | 2.622363 | 2.611941 |
| 14 | _Z7Kernel2PbS_S_S_i | 3.009366 | 3.041508 | 2.658992 | 2.648540 | 2.685776 | 2.662538 | 2.658992 |
| 15 | _Z6KernelP4NodePiPbS2_S2_S1_i | 2.446220 | 2.665534 | 2.127260 | 2.104934 | 2.094668 | 2.112056 | 2.127260 |
| 16 | _Z7Kernel2PbS_S_S_i | 2.471478 | 2.651606 | 2.172987 | 2.133128 | 2.131175 | 2.139139 | 2.172987 |
| 17 | _Z6KernelP4NodePiPbS2_S2_S1_i | 1.960547 | 2.059704 | 1.800174 | 1.800934 | 1.781982 | 1.794590 | 1.800174 |
| 18 | _Z7Kernel2PbS_S_S_i | 1.979803 | 2.059476 | 1.823621 | 1.818196 | 1.796159 | 1.811627 | 1.823621 |
| 19 | _Z6KernelP4NodePiPbS2_S2_S1_i | 1.901529 | 1.958770 | 1.767766 | 1.757634 | 1.741413 | 1.755517 | 1.767766 |
| 20 | _Z7Kernel2PbS_S_S_i | 1.917499 | 1.976412 | 1.780370 | 1.773681 | 1.755942 | 1.767768 | 1.780370 |
| 21 | _Z6KernelP4NodePiPbS2_S2_S1_i | 1.931141 | 1.988935 | 1.794890 | 1.788684 | 1.771147 | 1.782738 | 1.794890 |
| 22 | _Z7Kernel2PbS_S_S_i | 1.943814 | 2.001063 | 1.808224 | 1.802249 | 1.784606 | 1.796034 | 1.808224 |
| 23 | _Z6KernelP4NodePiPbS2_S2_S1_i | 1.946849 | 2.003969 | 1.811402 | 1.805489 | 1.787830 | 1.799212 | 1.811402 |
| 24 | _Z7Kernel2PbS_S_S_i | 1.946849 | 2.003969 | 1.811402 | 1.805489 | 1.787830 | 1.799212 | 1.811402 |

## hotspot-rodinia-3.1/1024_2_2___data_temp_1024___data_power_1024_output_out

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z14calculate_tempiPfS_S_iiiiffffff | 2.359126 | 2.329744 | 2.325867 | 1.307439 | 2.314163 | 1.307856 | 2.325867 |

## kmeans-rodinia-3.1/_o__i___data_28k_4x_features_txt

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z14invert_mappingPfS_ii | 3.867706 | 3.861812 | 3.622133 | 3.370165 | 3.619003 | 3.369139 | 3.622133 |
| 2 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 3 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 4 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 5 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 6 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 7 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 8 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 9 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 10 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 11 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 12 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 13 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 14 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 15 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 16 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |
| 17 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.859479 | 3.853322 | 3.614612 | 3.372352 | 3.611542 | 3.371261 | 3.614612 |

## kmeans-rodinia-3.1/_o__i___data_819200_txt

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| - | - | missing_runout (0 kernels) | missing_runout (0 kernels) | missing_runout (0 kernels) | missing_runout (0 kernels) | not_in_batch | not_in_batch | missing_runout (0 kernels) |

## kmeans-rodinia-3.1/_o__i___data_kdd_cup

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z14invert_mappingPfS_ii | 3.948273 | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | 3.858573 |
| 2 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.944579 | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | 3.854994 |
| 3 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.941161 | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | 3.851664 |
| 4 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.937741 | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | 3.848370 |
| 5 | _Z11kmeansPointPfiiiPiS_S_S0_ | 3.937740 | missing_runout | missing_runout | missing_runout | not_in_batch | not_in_batch | 3.848370 |

## lavaMD-rodinia-3.1/_boxes1d_10

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z15kernel_gpu_cuda7par_str7dim_strP7box_strP11FOUR_VECTORPdS4_ | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 2.025462 | 1.767584 | not_in_batch |

## lud-rodinia-3.1/_i___data_2048_dat

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z12lud_diagonalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | NA | NA | not_in_batch |
| 2 | _Z13lud_perimeterPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000000 | 1.000000 | not_in_batch |
| 3 | _Z12lud_internalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000731 | 1.000839 | not_in_batch |
| 4 | _Z12lud_diagonalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000732 | 1.000839 | not_in_batch |
| 5 | _Z13lud_perimeterPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000723 | 1.000827 | not_in_batch |
| 6 | _Z12lud_internalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.001026 | 1.000700 | not_in_batch |
| 7 | _Z12lud_diagonalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.001026 | 1.000700 | not_in_batch |
| 8 | _Z13lud_perimeterPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.001019 | 1.000695 | not_in_batch |
| 9 | _Z12lud_internalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000854 | 1.000636 | not_in_batch |
| 10 | _Z12lud_diagonalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000853 | 1.000636 | not_in_batch |
| 11 | _Z13lud_perimeterPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000853 | 1.000634 | not_in_batch |
| 12 | _Z12lud_internalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000764 | 1.000643 | not_in_batch |
| 13 | _Z12lud_diagonalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000764 | 1.000643 | not_in_batch |
| 14 | _Z13lud_perimeterPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000764 | 1.000640 | not_in_batch |
| 15 | _Z12lud_internalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000760 | 1.000618 | not_in_batch |
| 16 | _Z12lud_diagonalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000760 | 1.000618 | not_in_batch |
| 17 | _Z13lud_perimeterPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000759 | 1.000617 | not_in_batch |
| 18 | _Z12lud_internalPfii | not_in_batch | not_in_batch | not_in_batch | not_in_batch | 1.000740 | 1.000591 | not_in_batch |

## srad_v2-rodinia-3.1/2048_2048_0_127_0_127_0_5_2

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z11srad_cuda_1PfS_S_S_S_S_iif | 1.000699 | 1.000062 | 1.000132 | 1.000057 | not_in_batch | not_in_batch | 1.000132 |
| 2 | _Z11srad_cuda_2PfS_S_S_S_S_iiff | 1.001583 | 1.000149 | 1.000760 | 1.000025 | not_in_batch | not_in_batch | 1.000760 |
| 3 | _Z11srad_cuda_1PfS_S_S_S_S_iif | 1.001457 | 1.000417 | 1.000698 | 1.000050 | not_in_batch | not_in_batch | 1.000698 |
| 4 | _Z11srad_cuda_2PfS_S_S_S_S_iiff | 1.004261 | 1.000789 | 1.002483 | 1.000069 | not_in_batch | not_in_batch | 1.002483 |

## streamcluster-rodinia-3.1/10_20_256_65536_65536_1000_none_output_txt_1

| idx | kernel | Huff+Age | Huff | Shan+Age | Shan+Age FIFO512SP32TP4 | Shan+Age FIFO1024SP64TP4 | Shan+Age FIFO512SP32TP8 | Shan+Age rerun |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.120323 | 1.039695 | missing_runout | 1.113404 | not_in_batch | not_in_batch | 1.119112 |
| 2 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.082989 | 1.028318 | missing_runout | 1.076483 | not_in_batch | not_in_batch | 1.081809 |
| 3 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.071270 | 1.024693 | missing_runout | 1.065180 | not_in_batch | not_in_batch | 1.070302 |
| 4 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.065088 | 1.022896 | missing_runout | 1.058097 | not_in_batch | not_in_batch | 1.064232 |
| 5 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.060242 | 1.021840 | missing_runout | 1.054973 | not_in_batch | not_in_batch | 1.060876 |
| 6 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.058735 | 1.021143 | missing_runout | 1.052357 | not_in_batch | not_in_batch | 1.058142 |
| 7 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.056970 | 1.020641 | missing_runout | 1.050578 | not_in_batch | not_in_batch | 1.056568 |
| 8 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.056044 | 1.020260 | missing_runout | 1.049408 | not_in_batch | not_in_batch | 1.055222 |
| 9 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.054177 | 1.019976 | missing_runout | 1.048436 | not_in_batch | not_in_batch | 1.054279 |
| 10 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.053672 | 1.019735 | missing_runout | 1.047941 | not_in_batch | not_in_batch | 1.053124 |
| 11 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.053202 | 1.019555 | missing_runout | 1.047190 | not_in_batch | not_in_batch | 1.051924 |
| 12 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.052762 | 1.019392 | missing_runout | 1.046386 | not_in_batch | not_in_batch | 1.051683 |
| 13 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.052269 | 1.019257 | missing_runout | 1.045737 | not_in_batch | not_in_batch | 1.051172 |
| 14 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.051923 | 1.019137 | missing_runout | 1.045462 | not_in_batch | not_in_batch | 1.051006 |
| 15 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.051697 | 1.019032 | missing_runout | 1.044951 | not_in_batch | not_in_batch | 1.050129 |
| 16 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.051391 | 1.018940 | missing_runout | 1.044733 | not_in_batch | not_in_batch | 1.050011 |
| 17 | _Z19kernel_compute_costiilP5PointiiPfS1_PiPb | 1.050002 | 1.018431 | missing_runout | 1.043501 | not_in_batch | not_in_batch | 1.048633 |
