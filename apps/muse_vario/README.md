# MUSE — muse_vario

Compute experimental and model variograms, including directional analysis and the
full 3D anisotropy (ellipsoid) fitting.

## Command-line reference

The complete, always up-to-date reference for every option of `muse_vario` is
generated from the source code and available at:

- Markdown: ../../docs/md/muse_vario.md
- HTML:     ../../docs/html/muse_vario.html

or directly from the tool:

    muse_vario --help

## Full 3D anisotropic variogram (ellipsoid) — `--dim 3D --dir DIR --vario MODEL`

The directional analysis validated on the XY plane is repeated on arbitrary planes
(each defined by dip azimuth and dip, in degree). The common structure (model type
+ nugget) is selected by free-fitting a spread subset of the valid planes (~10%,
at least 3, covering different orientations) and keeping the candidate with the
lowest weighted MSE; the same structure and nugget are then shared by every
plane/direction. The fitted ranges of all planes are combined to fit the 3D
anisotropy ellipsoid: 3 semi-axes (range_max, range_min, range_z) + azimuth, roll,
pitch (setrot/GSLIB convention, directly usable by the simulation covariance).

- `--planes "dipazimuth!dip,dipazimuth!dip,..."` sets the analysis planes.
- `--planes AUTO` (default) scans the plane orientation space with a constant angular
  step (`--pstep`, default 45 degree): horizontal reference plane (dip 0), tilted
  planes at every intermediate dip over the full azimuth turn (so they do not share
  a common axis), and vertical planes (dip 90) over half a turn.

For each plane the tool saves one plot per computed direction
(`<var>_p<i>_dir<j>.jpeg`) and the rose diagram of ranges with the local anisotropy
ellipse (`<var>_p<i>_RangesDiagram.jpeg`). The JSON metadata stores the per-plane
variograms (`planes_vario`) and the ellipsoid (`ellipsoid`).

Example:

    muse_vario -V -p /path/to/project --var phi --nscore YES --vario MODEL \
        --dim 3D --dir DIR --deg 45 --degtol 22.5 --zdegtol 10 \
        --lagspac VARIABLE --planes AUTO

This workflow is validated end-to-end against a synthetic ground truth in
../../examples/24_aniso3D_GT (see its README.txt).

## Runnable examples

End-to-end workflows that use `muse_vario` are provided under ../../examples
(see ../../examples/README.md).
