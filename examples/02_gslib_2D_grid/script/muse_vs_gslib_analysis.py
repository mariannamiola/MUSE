# =============================================================================
# MUSE vs GSLIB — Publication-Ready Analysis (Elsevier Style)
# =============================================================================

import sys
import warnings
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import AutoMinorLocator
from scipy import stats
from pathlib import Path

warnings.filterwarnings("ignore")

# ─────────────────────────────────────────────
#  GLOBAL STYLE — ELSEVIER-LIKE
# ─────────────────────────────────────────────
plt.rcParams.update({
    "font.family": "serif",
    "font.serif": ["Times New Roman", "Times", "DejaVu Serif"],
    "mathtext.fontset": "stix",
    "font.size": 9,

    "axes.titlesize": 10,
    "axes.labelsize": 9,
    "axes.linewidth": 0.8,

    "xtick.labelsize": 8,
    "ytick.labelsize": 8,
    "xtick.direction": "in",
    "ytick.direction": "in",

    "lines.linewidth": 1.3,

    "legend.fontsize": 8,
    "legend.frameon": False,

    "grid.linewidth": 0.5,
    "grid.alpha": 0.4,

    "figure.dpi": 300,
    "savefig.dpi": 300,

    "pdf.fonttype": 42,
})

# Figure sizes (Elsevier single-column)
FIG_SQUARE = (3.4, 3.4)
FIG_RECT   = (3.4, 2.8)
FIG_TABLE  = (3.4, 4.2)


# ─────────────────────────────────────────────
#  I/O
# ─────────────────────────────────────────────
def load_values(filepath):
    values = []
    with open(filepath, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            try:
                values.append(float(line.split()[0]))
            except:
                continue

    arr = np.asarray(values)
    arr = arr[np.isfinite(arr)]
    arr = arr[(arr != -9999) & (arr != -999)]

    if len(arr) == 0:
        raise ValueError(f"No valid data in {filepath}")

    return arr


# ─────────────────────────────────────────────
#  METRICS
# ─────────────────────────────────────────────
def compute_metrics(muse, gslib):

    n = min(len(muse), len(gslib))
    m_s = np.sort(muse)[:n]
    g_s = np.sort(gslib)[:n]

    ks_stat, ks_p = stats.ks_2samp(muse, gslib)
    mw_stat, mw_p = stats.mannwhitneyu(muse, gslib, alternative="two-sided")

    pearson_r, pear_p = stats.pearsonr(m_s, g_s)
    spearman_r, sp_p = stats.spearmanr(m_s, g_s)

    diff = m_s - g_s
    mae = np.mean(np.abs(diff))
    rmse = np.sqrt(np.mean(diff**2))
    bias = np.mean(diff)
    rel_bias = bias / np.mean(g_s) * 100 if np.mean(g_s) != 0 else np.nan
    skill = 1 - rmse / np.std(g_s) if np.std(g_s) > 0 else np.nan

    slope, intercept, *_ = stats.linregress(g_s, m_s)

    def desc(arr):
        return {
            "n": len(arr),
            "mean": np.mean(arr),
            "std": np.std(arr),
            "cv": np.std(arr)/np.mean(arr)*100 if np.mean(arr) != 0 else np.nan,
            "min": np.min(arr),
            "p10": np.percentile(arr, 10),
            "median": np.median(arr),
            "p90": np.percentile(arr, 90),
            "max": np.max(arr),
            "skew": stats.skew(arr),
            "kurt": stats.kurtosis(arr),
        }

    return {
        "muse": desc(muse),
        "gslib": desc(gslib),
        "ks_stat": ks_stat, "ks_p": ks_p,
        "mw_stat": mw_stat, "mw_p": mw_p,
        "pearson_r": pearson_r, "pear_p": pear_p,
        "spearman_r": spearman_r, "sp_p": sp_p,
        "mae": mae, "rmse": rmse,
        "bias": bias, "rel_bias": rel_bias,
        "skill": skill,
        "slope": slope, "intercept": intercept,
        "n_common": n
    }

# ─────────────────────────────────────────────
#  METRICS - PRINT REPORT
# ─────────────────────────────────────────────
def print_report(m, f1, f2):

    print("\n" + "="*60)
    print("MUSE vs GSLIB — Full Report")
    print("="*60)

    print(f"MUSE : {f1}")
    print(f"GSLIB: {f2}")
    print(f"N common: {m['n_common']}")

    print("\n--- Descriptive statistics ---")
    for key in ["mean","std","cv","min","p10","median","p90","max"]:
        print(f"{key:>10}  MUSE={m['muse'][key]:.4g}   GSLIB={m['gslib'][key]:.4g}")

    print("\n--- QQ metrics ---")
    print(f"Pearson r     : {m['pearson_r']:.4f} (p={m['pear_p']:.3g})")
    print(f"Spearman rho  : {m['spearman_r']:.4f} (p={m['sp_p']:.3g})")
    print(f"MAE           : {m['mae']:.4g}")
    print(f"RMSE          : {m['rmse']:.4g}")
    print(f"Bias          : {m['bias']:+.4g} ({m['rel_bias']:+.2f}%)")
    print(f"Skill score   : {m['skill']:.4f}")
    print(f"Slope         : {m['slope']:.4f}")
    print(f"Intercept     : {m['intercept']:.4g}")

    print("\n--- Distribution tests ---")
    print(f"KS  : {m['ks_stat']:.4f} (p={m['ks_p']:.3g})")
    print(f"MWU : {m['mw_stat']:.4g} (p={m['mw_p']:.3g})")

    print("="*60 + "\n")

# ─────────────────────────────────────────────
#  QQ PLOT
# ─────────────────────────────────────────────
def plot_qq(muse, gslib, metrics, title, out_base):

    n = min(len(muse), len(gslib))
    probs = np.linspace(0, 100, n + 2)[1:-1]

    m_q = np.percentile(muse, probs)
    g_q = np.percentile(gslib, probs)

    fig, ax = plt.subplots(figsize=FIG_SQUARE)

    lo = min(m_q.min(), g_q.min())
    hi = max(m_q.max(), g_q.max())
    pad = (hi - lo) * 0.02

    # 1:1
    ax.plot([lo, hi], [lo, hi], "--", color="black", lw=1)

    # scatter
    ax.scatter(g_q, m_q,
               s=14,
               facecolor="#1A73E8",
               edgecolor="black",
               linewidth=0.3)

    # regression
    x = np.linspace(lo, hi, 200)
    ax.plot(x, metrics["slope"] * x + metrics["intercept"],
            color="black", lw=1.3)

    ax.set_xlim(lo - pad, hi + pad)
    ax.set_ylim(lo - pad, hi + pad)

    ax.set_xlabel("GSLIB quantiles")
    ax.set_ylabel("MUSE quantiles")
    ax.set_title("QQ plot", pad=4)

    ax.grid(True, linestyle=":")

    fig.savefig(out_base + "_qq.pdf", bbox_inches="tight", pad_inches=0.02)
    fig.savefig(out_base + "_qq.png")



# ─────────────────────────────────────────────
#  RESIDUALS
# ─────────────────────────────────────────────
def plot_residuals(muse, gslib, title, out_base):

    n = min(len(muse), len(gslib))
    probs = np.linspace(0, 100, n + 2)[1:-1]

    m_q = np.percentile(muse, probs)
    g_q = np.percentile(gslib, probs)

    diff = m_q - g_q

    fig, ax = plt.subplots(figsize=FIG_RECT)

    ax.axhline(0, linestyle="--", color="black", lw=1)

    ax.scatter(g_q, diff,
               s=14,
               facecolor="#1A73E8",
               edgecolor="black",
               linewidth=0.3)

    ax.set_xlabel("GSLIB quantiles")
    ax.set_ylabel("Residual (MUSE − GSLIB)")
    ax.set_title("Quantile residuals", pad=4)

    ax.grid(True, linestyle=":")

    fig.savefig(out_base + "_residuals.pdf", bbox_inches="tight", pad_inches=0.02)
    fig.savefig(out_base + "_residuals.png")


# ─────────────────────────────────────────────
#  CDF
# ─────────────────────────────────────────────
def plot_cdf(muse, gslib, title, out_base):

    fig, ax = plt.subplots(figsize=FIG_SQUARE)

    for arr, label in [(muse, "MUSE"), (gslib, "GSLIB")]:
        s = np.sort(arr)
        cdf = np.arange(1, len(s)+1)/len(s)
        ax.plot(s, cdf, label=label, lw=1.3)

    ax.set_xlabel("Value")
    ax.set_ylabel("F(x)")
    ax.set_title("Empirical CDF", pad=4)

    ax.legend()
    ax.grid(True, linestyle=":")

    fig.savefig(out_base + "_cdf.pdf", bbox_inches="tight", pad_inches=0.02)
    fig.savefig(out_base + "_cdf.png")


# ─────────────────────────────────────────────
#  TABLE
# ─────────────────────────────────────────────
def plot_table(metrics, out_base):

    fig, ax = plt.subplots(figsize=(3.4, 4.5))
    ax.axis("off")

    m = metrics

    lines = [
        "Descriptive statistics",
        f"Mean: {m['muse']['mean']:.3g} / {m['gslib']['mean']:.3g}",
        f"Std:  {m['muse']['std']:.3g} / {m['gslib']['std']:.3g}",
        f"CV%:  {m['muse']['cv']:.2f} / {m['gslib']['cv']:.2f}",
        "",
        "Quantile metrics",
        f"Pearson r: {m['pearson_r']:.3f}",
        f"Spearman ρ: {m['spearman_r']:.3f}",
        f"RMSE: {m['rmse']:.3g}",
        f"Bias: {m['bias']:+.3g} ({m['rel_bias']:+.2f}%)",
        f"Skill: {m['skill']:.3f}",
        "",
        "Distribution tests",
        f"KS p: {m['ks_p']:.3g}",
        f"MWU p: {m['mw_p']:.3g}",
    ]

    for i, txt in enumerate(lines):
        ax.text(0.05, 0.95 - i*0.065, txt, fontsize=8)

    fig.savefig(out_base + "_table.pdf", bbox_inches="tight")
    fig.savefig(out_base + "_table.png")


# ─────────────────────────────────────────────
#  MAIN
# ─────────────────────────────────────────────
if __name__ == "__main__":

    # ── Set file paths here if not using CLI ──────────────────
    PROJECT_NAME = "02_gslib_2D_grid" 
    VARIABLE_NAME = "p_c"  # adjust if needed

    PROJECT_PATH = "../../MUSE_test/" + PROJECT_NAME + "/out/compute/"  # adjust if needed
    print(f"Project path: {PROJECT_PATH}")

    FILE_MUSE  = PROJECT_PATH + f"{VARIABLE_NAME}_OMNI3D_area/_varspace/_stats/{VARIABLE_NAME}_mean.csv"
    FILE_GSLIB = PROJECT_PATH + f"{VARIABLE_NAME}_OMNI3D_area/_varspace/_stats/{VARIABLE_NAME}_mean-GSLIB.csv"

    OUTPUT_DIR = "_validation"
    Path(OUTPUT_DIR).mkdir(exist_ok=True)

    #OUT = "muse_vs_gslib"
    OUT = str(Path(OUTPUT_DIR) / "muse_vs_gslib")

    if len(sys.argv) >= 3:
        FILE_MUSE = sys.argv[1]
        FILE_GSLIB = sys.argv[2]

    muse = load_values(FILE_MUSE)
    gslib = load_values(FILE_GSLIB)

    metrics = compute_metrics(muse, gslib)

    print_report(metrics, FILE_MUSE, FILE_GSLIB)

    plot_qq(muse, gslib, metrics, "", OUT)
    plot_residuals(muse, gslib, "", OUT)
    plot_cdf(muse, gslib, "", OUT)
    plot_table(metrics, OUT)

    print("Done.")