# MUSE — muse_geometry

Build and process the geometric models used by MUSE: import geospatial data
(vector / raster / point cloud), triangulate or grid surfaces, build volumetric
(tetrahedral / hexahedral / voxel) meshes, apply offsets and transformations, and
convert meshes between formats.

## Command-line reference

The complete, always up-to-date reference for every option of `muse_geometry` is
generated from the source code and available at:

- Markdown: ../../docs/md/muse_geometry.md
- HTML:     ../../docs/html/muse_geometry.html

or directly from the tool:

    muse_geometry --help

## Runnable examples

End-to-end workflows that use `muse_geometry` are provided under ../../examples
(see ../../examples/README.md). For instance, 22_Tomography builds a hexahedral
volume mesh, while the 2D-section examples triangulate the input boundaries.
