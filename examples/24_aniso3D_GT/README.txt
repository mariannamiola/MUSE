24_aniso3D_GT - VALIDATION EXAMPLE FOR THE FULL 3D ANISOTROPIC VARIOGRAM (ELLIPSOID)
====================================================================================

PURPOSE
  Validate the muse_vario FULL 3D workflow (--dim 3D --dir DIR --vario MODEL) against
  a synthetic dataset with KNOWN 3D anisotropy (ground truth).

GROUND TRUTH (see script/00_generate_data.py, fully reproducible with fixed seed)
  - Exact Gaussian Random Field at 3000 scattered points in a 150 x 150 x 50 box,
    simulated by Cholesky factorization of the anisotropic exponential covariance
    (no approximation, unlike sgsim): C(h) = sill * exp(-3 h'/range_max) + nugget.
  - Anisotropic distance h' computed with the GSLIB setrot() convention, the SAME
    convention used by geostatslib/MUSE (ang1=azimuth, ang2=roll, ang3=pitch) and
    the same effective-range convention of the MUSE exponential fit.
  - Parameters: range_max=60, range_min=30, range_z=12, azimuth=30, roll=15,
    pitch=0, nugget=0.1, sill=0.9 (total variance 1), model EXPONENTIAL.
  - Parameters can be changed at the top of 00_generate_data.py.

USAGE
  cd script
  ./10_test_MUSE_24_aniso3D_GT.sh [optional: working directory, default ../MUSE_test]

  The script: (re)generates the data if missing, builds the MUSE project, runs
  muse_vario with the 3D ellipsoid workflow (AUTO plane grid, --pstep 45 -> 13 planes,
  6 in-plane directions) and prints the comparison table ground truth vs fitted
  (semi-axes, angles, nugget, model type, angular error of each principal axis).

FILES
  data/data.csv               synthetic samples (X;Y;Z;phi), MUSE csv layout
  data/ground_truth.json      exact parameters used for the generation
  script/00_generate_data.py  ground truth generator (numpy only)
  script/10_test_MUSE_24_aniso3D_GT.sh   end-to-end test
  script/20_compare_ground_truth.py      comparison ground truth vs fitted ellipsoid

REFERENCE RESULT (seed 42, first validation run)
  parameter     ground truth      fitted    error
  model type     EXPONENTIAL         Exp
  nugget               0.100       0.140    abs err 0.040
  range_max             60.0        50.6    rel err -15.7%
  range_min             30.0        26.0    rel err -13.4%
  range_z               12.0        13.3    rel err +10.6%
  azimuth               30.0        39.8    axis err 9.5 deg
  roll                  15.0        15.5    axis err 9.5 deg
  pitch                  0.0         3.4    axis err 1.1 deg
  (estimation errors of 10-20% on ranges are expected for a single realization
   with 3000 samples; the ellipsoid fit residual is stored in the output JSON)
