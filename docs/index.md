# MUSE Command-Line Options Documentation

**Auto-generated from source code - Do NOT edit manually**

This document is automatically generated from the application source code.
When options are modified in the source files, rebuild the project to update this documentation.



Muse is....
Muse does...
Muse copyright Miola & et al.


....

links...

## Available Applications

The MUSE toolkit consists of the following applications:

- **[muse_compute](muse_compute.md)**: MUSE = Modelling of Uncertainty as a Support of Environment; Compute tool
- **[muse_data](muse_data.md)**: MUSE = Modelling of Uncertainty as a Support of Environment; Data tool
- **[muse_export](muse_export.md)**: MUSE = Modelling of Uncertainty as a Support of Environment; Export tool
- **[muse_geometry](muse_geometry.md)**: MUSE = Modelling of Uncertainty as a Support of Environment; Geometry tool
- **[muse_manipulate](muse_manipulate.md)**: MUSE = Modelling of Uncertainty as a Support of Environment; Manipulate tool
- **[muse_plot](muse_plot.md)**: MUSE = Modelling of Uncertainty as a Support of Environment; Plot tool
- **[muse_project](muse_project.md)**: MUSE = Modelling of Uncertainty as a Support of Environment; Muse Project tool
- **[muse_utility](muse_utility.md)**: MUSE = Modelling of Uncertainty as a Support of Environment; Muse Utility tool
- **[muse_vario](muse_vario.md)**: MUSE = Modelling of Uncertainty as a Support of Environment; Vario tool

## Getting Help

All MUSE applications support the standard help flags:

```bash
# Display help for any tool
<tool> --help
<tool> -h
<tool> /?
```

## Project Structure

When creating a project with `muse_project`, the directory structure will be:

```
MyProject/
├── in/           # Input data directory
└── out/          # Output results directory
    └── MyProject.json  # Project metadata and settings
```

## Tips and Best Practices

1. Use `--help` flag to see all available options for any tool
2. Use absolute paths to avoid confusion with relative paths
3. Project names should not contain spaces; use underscores or hyphens instead
4. Keep projects organized with consistent naming conventions
