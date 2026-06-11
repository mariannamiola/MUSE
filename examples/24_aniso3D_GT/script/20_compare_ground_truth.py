#!/usr/bin/env python3
"""
Compare the ellipsoid fitted by muse_vario (--dim 3D) with the synthetic ground truth.

Usage: 20_compare_ground_truth.py <ground_truth.json> <vario_output.json>

Prints a side-by-side table (ground truth vs fitted) for the three semi-axes, the
setrot angles, nugget and model type, plus the angular error of each principal axis
(computed from the direction vectors, so it is insensitive to equivalent angle triplets).

Pure standard library (no numpy required).
"""
import json
import math
import sys


def setrot_pure_rows(ang1, ang2, ang3):
    """GSLIB setrot() rotation rows WITHOUT anisotropy scaling (pure rotation)."""
    alpha = math.radians(90.0 - ang1)
    beta = math.radians(-ang2)
    theta = math.radians(ang3)
    sina, cosa = math.sin(alpha), math.cos(alpha)
    sinb, cosb = math.sin(beta), math.cos(beta)
    sint, cost = math.sin(theta), math.cos(theta)
    return [
        [cosb * cosa, cosb * sina, -sinb],
        [-cost * sina + sint * sinb * cosa, cost * cosa + sint * sinb * sina, sint * cosb],
        [sint * sina + cost * sinb * cosa, -sint * cosa + cost * sinb * sina, cost * cosb],
    ]


def axis_angle_deg(u, v):
    """Angle (degree) between two bidirectional axes (sign-independent)."""
    dot = sum(a * b for a, b in zip(u, v))
    nu = math.sqrt(sum(a * a for a in u))
    nv = math.sqrt(sum(b * b for b in v))
    d = min(1.0, abs(dot) / (nu * nv))
    return math.degrees(math.acos(d))


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)

    with open(sys.argv[1]) as f:
        gt = json.load(f)
    with open(sys.argv[2]) as f:
        out = json.load(f)
    ell = out["ellipsoid"]

    # The common nugget/type shared by all planes is stored in the fitted models
    fit0 = out["planes_vario"][0]["fit_experimental_vario"][0]

    # Principal axes: ground truth from the setrot angles, fitted from the stored vectors
    R_gt = setrot_pure_rows(gt["azimuth"], gt["roll"], gt["pitch"])
    err_max = axis_angle_deg(R_gt[0], ell["max_axis_dir"])
    err_min = axis_angle_deg(R_gt[1], ell["min_axis_dir"])
    err_z = axis_angle_deg(R_gt[2], ell["z_axis_dir"])

    rows = [
        ("model type", gt["model"], fit0["type"], ""),
        ("nugget", f"{gt['nugget']:.3f}", f"{fit0['nugget']:.3f}",
         f"abs err {abs(fit0['nugget'] - gt['nugget']):.3f}"),
        ("range_max", f"{gt['range_max']:.1f}", f"{ell['max_semiaxis']:.1f}",
         f"rel err {100 * (ell['max_semiaxis'] - gt['range_max']) / gt['range_max']:+.1f}%"),
        ("range_min", f"{gt['range_min']:.1f}", f"{ell['min_semiaxis']:.1f}",
         f"rel err {100 * (ell['min_semiaxis'] - gt['range_min']) / gt['range_min']:+.1f}%"),
        ("range_z", f"{gt['range_z']:.1f}", f"{ell['z_semiaxis']:.1f}",
         f"rel err {100 * (ell['z_semiaxis'] - gt['range_z']) / gt['range_z']:+.1f}%"),
        ("azimuth", f"{gt['azimuth']:.1f}", f"{ell['azimuth']:.1f}",
         f"axis err {err_max:.1f} deg"),
        ("roll", f"{gt['roll']:.1f}", f"{ell['roll']:.1f}",
         f"axis err {err_min:.1f} deg"),
        ("pitch", f"{gt['pitch']:.1f}", f"{ell['pitch']:.1f}",
         f"axis err {err_z:.1f} deg"),
    ]

    print()
    print("=================================================================")
    print("====== GROUND TRUTH vs FITTED 3D ANISOTROPY ELLIPSOID ==========")
    print("=================================================================")
    print(f"{'parameter':<12}{'ground truth':>14}{'fitted':>12}    {'error'}")
    print("-" * 65)
    for name, g, f, e in rows:
        print(f"{name:<12}{g:>14}{f:>12}    {e}")
    print("-" * 65)
    print(f"ellipsoid fit residual (mean |p'Ap - 1|): {ell['fit_residual']:.3f}")
    print(f"ellipsoid is_valid: {ell['is_valid']}")
    print("axis err = angle between true and fitted principal axis (bidirectional)")
    print()


if __name__ == "__main__":
    main()
