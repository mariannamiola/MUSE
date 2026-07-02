# state file generated using paraview version 5.10.0-RC1

import os
import json

from paraview.simple import *
paraview.simple._DisableFirstRenderCameraReset()


# ----------------------------------------------------------------
# load config.json
# ----------------------------------------------------------------

script_dir  = os.path.dirname(os.path.realpath(__file__))
config_path = os.path.join(script_dir, "config.json")

with open(config_path, "r") as f:
    config = json.load(f)


# ----------------------------------------------------------------
# setup variabili globali da config
# ----------------------------------------------------------------

work_folder    = config["work_folder"]
project_name   = config["project_name"]
project_folder = os.path.join(work_folder, project_name)

var_name   = config["varname"]
unit       = config["units"]
sample_csv = config["samples"]

geom_name = config["geometry"]["name"]
geom_ext  = config["geometry"]["ext"]
sub       = config["geometry"].get("sub", "")

vario = config["vario"]["dir"] + config["vario"]["dim"]

geom_folder    = os.path.join(project_folder, "out", "geometry", "surf")
compute_folder = os.path.join(project_folder, "out", "compute")

obj_file        = os.path.join(script_dir, "..", "..", geom_folder, f"{geom_name}.{geom_ext}")
samplescsv_file = os.path.join(script_dir, "..", "data", sample_csv)

# ----------------------------------------------------------------
# retrocompatibilità: compute e scalarfields possono essere
# oggetti singoli (vecchio formato) o liste (nuovo formato)
# ----------------------------------------------------------------

compute_list = config["compute"]
if isinstance(compute_list, dict):
    compute_list = [compute_list]

scalarfields_cfg = config["columns"].get("scalarfields")
if scalarfields_cfg is None:
    # retrocompatibilità con il vecchio campo singolo "scalarfield"
    scalarfields_cfg = [config["columns"]["scalarfield"]]

if len(compute_list) != len(scalarfields_cfg):
    raise ValueError(
        f"'compute' ha {len(compute_list)} elementi ma "
        f"'scalarfields' ne ha {len(scalarfields_cfg)}. "
        "Le due liste devono avere la stessa lunghezza."
    )


# ----------------------------------------------------------------
# View
# ----------------------------------------------------------------

renderView1 = GetActiveViewOrCreate('RenderView')
renderView1.ViewSize = [1000, 1000]


# ----------------------------------------------------------------
# OBJ geometry (condivisa da tutti i campi)
# ----------------------------------------------------------------

areaobj = WavefrontOBJReader(registrationName=geom_name, FileName=obj_file)


# ----------------------------------------------------------------
# CSV Reader - sample points
# ----------------------------------------------------------------

samples_cfg = config["columns"]["samples"]

samplescsv = CSVReader(registrationName=sample_csv, FileName=[samplescsv_file])
samplescsv.UseStringDelimiter = 0
samplescsv.HaveHeaders = 1
samplescsv.FieldDelimiterCharacters = ';'

tableToPoints_samples = TableToPoints(
    registrationName='TableToPoints_samples',
    Input=samplescsv
)
tableToPoints_samples.XColumn = samples_cfg["x"]
tableToPoints_samples.YColumn = samples_cfg["y"]
tableToPoints_samples.ZColumn = samples_cfg["z"]
tableToPoints_samples.KeepAllDataArrays = 1


# ----------------------------------------------------------------
# Funzione di supporto: configura LUT categorica o continua
# ----------------------------------------------------------------

def setup_lut(lut, cfg):
    """
    Configura una color transfer function.
    - cat=True  → scala categorica con N classi intere
    - cat=False → scala continua (rescale automatico al range dei dati)
    """
    if cfg.get("cat"):
        lut.InterpretValuesAsCategories = 1
        lut.AnnotationsInitialized      = 1
        lut.ScalarRangeInitialized      = 1.0
        n      = int(cfg["n_cat"])
        lut.NumberOfTableValues = n
        values = [str(i) for i in range(1, n + 1)]
        lut.Annotations          = [item for v in values for item in (v, v)]
        lut.ActiveAnnotatedValues = values
    else:
        lut.RescaleTransferFunctionToDataRange()


# ----------------------------------------------------------------
# Loop principale: un campo scalare per ogni coppia
# (compute_cfg, scalarfield_cfg)
# ----------------------------------------------------------------

first_source = None   # usato dopo per la camera

for idx, (comp_cfg, sf_cfg) in enumerate(zip(compute_list, scalarfields_cfg)):

    label    = sf_cfg.get("label", f"field_{idx}")
    val_col  = sf_cfg["value"]
    sf_units = comp_cfg.get("units", "-")

    # --- costruzione path CSV ---
    comp_geom   = comp_cfg.get("geom", "") or geom_name
    compute_sim = (
        f"{var_name}_{sub}_{vario}_{comp_geom}" if sub
        else f"{var_name}_{vario}_{comp_geom}"
    )
    space = os.path.join(
        comp_cfg.get("space", ""),
        comp_cfg.get("where", "")
    ).strip(os.sep)

    sim_name = var_name + '_' + comp_cfg["var"]

    csv_file = os.path.join(
        script_dir, "..", "..", compute_folder,
        compute_sim, space, f"{sim_name}.csv"
    )

    print(f"[{label}] CSV  : {csv_file}")
    print(f"[{label}] MESH : {obj_file}")

    # --- pipeline CSV → punti → triangolazione → celle ---
    csv_reader = CSVReader(
        registrationName=f"CSVReader_{label}",
        FileName=[csv_file]
    )
    csv_reader.UseStringDelimiter       = 0
    csv_reader.HaveHeaders              = 0
    csv_reader.FieldDelimiterCharacters = ''
    csv_reader.AddTabFieldDelimiter     = 1

    ttp = TableToPoints(
        registrationName=f"TableToPoints_{label}",
        Input=csv_reader
    )
    ttp.XColumn             = sf_cfg["x"]
    ttp.YColumn             = sf_cfg["y"]
    ttp.ZColumn             = sf_cfg["z"]
    ttp.KeepAllDataArrays   = 1

    tri = Triangulate(
        registrationName=f"Triangulate_{label}",
        Input=ttp
    )

    p2c = PointDatatoCellData(
        registrationName=f"P2C_{label}",
        Input=tri
    )
    p2c.PointDataArraytoprocess = val_col
    p2c.PassPointData           = 1

    # --- append con la geometria OBJ ---
    appAttr = AppendAttributes(
        registrationName=f"AppendAttr_{label}",
        Input=[areaobj, p2c]
    )

    if first_source is None:
        first_source = appAttr   # reference per la camera

    SetActiveSource(appAttr)

    # --- display sulla mesh ---
    display = Show(appAttr, renderView1, 'GeometryRepresentation')
    display.Representation  = 'Surface'
    display.ColorArrayName  = ['CELLS', val_col]

    lut = GetColorTransferFunction(val_col)
    setup_lut(lut, sf_cfg)
    display.LookupTable = lut

    ColorBy(display, ('CELLS', val_col))

    # --- color bar ---
    cb = GetScalarBar(lut, renderView1)
    cb.Title            = sim_name
    cb.ComponentTitle   = f'[{sf_units}]'
    cb.HorizontalTitle  = 1
    cb.RangeLabelFormat = '%-#7.3g'
    cb.WindowLocation   = sf_cfg.get("colorbar_position", "Lower Left Corner")

    display.SetScalarBarVisibility(renderView1, True)


# ----------------------------------------------------------------
# Sample points
# ----------------------------------------------------------------

tableToPoints1Display = Show(
    tableToPoints_samples, renderView1, 'GeometryRepresentation'
)
tableToPoints1Display.UseSeparateColorMap = True

p_cLUT = GetColorTransferFunction(
    samples_cfg["value"], tableToPoints1Display, separate=True
)
setup_lut(p_cLUT, samples_cfg)

tableToPoints1Display.Representation = 'Points'
tableToPoints1Display.ColorArrayName = ['POINTS', samples_cfg["value"]]
tableToPoints1Display.LookupTable    = p_cLUT
tableToPoints1Display.PointSize      = 5.0

p_cLUTColorBar = GetScalarBar(p_cLUT, renderView1)
p_cLUTColorBar.Title            = var_name
p_cLUTColorBar.ComponentTitle   = f'[{unit}]'
p_cLUTColorBar.HorizontalTitle  = 1
p_cLUTColorBar.RangeLabelFormat = '%-#7.3g'
p_cLUTColorBar.WindowLocation   = 'Upper Right Corner'

tableToPoints1Display.SetScalarBarVisibility(renderView1, True)


# ----------------------------------------------------------------
# Camera (calcolata sui bounds del primo campo)
# ----------------------------------------------------------------

renderView1.ResetCamera()

view_plane  = config.get("view", {}).get("plane", "XY").upper()
dist_factor = config.get("view", {}).get("distance_factor", 1.5)

bounds = first_source.GetDataInformation().GetBounds()
cx = (bounds[0] + bounds[1]) / 2.0
cy = (bounds[2] + bounds[3]) / 2.0
cz = (bounds[4] + bounds[5]) / 2.0
extent   = max(bounds[1]-bounds[0], bounds[3]-bounds[2], bounds[5]-bounds[4])
distance = extent * dist_factor

camera_settings = {
    "XY": {"pos": [cx, cy, cz + distance], "up": [0, 1, 0]},
    "XZ": {"pos": [cx, cy + distance, cz], "up": [0, 0, 1]},
    "YZ": {"pos": [cx - distance, cy, cz], "up": [0, 0, 1]},
}

cam = camera_settings.get(view_plane, camera_settings["XY"])
renderView1.CameraFocalPoint         = [cx, cy, cz]
renderView1.CameraPosition           = cam["pos"]
renderView1.CameraViewUp             = cam["up"]
renderView1.CameraParallelProjection = 1


# ----------------------------------------------------------------
# Output
# ----------------------------------------------------------------

viewsize = renderView1.ViewSize

if config["output"]["save"]:
    directory = os.path.join(
        script_dir, '..', '..', project_folder, config["output"]["dir"]
    )
    os.makedirs(directory, exist_ok=True)

    # un'unica screenshot con tutti i campi visibili
    out_name = var_name + '_' + '_'.join(
        c["var"] for c in compute_list
    )
    SaveScreenshot(
        os.path.join(directory, out_name + '.' + config["output"]["format"]),
        renderView1,
        ImageResolution=viewsize,
        FontScaling=False,
        OverrideColorPalette='WhiteBackground',
        TransparentBackground=False
    )


if __name__ == '__main__':
    SaveExtracts(ExtractsOutputDirectory='extracts')