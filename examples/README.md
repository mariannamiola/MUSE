# MUSE — Worked examples

This folder collects the reproducible end-to-end examples that accompany the MUSE
paper. Each example builds a MUSE project from scratch (data import → geometry →
variogram → simulation) using only the MUSE command-line tools, and can be run
without any manual step.

## How to run

Every example is self-contained in its own folder with the same layout:

```
<example>/
├── data/                     input data (samples in MUSE csv layout, boundaries, ...)
└── script/
    ├── 10_test_MUSE_<name>.sh   end-to-end pipeline (project → data → geometry → vario → compute)
    ├── 20_visual_2D.py|3D.py    visualization of the results
    └── config_*.json            plotting/analysis configurations
```

To run an example:

```bash
cd <example>/script
./10_test_MUSE_<name>.sh          # optional argument: working directory (default ../MUSE_test)
python 20_visual_2D.py            # or 20_visual_3D.py
```

By default the pipeline creates the MUSE project under `MUSE_test/` inside the example
folder (this is the working/output directory, not an input).

## Summary of the examples

Click the example name to open its folder on GitHub; the pipeline is in
`<example>/script/10_test_MUSE_<name>.sh`.

| Example | Dim. | Samples | Variable(s) | Domain / geometry | MUSE workflow demonstrated |
|---------|------|--------:|-------------|-------------------|----------------------------|
| [`06_sec_2D_400_tri`](06_sec_2D_400_tri) | 2D section | ≈400 | `phi` (continuous), `class` (categorical) | single triangulated x–z section | Data import → surface triangulation → directional normal-score variogram → SGS (`phi`) and SIS (`class`). Reduced dataset (quick run). |
| [`06_sec_2D_4000_tri`](06_sec_2D_4000_tri) | 2D section | ≈4 700 | `phi`, `class` | single triangulated x–z section | Same workflow as above on the full dataset. |
| [`07_sec_anti_2D_4000_md_tri`](07_sec_anti_2D_4000_md_tri) | 2D section (anticline, multi-domain) | ≈15 700 | `phi` | 4 stacked stratigraphic domains (`D_01`–`D_04`) built and merged, per-domain transform | **Multi-domain** workflow: build and merge four surfaces, apply a per-domain stratigraphic transformation, then variogram/SGS domain by domain. |
| [`07_sec_anti_2D_4000_md_tri_uncorrect`](07_sec_anti_2D_4000_md_tri_uncorrect) | 2D section (anticline, multi-domain) | ≈15 700 | `phi` | 4 domains, **no** transform | **Incorrect baseline** of the multi-domain case (no stratigraphic transformation) — for comparison against `07_sec_anti_2D_4000_md_tri`. |
| [`08_anti_2D_400_tri`](08_anti_2D_400_tri) | 2D section (anticline) | ≈400 | `phi` | single folded domain, `PROPORTIONAL` stratigraphic transform | Adds the **stratigraphic (unfolding) coordinate transformation** before variogram/SGS. Reduced dataset (quick run). |
| [`08_anti_2D_4000_tri`](08_anti_2D_4000_tri) | 2D section (anticline) | ≈4 700 | `phi`, `class` | single folded domain, `PROPORTIONAL` transform | Full dataset: `phi` (SGS) + `class` (SIS) simulated in the unfolded domain. |
| [`08_anti_2D_4000_tri_uncorrect`](08_anti_2D_4000_tri_uncorrect) | 2D section (anticline) | ≈4 700 | `phi`, `class` | folded domain, **no** transform | **Incorrect baseline**: same data simulated in the raw geometric coordinates, i.e. ignoring the fold — for comparison against `08_anti_2D_4000_tri`. |
| [`22_Tomography`](22_Tomography) | 3D volume | ≈4 970 | `C` (continuous) | boundary → extruded **hexahedral** volume mesh | Full **3D volumetric** workflow: hex meshing, 3D directional variogram (horizontal `3Dxy` + vertical `3Dz`), SGS/SIS in 3D. |
| [`24_aniso3D_GT`](24_aniso3D_GT) | 3D (validation) | 3 000 (synthetic) | `phi` | 150×150×50 box, **known** anisotropy ellipsoid | **Validation** of the full 3D anisotropic (ellipsoid) variogram fitting against a ground-truth Gaussian random field (see [`24_aniso3D_GT/README.txt`](24_aniso3D_GT/README.txt)). |

### Reading the naming convention

- `sec` = vertical **sec**tion (x–z); `anti` = **anti**cline (folded) structure; `md` = **m**ulti-**d**omain.
- `2D` / `3D` = simulation domain; `tri` = triangulated surface; `400` / `4000` = nominal number of samples (reduced vs full dataset).
- `_uncorrect` = deliberately **incorrect** variant that skips the stratigraphic coordinate transformation, provided as a baseline to illustrate its effect.

### Notes

- The `400` variants of examples `06` and `08` are reduced-size copies meant for a quick smoke test; the `4000` variants use the full datasets.
- The `_uncorrect` variants differ from their correct counterpart only by removing the
  `muse_manipulate -T --sttype ...` stratigraphic transformation step (variogram and
  simulation are then run on the folded geometric coordinates).
- Example [`24_aniso3D_GT`](24_aniso3D_GT) is a quantitative validation: `00_generate_data.py` produces a
  synthetic dataset with known ellipsoid parameters and `20_compare_ground_truth.py`
  reports the fitted-vs-true comparison table.
