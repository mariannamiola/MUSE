#!/usr/bin/env python3
"""
Generate a synthetic 3D dataset with a KNOWN anisotropic variogram (ground truth),
to validate the muse_vario FULL 3D ellipsoid workflow (--dim 3D --dir DIR --vario MODEL).

The field is an exact Gaussian Random Field, sampled at scattered points, built by
Cholesky factorization of the anisotropic exponential covariance:

    C(h) = sill * exp(-3 * h' / range_max) + nugget * delta(h)

where h' is the anisotropic distance computed with the GSLIB setrot() convention
(the same convention used by geostatslib and by the MUSE fitted "range", which is
the EFFECTIVE range: gamma(h) = c0 + c*(1 - exp(-3h/r)) ).

Outputs (in ../data):
    data.csv          - MUSE csv input (X;Y;Z;phi), same layout of the other examples
    ground_truth.json - the exact variogram parameters used for the generation
"""
import json
import os
import numpy as np

# =============================== GROUND TRUTH =================================
RANGE_MAX = 60.0   # effective range along the major axis
RANGE_MIN = 30.0   # effective range along the in-plane minor axis
RANGE_Z   = 12.0   # effective range along the third (sub-vertical) axis
AZIMUTH   = 30.0   # setrot ang1: azimuth of the major axis (degree, clockwise from North)
ROLL      = 15.0   # setrot ang2: dip of the major axis above the horizontal (degree)
PITCH     = 0.0    # setrot ang3: rotation around the major axis (degree)
NUGGET    = 0.10   # nugget variance (iid noise added to the field)
SILL      = 0.90   # structured sill (total variance = SILL + NUGGET = 1)

N_POINTS  = 3000                    # number of scattered samples
BOX       = (150.0, 150.0, 50.0)    # domain extents: X, Y and vertical thickness
SEED      = 42                      # random seed for full reproducibility
# ==============================================================================


def setrot_rows(ang1, ang2, ang3, anis1, anis2):
    """GSLIB setrot(): rows of the anisotropy-scaled rotation matrix."""
    alpha = np.radians(90.0 - ang1)
    beta = np.radians(-ang2)
    theta = np.radians(ang3)
    sina, cosa = np.sin(alpha), np.cos(alpha)
    sinb, cosb = np.sin(beta), np.cos(beta)
    sint, cost = np.sin(theta), np.cos(theta)
    afac1, afac2 = 1.0 / max(anis1, 1e-20), 1.0 / max(anis2, 1e-20)
    return np.array([
        [cosb * cosa, cosb * sina, -sinb],
        [afac1 * (-cost * sina + sint * sinb * cosa),
         afac1 * (cost * cosa + sint * sinb * sina),
         afac1 * (sint * cosb)],
        [afac2 * (sint * sina + cost * sinb * cosa),
         afac2 * (-sint * cosa + cost * sinb * sina),
         afac2 * (cost * cosb)],
    ])


def main():
    rng = np.random.default_rng(SEED)

    # Scattered sample locations: uniform in the box, Z expressed as depth (negative)
    x = rng.uniform(0.0, BOX[0], N_POINTS)
    y = rng.uniform(0.0, BOX[1], N_POINTS)
    z = rng.uniform(-BOX[2], 0.0, N_POINTS)
    pts = np.column_stack([x, y, z])

    # Anisotropic distance: transform once with the scaled rotation, then plain euclidean
    R = setrot_rows(AZIMUTH, ROLL, PITCH, RANGE_MIN / RANGE_MAX, RANGE_Z / RANGE_MAX)
    q = pts @ R.T
    diff = q[:, None, :] - q[None, :, :]
    dist = np.sqrt(np.sum(diff * diff, axis=2))

    # Exact anisotropic exponential covariance (effective-range convention: -3h/r)
    cov = SILL * np.exp(-3.0 * dist / RANGE_MAX)
    cov[np.diag_indices_from(cov)] += 1e-8  # jitter for numerical stability

    # Exact GRF by Cholesky factorization + independent nugget noise
    L = np.linalg.cholesky(cov)
    phi = L @ rng.standard_normal(N_POINTS) + np.sqrt(NUGGET) * rng.standard_normal(N_POINTS)

    # Write the MUSE csv (same 6-line header layout of the other examples)
    out_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "data")
    os.makedirs(out_dir, exist_ok=True)
    csv_path = os.path.join(out_dir, "data.csv")
    with open(csv_path, "w") as f:
        f.write("X;Y;Z;phi\n")
        f.write("m;m;m; \n")
        f.write("R+;R+;D;R\n")
        f.write(";;; \n")
        f.write("X;Y;depth;synthetic gaussian\n")
        f.write(";;; \n")
        for i in range(N_POINTS):
            f.write(f"{x[i]:.2f};{y[i]:.2f};{z[i]:.2f};{phi[i]:.4f}\n")

    # Write the ground truth parameters next to the data
    gt = {
        "model": "EXPONENTIAL",
        "range_max": RANGE_MAX, "range_min": RANGE_MIN, "range_z": RANGE_Z,
        "azimuth": AZIMUTH, "roll": ROLL, "pitch": PITCH,
        "nugget": NUGGET, "sill": SILL,
        "n_points": N_POINTS, "box": BOX, "seed": SEED,
        "note": "ranges are EFFECTIVE ranges; angles follow the GSLIB setrot() convention "
                "(ang1=azimuth, ang2=roll, ang3=pitch), the same used by MUSE EllipsoidParameter",
    }
    gt_path = os.path.join(out_dir, "ground_truth.json")
    with open(gt_path, "w") as f:
        json.dump(gt, f, indent=2)

    print(f"Generated {N_POINTS} samples -> {csv_path}")
    print(f"Ground truth -> {gt_path}")
    print(f"sample variance = {np.var(phi):.3f} (expected ~ {SILL + NUGGET})")


if __name__ == "__main__":
    main()
