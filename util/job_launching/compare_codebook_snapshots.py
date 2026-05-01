#!/usr/bin/env python3

import argparse
import math
import re
import sys


SNAPSHOT_RE = re.compile(
    r"codebook snapshot: cycle = (\d+), entropy = ([0-9.]+) bits/symbol .* total symbols = (\d+)"
)
TOP_RE = re.compile(
    r"top(\d+): pattern = 0x([0-9a-fA-F]+) \([^)]*\), frequency = (\d+), probability = ([0-9.]+)"
)


def parse_snapshots(path):
    snapshots = []
    current = None

    with open(path, "r", errors="replace") as f:
        for lineno, line in enumerate(f, 1):
            match = SNAPSHOT_RE.search(line)
            if match:
                current = {
                    "cycle": int(match.group(1)),
                    "entropy": float(match.group(2)),
                    "total_symbols": int(match.group(3)),
                    "top": [],
                    "lineno": lineno,
                }
                snapshots.append(current)
                continue

            match = TOP_RE.search(line)
            if match and current is not None:
                current["top"].append(
                    {
                        "rank": int(match.group(1)),
                        "pattern": match.group(2).lower(),
                        "frequency": int(match.group(3)),
                        "probability": float(match.group(4)),
                    }
                )

    return snapshots


def same_top_entries(left, right, tol=1e-12):
    if len(left) != len(right):
        return False
    for a, b in zip(left, right):
        if a["rank"] != b["rank"]:
            return False
        if a["pattern"] != b["pattern"]:
            return False
        if a["frequency"] != b["frequency"]:
            return False
        if not math.isclose(a["probability"], b["probability"], rel_tol=0.0, abs_tol=tol):
            return False
    return True


def compare(left_path, right_path, tol):
    left = parse_snapshots(left_path)
    right = parse_snapshots(right_path)

    print(f"left snapshots : {len(left)}")
    print(f"right snapshots: {len(right)}")

    if len(left) != len(right):
        print("snapshot count differs")
        return 1

    identical = True
    first_difference = None

    for idx, (a, b) in enumerate(zip(left, right), 1):
        fields_equal = (
            a["cycle"] == b["cycle"]
            and math.isclose(a["entropy"], b["entropy"], rel_tol=0.0, abs_tol=tol)
            and a["total_symbols"] == b["total_symbols"]
            and same_top_entries(a["top"], b["top"], tol)
        )
        if not fields_equal:
            identical = False
            first_difference = (idx, a, b)
            break

    if identical:
        print("result: snapshots are identical")
        if left:
            print(
                f"first snapshot: cycle={left[0]['cycle']} entropy={left[0]['entropy']:.6f} total_symbols={left[0]['total_symbols']}"
            )
            print(
                f"last snapshot : cycle={left[-1]['cycle']} entropy={left[-1]['entropy']:.6f} total_symbols={left[-1]['total_symbols']}"
            )
        return 0

    idx, a, b = first_difference
    print(f"result: first difference at snapshot #{idx}")
    print(
        "left : "
        f"cycle={a['cycle']} entropy={a['entropy']:.6f} total_symbols={a['total_symbols']} line={a['lineno']}"
    )
    print(
        "right: "
        f"cycle={b['cycle']} entropy={b['entropy']:.6f} total_symbols={b['total_symbols']} line={b['lineno']}"
    )

    top_count = max(len(a["top"]), len(b["top"]))
    for i in range(top_count):
        left_entry = a["top"][i] if i < len(a["top"]) else None
        right_entry = b["top"][i] if i < len(b["top"]) else None
        if left_entry != right_entry:
            print(f"top entry difference at rank {i}:")
            print(f"  left : {left_entry}")
            print(f"  right: {right_entry}")
            break

    return 2


def main():
    parser = argparse.ArgumentParser(
        description="Compare codebook snapshot sections between two GPGPU-Sim run.out files."
    )
    parser.add_argument("left")
    parser.add_argument("right")
    parser.add_argument(
        "--tolerance",
        type=float,
        default=1e-12,
        help="Absolute tolerance for floating-point comparisons.",
    )
    args = parser.parse_args()

    sys.exit(compare(args.left, args.right, args.tolerance))


if __name__ == "__main__":
    main()
