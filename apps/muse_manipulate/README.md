# MUSE — muse_manipulate

Extract, project and transform data and geometry: sample extraction from geometry
models, point projection on surfaces / sections / volumes, and stratigraphic
coordinate-system transformation (unfolding).

## Command-line reference

The complete, always up-to-date reference for every option of `muse_manipulate` is
generated from the source code and available at:

- Markdown: ../../docs/md/muse_manipulate.md
- HTML:     ../../docs/html/muse_manipulate.html

or directly from the tool:

    muse_manipulate --help

## Runnable examples

End-to-end workflows that use `muse_manipulate` are provided under ../../examples
(see ../../examples/README.md). The `08_anti_*` and `07_*_md_*` examples show the
stratigraphic coordinate transformation applied before variogram/simulation.


## Robust tetrahedral stratigraphic geometry

`-V --type TET --geom final.vtk` derives columns from the final tetrahedral
boundary, independently of vertex ordering and `--reggrow`. For samples, use
`-V --type SAMPLES --geom final.vtk --sub domain`; this recomputes selection from
original data coordinates and writes original sample IDs to `domain.json`.
Both operations use outward-oriented full-line intersections, local coordinates,
long-double barycentrics, distance tolerances, and a projected BVH. Duplicate
edge/vertex hits merge. Missing/tangent sample columns and outside samples are
excluded; ambiguous intervals and invalid volume columns fail before projection
artifacts are written. Closest-point fallbacks are not used. Coordinates must be
in the mesh reference frame; explicit data rotation is rejected on this path.
The existing surface-only, section and hexahedral projection paths remain available.

Run both projections before transformation. Pass the **same** positive
`--thickness` to `-T --type SAMPLES` and `-T --type VOLUME` (50 for Polcevera).
Without this option, the mean of the volume projection thicknesses is used when
available; standalone surface/section workflows retain their previous mean.
Nonfinite coordinates, sentinel bounds, zero/reversed thickness, and overflow are
rejected. `PROPORTIONAL`, `TRUNCATION` and `ONLAP` retain their formulas;
`COMBINATION` fails explicitly because it has no defined reference surface.
Collapsed/inverted/nonfinite-volume transformed tetrahedra produce aggregate
warnings with example IDs and retain connectivity, allowing downstream work.
This does not guarantee positive transformed cell volumes.

Example (the project already contains original data under `out/data`):

```bash
muse_manipulate -V -p "$WP" --type SAMPLES --geom "$MESH" --sub domain --name domain --prdir Z
muse_manipulate -V -p "$WP" --type TET --geom "$MESH" --name domain --prdir Z
muse_manipulate -T -p "$WP" --type SAMPLES --sub domain --name domain --top domain_top --bot domain_bot --sttype PROPORTIONAL --prdir Z --thickness 50
muse_manipulate -T -p "$WP" --type VOLUME --geom "$MESH" --name domain --top domain_top --bot domain_bot --sttype PROPORTIONAL --prdir Z --thickness 50 --vtk
```

Tests against the independent Python reference (requires its NumPy/SciPy/meshio
dependencies and a C++17 compiler):

```bash
python3 -m unittest discover -s external/muse/tests -v
```

Column tolerance is independent of machine epsilon and of the absolute coordinate
offset. By default it is `1e-9 * max(model axis extents)`. Use the same
`--geom-tolerance VALUE` on both `-V` commands to specify a positive absolute
resolution in model units (for example `0.000001` for one micrometre in a model
expressed in metres). The effective value is printed in the log. This controls
triangle inclusion, duplicate-height merging, sample membership and minimum
column thickness. Features at or below this resolution cannot be distinguished.
Exact algebraic zero tests for vertical faces and degenerate boundary tetrahedra
remain; these do not use a machine-epsilon threshold. Arithmetic still uses
floating-point coordinates and long-double intermediate calculations.
