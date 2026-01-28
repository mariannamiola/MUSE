## muse_plot

**Description:** MUSE = Modelling of Uncertainty as a Support of Environment; Plot tool

### Usage

```bash
muse_plot [OPTIONS]
```

**Note:** For detailed options, run:
```bash
muse_plot --help
```

---

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
