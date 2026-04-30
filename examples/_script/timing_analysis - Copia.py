"""
timing_analysis.py
==================
Computational performance analysis for MUSE-compute (SGS algorithm).
Produces publication-ready figures for scientific papers.

Each experiment is a single timing_summary.csv produced by one MUSE-compute run.
The script aggregates multiple runs and plots:
  Fig 1 — SGS time vs number of simulations   (multi-curve: one per mesh resolution)
  Fig 2 — SGS time vs number of mesh cells     (multi-curve: one per n_simulations)
  Fig 3 — SGS time vs number of input points   (multi-curve: one per n_simulations)

----------------------------------------------------------------------
USAGE — single file (quick inspection)
----------------------------------------------------------------------
  python timing_analysis.py \\
      --file path/to/timing_summary.csv \\
      --n_cells 233888 --n_sims 10 --n_input 4000

----------------------------------------------------------------------
USAGE — benchmark dataset (multiple runs → full figures)
----------------------------------------------------------------------
Provide a benchmark CSV that aggregates all runs:
  n_cells, n_sims, n_input, time_seconds
  233888,  10,    4000,   100.07
  233888,  50,    4000,   498.3
  ...

  python timing_analysis.py \\
      --benchmark benchmark.csv \\
      --output_dir ./plots --format pdf --dpi 300

----------------------------------------------------------------------
USAGE — demo (synthetic data, no files needed)
----------------------------------------------------------------------
  python timing_analysis.py --demo --output_dir ./plots
"""

import argparse
import re
import sys
from pathlib import Path

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from scipy import stats


# ══════════════════════════════════════════════════════════════════════
# STYLE
# ══════════════════════════════════════════════════════════════════════

STYLE = {
    "figure.dpi":           150,
    "figure.facecolor":     "white",
    "axes.facecolor":       "white",
    "axes.grid":            True,
    "grid.color":           "#e0e0e0",
    "grid.linewidth":       0.6,
    "grid.linestyle":       "--",
    "axes.spines.top":      False,
    "axes.spines.right":    False,
    "axes.linewidth":       0.8,
    "axes.labelsize":       11,
    "axes.titlesize":       11,
    "axes.titleweight":     "bold",
    "xtick.labelsize":      9,
    "ytick.labelsize":      9,
    "legend.fontsize":      9,
    "legend.framealpha":    0.92,
    "legend.edgecolor":     "#cccccc",
    "lines.linewidth":      1.2,
    "lines.markersize":     3.5,
    "font.family":          "sans-serif",
    "font.sans-serif":      ["Helvetica", "Arial", "DejaVu Sans"],
    "mathtext.fontset":     "cm",
}

# Okabe-Ito colour-blind safe palette
#PALETTE = ["#0072B2", "#E69F00", "#009E73", "#CC79A7", "#D55E00", "#56B4E9", "#F0E442"]
#PALETTE = ["#08306b", "#2171b5", "#6baed6", "#c6dbef", "#fc8d59", "#d73027", "#a50026"] ##stile JGR/AGU
PALETTE = ["#333333", "#666666", "#999999", "#1a6faf", "#c0392b", "#27ae60", "#8e44ad"] ##Nature/Science style
MARKERS = ["o", "s", "^", "D", "v", "P", "X"]


# ══════════════════════════════════════════════════════════════════════
# HELPERS
# ══════════════════════════════════════════════════════════════════════

def fmt_time(s: float) -> str:
    """Format seconds into a human-readable string."""
    if s >= 3600:
        return f"{s / 3600:.2f} h"
    if s >= 60:
        return f"{s / 60:.2f} min"
    return f"{s:.2f} s"


def load_single_timing(path: str, phase: str = "SGS_simulation") -> float:
    """
    Load a timing_summary.csv produced by MUSE-compute and extract
    the elapsed time for *phase*.  Returns time in seconds.
    """
    df = pd.read_csv(path)
    df.columns = [c.strip() for c in df.columns]
    row = df[df["phase"].str.strip() == phase]
    if row.empty:
        raise ValueError(f"Phase '{phase}' not found in {path}.\n"
                         f"Available phases: {df['phase'].tolist()}")
    return float(row["time_seconds"].iloc[0])


def load_benchmark(path: str) -> pd.DataFrame:
    """
    Load the aggregated benchmark CSV.
    Required columns: n_cells, n_sims, n_input, time_seconds
    Optional column:  time_std  (standard deviation across repeated runs)
    """
    df = pd.read_csv(path)
    required = {"n_cells", "n_sims", "n_input", "time_seconds"}
    missing = required - set(df.columns)
    if missing:
        raise ValueError(f"Benchmark CSV is missing columns: {missing}")
    if "time_std" not in df.columns:
        df["time_std"] = np.nan
    print(f"[INFO] Benchmark loaded: {len(df)} rows from '{path}'")
    return df


# ══════════════════════════════════════════════════════════════════════
# DEMO DATA
# ══════════════════════════════════════════════════════════════════════

def generate_demo_data() -> pd.DataFrame:
    """
    Synthetic benchmark data that mimics realistic SGS scaling.
    Model:  T ≈ α · N_cells^0.85 · N_sims^1.0 · N_input^0.25
    """
    rng = np.random.default_rng(42)

    cells_list  = [10_000, 50_000, 100_000, 233_888, 500_000]
    sims_list   = [5, 10, 25, 50, 100]
    input_list  = [500, 1_000, 2_000, 4_000, 8_000]

    ALPHA = 3e-9  # calibrated so that (233888, 10, 4000) ≈ 100 s

    rows = []

    # Vary n_sims — fixed cells & input
    for nc in cells_list:
        for ns in sims_list:
            ni = 4_000
            t  = ALPHA * (nc ** 0.85) * ns * (ni ** 0.25)
            t *= rng.lognormal(0, 0.04)
            rows.append({"n_cells": nc, "n_sims": ns, "n_input": ni,
                         "time_seconds": t, "time_std": t * 0.03,
                         "vary": "n_sims"})

    # Vary n_input — fixed cells & sims
    for ni in input_list:
        for ns in sims_list:
            nc = 233_888
            t  = ALPHA * (nc ** 0.85) * ns * (ni ** 0.25)
            t *= rng.lognormal(0, 0.04)
            rows.append({"n_cells": nc, "n_sims": ns, "n_input": ni,
                         "time_seconds": t, "time_std": t * 0.03,
                         "vary": "n_input"})

    df = pd.DataFrame(rows)
    print(f"[DEMO] Generated {len(df)} synthetic benchmark rows.")
    return df


# ══════════════════════════════════════════════════════════════════════
# PLOT FUNCTIONS
# ══════════════════════════════════════════════════════════════════════

def _error_bars(ax, x, y, yerr, color, marker, label):
    """Plot line + optional error bars."""
    if np.all(np.isnan(yerr)):
        ax.plot(x, y, marker=marker, color=color, label=label)
    else:
        ax.errorbar(x, y, yerr=yerr, marker=marker, color=color,
                    label=label, capsize=3, capthick=0.8,
                    elinewidth=0.8, linewidth=1.8, markersize=3.5)


def _annotate_config(ax, text: str):
    ax.text(0.97, 0.05, text, transform=ax.transAxes, fontsize=7.5,
            ha="right", va="bottom",
            bbox=dict(boxstyle="round,pad=0.3", facecolor="#f5f5f5", alpha=0.85))


def _power_law_annotation(ax, x_vals, y_vals, pos=(0.05, 0.95)):
    """Fit y = a·x^b in log-log and annotate the exponent."""
    mask = (x_vals > 0) & (y_vals > 0)
    if mask.sum() < 3:
        return
    lx = np.log10(x_vals[mask].astype(float))
    ly = np.log10(y_vals[mask].astype(float))
    slope, _, r, *_ = stats.linregress(lx, ly)
    ax.text(pos[0], pos[1], f"slope ≈ {slope:.2f}  (log–log)",
            transform=ax.transAxes, fontsize=8.5, va="top", ha="left",
            bbox=dict(boxstyle="round,pad=0.3", facecolor="#fff9e6", alpha=0.9))


# ─────────────────────────────────────────────────────────────────────
# Fig 1 — single run: time breakdown bar chart + cumulative info
# ─────────────────────────────────────────────────────────────────────

def plot_single_run(timing_path: str, n_cells: int, n_sims: int,
                    n_input: int, output_dir: Path,
                    phase: str = "SGS_simulation",
                    dpi: int = 300, fmt: str = "pdf"):
    """
    Visual summary of a single MUSE-compute run.
    Shows all phases as a horizontal bar chart and highlights SGS time.
    """
    df = pd.read_csv(timing_path)
    df.columns = [c.strip() for c in df.columns]
    df["time_seconds"] = pd.to_numeric(df["time_seconds"], errors="coerce")
    df = df.dropna(subset=["time_seconds"])
    df = df.sort_values("time_seconds", ascending=True)

    colors = [PALETTE[1] if p == phase else "#bdbdbd" for p in df["phase"]]

    fig, ax = plt.subplots(figsize=(8, max(3, len(df) * 0.55)))
    bars = ax.barh(df["phase"], df["time_seconds"], color=colors,
                   edgecolor="white", linewidth=0.5)

    for bar, val in zip(bars, df["time_seconds"]):
        ax.text(bar.get_width() + df["time_seconds"].max() * 0.01,
                bar.get_y() + bar.get_height() / 2,
                fmt_time(val), va="center", ha="left", fontsize=8)

    ax.set_xlabel("Elapsed time (s)")
    ax.set_title("Computational time — MUSE-compute run summary", pad=10)
    ax.xaxis.set_major_formatter(ticker.FuncFormatter(lambda x, _: fmt_time(x)))

    cfg = (f"Mesh cells: {n_cells:,}   |   Simulations: {n_sims}"
           f"   |   Input points: {n_input:,}")
    fig.text(0.5, -0.04, cfg, ha="center", fontsize=8.5, color="#444444")

    # Legend patch for SGS highlight
    from matplotlib.patches import Patch
    legend_els = [Patch(facecolor=PALETTE[1], label=phase),
                  Patch(facecolor="#bdbdbd", label="Other phases")]
    ax.legend(handles=legend_els, loc="lower right", fontsize=8)

    fig.tight_layout()
    out = output_dir / f"fig0_single_run_summary.{fmt}"
    fig.savefig(out, dpi=dpi, bbox_inches="tight")
    print(f"[SAVED] {out}")
    plt.close(fig)


# ─────────────────────────────────────────────────────────────────────
# Fig 1 — SGS time vs N simulations  (multi-curve: mesh resolution)
# ─────────────────────────────────────────────────────────────────────

def plot_time_vs_nsims(df: pd.DataFrame, output_dir: Path,
                       fixed_n_input: int,
                       dpi: int = 300, fmt: str = "pdf"):
    """
    SGS wall-clock time vs number of simulations.
    One curve per mesh resolution (n_cells).
    Fixed: n_input = fixed_n_input
    """
    sub = df[df["n_input"] == fixed_n_input].copy()
    if sub.empty:
        print(f"[WARN] No data for n_input={fixed_n_input}. Skipping Fig 1.")
        return

    cells_vals = sorted(sub["n_cells"].unique())
    fig, axes = plt.subplots(1, 2, figsize=(11, 4.5))

    for i, nc in enumerate(cells_vals):
        grp = sub[sub["n_cells"] == nc].sort_values("n_sims")
        x   = grp["n_sims"].values
        y   = grp["time_seconds"].values / 60       # → minutes
        err = grp["time_std"].values / 60
        c   = PALETTE[i % len(PALETTE)]
        mk  = MARKERS[i % len(MARKERS)]
        lbl = f"{nc:,} cells"
        for ax in axes:
            _error_bars(ax, x, y, err, c, mk, lbl)

    # Linear fit annotation on the largest mesh
    nc_max = max(cells_vals)
    grp_max = sub[sub["n_cells"] == nc_max].sort_values("n_sims")
    sl, ic, r, *_ = stats.linregress(grp_max["n_sims"],
                                      grp_max["time_seconds"] / 60)
    axes[0].text(0.05, 0.95,
                 f"Linear fit (largest mesh): R² = {r**2:.3f}",
                 transform=axes[0].transAxes, fontsize=8,
                 va="top", bbox=dict(boxstyle="round,pad=0.3",
                                     facecolor="#fff9e6", alpha=0.9))

    axes[0].set_xlabel("Number of simulations")
    axes[0].set_ylabel("SGS wall-clock time (min)")
    axes[0].set_title("(a) Linear scale")
    axes[0].legend(title="Mesh resolution", ncol=1)
    _annotate_config(axes[0], f"Input points: {fixed_n_input:,}")

    axes[1].set_xscale("log"); axes[1].set_yscale("log")
    axes[1].set_xlabel("Number of simulations")
    axes[1].set_ylabel("SGS wall-clock time (min)")
    axes[1].set_title("(b) Log–log scale")
    axes[1].legend(title="Mesh resolution", ncol=1)
    _annotate_config(axes[1], f"Input points: {fixed_n_input:,}")

    # Power-law annotation on log-log (largest mesh)
    _power_law_annotation(axes[1],
                          grp_max["n_sims"].values,
                          grp_max["time_seconds"].values / 60)

    fig.suptitle("SGS Computational Scalability — Number of Simulations",
                 fontweight="bold", y=1.01)
    fig.tight_layout()
    out = output_dir / f"fig1_time_vs_nsims.{fmt}"
    fig.savefig(out, dpi=dpi, bbox_inches="tight")
    print(f"[SAVED] {out}")
    plt.close(fig)


# ─────────────────────────────────────────────────────────────────────
# Fig 2 — SGS time vs mesh cells  (multi-curve: n_simulations)
# ─────────────────────────────────────────────────────────────────────

def plot_time_vs_ncells(df: pd.DataFrame, output_dir: Path,
                        fixed_n_input: int,
                        dpi: int = 300, fmt: str = "pdf"):
    """
    SGS wall-clock time vs mesh resolution (number of cells).
    One curve per number of simulations.
    Fixed: n_input = fixed_n_input
    """
    sub = df[df["n_input"] == fixed_n_input].copy()
    if sub.empty:
        print(f"[WARN] No data for n_input={fixed_n_input}. Skipping Fig 2.")
        return

    sims_vals = sorted(sub["n_sims"].unique())
    fig, axes = plt.subplots(1, 2, figsize=(11, 4.5))

    for i, ns in enumerate(sims_vals):
        grp = sub[sub["n_sims"] == ns].sort_values("n_cells")
        x   = grp["n_cells"].values
        y   = grp["time_seconds"].values / 60
        err = grp["time_std"].values / 60
        c   = PALETTE[i % len(PALETTE)]
        mk  = MARKERS[i % len(MARKERS)]
        lbl = f"{ns} sim."
        for ax in axes:
            _error_bars(ax, x, y, err, c, mk, lbl)

    axes[0].set_xlabel("Number of mesh cells")
    axes[0].set_ylabel("SGS wall-clock time (min)")
    axes[0].set_title("(a) Linear scale")
    axes[0].legend(title="No. of simulations", ncol=2)
    axes[0].xaxis.set_major_formatter(
        ticker.FuncFormatter(lambda x, _: f"{int(x):,}"))
    _annotate_config(axes[0], f"Input points: {fixed_n_input:,}")

    axes[1].set_xscale("log"); axes[1].set_yscale("log")
    axes[1].set_xlabel("Number of mesh cells")
    axes[1].set_ylabel("SGS wall-clock time (min)")
    axes[1].set_title("(b) Log–log scale")
    axes[1].legend(title="No. of simulations", ncol=2)
    _annotate_config(axes[1], f"Input points: {fixed_n_input:,}")

    # Power-law annotation on the largest n_sims curve
    ns_max = max(sims_vals)
    grp_max = sub[sub["n_sims"] == ns_max].sort_values("n_cells")
    _power_law_annotation(axes[1],
                          grp_max["n_cells"].values,
                          grp_max["time_seconds"].values / 60)

    fig.suptitle("SGS Computational Scalability — Mesh Resolution",
                 fontweight="bold", y=1.01)
    fig.tight_layout()
    out = output_dir / f"fig2_time_vs_ncells.{fmt}"
    fig.savefig(out, dpi=dpi, bbox_inches="tight")
    print(f"[SAVED] {out}")
    plt.close(fig)


# ─────────────────────────────────────────────────────────────────────
# Fig 3 — SGS time vs n_input  (multi-curve: n_simulations)
# ─────────────────────────────────────────────────────────────────────

def plot_time_vs_ninput(df: pd.DataFrame, output_dir: Path,
                        fixed_n_cells: int,
                        dpi: int = 300, fmt: str = "pdf"):
    """
    SGS wall-clock time vs number of conditioning input points.
    One curve per number of simulations.
    Fixed: n_cells = fixed_n_cells
    """
    sub = df[df["n_cells"] == fixed_n_cells].copy()
    if sub.empty:
        print(f"[WARN] No data for n_cells={fixed_n_cells}. Skipping Fig 3.")
        return

    sims_vals = sorted(sub["n_sims"].unique())
    fig, axes = plt.subplots(1, 2, figsize=(11, 4.5))

    for i, ns in enumerate(sims_vals):
        grp = sub[sub["n_sims"] == ns].sort_values("n_input")
        x   = grp["n_input"].values
        y   = grp["time_seconds"].values / 60
        err = grp["time_std"].values / 60
        c   = PALETTE[i % len(PALETTE)]
        mk  = MARKERS[i % len(MARKERS)]
        lbl = f"{ns} sim."
        for ax in axes:
            _error_bars(ax, x, y, err, c, mk, lbl)

    axes[0].set_xlabel("Number of samples")
    axes[0].set_ylabel("SGS wall-clock time (min)")
    axes[0].set_title("(a) Linear scale")
    axes[0].legend(title="No. of simulations", ncol=2)
    axes[0].xaxis.set_major_formatter(
        ticker.FuncFormatter(lambda x, _: f"{int(x):,}"))
    _annotate_config(axes[0], f"Mesh cells: {fixed_n_cells:,}")

    axes[1].set_xscale("log"); axes[1].set_yscale("log")
    axes[1].set_xlabel("Number of samples")
    axes[1].set_ylabel("SGS wall-clock time (min)")
    axes[1].set_title("(b) Log–log scale")
    axes[1].legend(title="No. of simulations", ncol=2)
    _annotate_config(axes[1], f"Mesh cells: {fixed_n_cells:,}")

    ns_max = max(sims_vals)
    grp_max = sub[sub["n_sims"] == ns_max].sort_values("n_input")
    _power_law_annotation(axes[1],
                          grp_max["n_input"].values,
                          grp_max["time_seconds"].values / 60)

    fig.suptitle("SGS Computational Scalability — Conditioning Data",
                 fontweight="bold", y=1.01)
    fig.tight_layout()
    out = output_dir / f"fig3_time_vs_ninput.{fmt}"
    fig.savefig(out, dpi=dpi, bbox_inches="tight")
    print(f"[SAVED] {out}")
    plt.close(fig)


# ─────────────────────────────────────────────────────────────────────
# Summary table
# ─────────────────────────────────────────────────────────────────────

def plot_summary_table(df: pd.DataFrame, output_dir: Path,
                       dpi: int = 150, fmt: str = "pdf"):
    """Styled summary table of all benchmark runs."""
    tbl_df = df[["n_cells", "n_sims", "n_input", "time_seconds", "time_std"]].copy()
    tbl_df["time_min"]     = (tbl_df["time_seconds"] / 60).round(3)
    tbl_df["time_std_min"] = (tbl_df["time_std"] / 60).round(4)
    tbl_df = tbl_df.sort_values(["n_cells", "n_sims", "n_input"])

    col_labels = ["Mesh cells", "N sim", "Input pts",
                  "Time (min)", "Std (min)"]
    cell_data  = tbl_df[["n_cells", "n_sims", "n_input",
                          "time_min", "time_std_min"]].values

    # Format integers with thousands separator
    fmt_cell = []
    for row in cell_data:
        fmt_cell.append([
            f"{int(row[0]):,}", f"{int(row[1])}", f"{int(row[2]):,}",
            f"{row[3]:.3f}", f"{row[4]:.4f}" if not np.isnan(row[4]) else "—"
        ])

    n_rows = len(fmt_cell)
    fig, ax = plt.subplots(figsize=(10, max(2.5, n_rows * 0.38 + 1.2)))
    ax.axis("off")

    tbl = ax.table(cellText=fmt_cell, colLabels=col_labels,
                   cellLoc="center", loc="center")
    tbl.auto_set_font_size(False)
    tbl.set_fontsize(9)
    tbl.scale(1, 1.45)

    HDR = "#1a3a5c"
    for j in range(len(col_labels)):
        tbl[0, j].set_facecolor(HDR)
        tbl[0, j].set_text_props(color="white", fontweight="bold")
    for i in range(1, n_rows + 1):
        fc = "#f0f4f8" if i % 2 == 0 else "white"
        for j in range(len(col_labels)):
            tbl[i, j].set_facecolor(fc)

    ax.set_title("SGS Computational Performance — Benchmark Summary",
                 fontweight="bold", pad=14)
    fig.tight_layout()
    out = output_dir / f"table_summary.{fmt}"
    fig.savefig(out, dpi=dpi, bbox_inches="tight")
    print(f"[SAVED] {out}")
    plt.close(fig)


# ══════════════════════════════════════════════════════════════════════
# MAIN
# ══════════════════════════════════════════════════════════════════════

def main():
    parser = argparse.ArgumentParser(
        description="MUSE-compute SGS timing analysis — publication figures",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )

    # ── Single-run mode ──────────────────────────────────────────────
    parser.add_argument("--file",     help="Path to a single timing_summary.csv")
    parser.add_argument("--n_cells",  type=int, default=0,
                        help="Number of mesh cells for this run")
    parser.add_argument("--n_sims",   type=int, default=0,
                        help="Number of simulations for this run")
    parser.add_argument("--n_input",  type=int, default=0,
                        help="Number of conditioning input points for this run")

    # ── Benchmark mode ───────────────────────────────────────────────
    parser.add_argument("--benchmark",
                        help="Aggregated benchmark CSV "
                             "(columns: n_cells, n_sims, n_input, time_seconds[, time_std])")
    parser.add_argument("--fixed_n_input",  type=int, default=None,
                        help="Fixed n_input value for Figs 1 & 2 (default: most common)")
    parser.add_argument("--fixed_n_cells",  type=int, default=None,
                        help="Fixed n_cells value for Fig 3 (default: most common)")

    # ── Demo mode ────────────────────────────────────────────────────
    parser.add_argument("--demo", action="store_true",
                        help="Run with synthetic data (no files required)")

    # ── Output ───────────────────────────────────────────────────────
    parser.add_argument("--phase",      default="SGS_simulation",
                        help="Phase name to extract (default: SGS_simulation)")
    parser.add_argument("--output_dir", default="./plots")
    parser.add_argument("--format",     default="pdf",
                        choices=["pdf", "png", "svg"])
    parser.add_argument("--dpi",        type=int, default=300)

    args = parser.parse_args()

    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)
    plt.rcParams.update(STYLE)

    # ────────────────────────────────────────────────────────────────
    # DEMO
    # ────────────────────────────────────────────────────────────────
    if args.demo:
        print("=" * 60)
        print("DEMO MODE — synthetic benchmark data")
        print("=" * 60)
        df = generate_demo_data()
        fixed_ni = 4_000
        fixed_nc = 233_888

        plot_time_vs_nsims(df, output_dir, fixed_n_input=fixed_ni,
                           dpi=args.dpi, fmt=args.format)
        plot_time_vs_ncells(df, output_dir, fixed_n_input=fixed_ni,
                            dpi=args.dpi, fmt=args.format)
        plot_time_vs_ninput(df, output_dir, fixed_n_cells=fixed_nc,
                            dpi=args.dpi, fmt=args.format)
        plot_summary_table(df, output_dir, fmt=args.format)
        print(f"\n[DONE] Figures saved in: {output_dir}")
        return

    # ────────────────────────────────────────────────────────────────
    # SINGLE FILE
    # ────────────────────────────────────────────────────────────────
    if args.file:
        if not args.n_cells or not args.n_sims or not args.n_input:
            parser.error("--file requires --n_cells, --n_sims and --n_input.")
        t = load_single_timing(args.file, phase=args.phase)
        print(f"\n[RESULT]  {args.phase}  →  {fmt_time(t)}")
        print(f"          Mesh cells : {args.n_cells:,}")
        print(f"          Simulations: {args.n_sims}")
        print(f"          Input pts  : {args.n_input:,}")

        # Add this run to a one-row DataFrame and draw a single-run bar chart
        plot_single_run(args.file, args.n_cells, args.n_sims, args.n_input,
                        output_dir, phase=args.phase,
                        dpi=args.dpi, fmt=args.format)
        print(f"\n[DONE] Figure saved in: {output_dir}")
        return

    # ────────────────────────────────────────────────────────────────
    # BENCHMARK CSV
    # ────────────────────────────────────────────────────────────────
    if args.benchmark:
        df = load_benchmark(args.benchmark)

        fixed_ni = (args.fixed_n_input
                    if args.fixed_n_input is not None
                    else int(df["n_input"].mode()[0]))
        fixed_nc = (args.fixed_n_cells
                    if args.fixed_n_cells is not None
                    else int(df["n_cells"].mode()[0]))

        print(f"[INFO] Fixed n_input for Figs 1 & 2 : {fixed_ni:,}")
        print(f"[INFO] Fixed n_cells for Fig 3       : {fixed_nc:,}")

        plot_time_vs_nsims(df, output_dir, fixed_n_input=fixed_ni,
                           dpi=args.dpi, fmt=args.format)
        plot_time_vs_ncells(df, output_dir, fixed_n_input=fixed_ni,
                            dpi=args.dpi, fmt=args.format)
        plot_time_vs_ninput(df, output_dir, fixed_n_cells=fixed_nc,
                            dpi=args.dpi, fmt=args.format)
        plot_summary_table(df, output_dir, fmt=args.format)
        print(f"\n[DONE] Figures saved in: {output_dir}")
        return

    parser.print_help()
    sys.exit(1)


if __name__ == "__main__":
    main()