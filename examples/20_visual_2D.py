# state file generated using paraview version 5.10.0-RC1

# uncomment the following three lines to ensure this script works in future versions
#import paraview
#paraview.compatibility.major = 5
#paraview.compatibility.minor = 10

import os
import json

#### import the simple module from the paraview
from paraview.simple import *

paraview.simple._DisableFirstRenderCameraReset()


# ----------------------------------------------------------------
# load config.ylm
# ----------------------------------------------------------------

script_dir=os.path.dirname(os.path.realpath(__file__))
config_path = os.path.join(script_dir, "config.json")

with open(config_path, "r") as f:
    config = json.load(f)


# ----------------------------------------------------------------
# setup variables from config
# ----------------------------------------------------------------

work_folder=config["work_folder"]
project_name=config["project_name"]
project_folder=os.path.join(work_folder, project_name)
print(project_folder)

var_name=config["varname"]
unit=config["units"]
sample_csv = config["samples"]

geom_name=config["geometry"]["name"]
geom_ext=config["geometry"]["ext"]
sub = config["geometry"].get("sub", "")

vario=config["vario"]["dir"] + config["vario"]["dim"]
comp_geom = config["compute"].get("geom", "")
if not comp_geom:
    comp_geom = geom_name
print(comp_geom)


## check if sub is empty
if not sub:
    compute_sim=f"{var_name}_{vario}_{comp_geom}"
else:
    compute_sim=f"{var_name}_{sub}_{vario}_{comp_geom}"


geom_folder=os.path.join(project_folder, "out", "geometry", "surf")
compute_folder=os.path.join(project_folder, "out", "compute")
print(geom_folder)
print(compute_folder)

where=config["compute"].get("where", "") ##cartella stats; se vuota, indica che sto prendendo le singole simulazioni
## check if where is empty
if not where:
    space=config["compute"]["space"]
else:
    space=os.path.join(config["compute"]["space"], where)
print(space)

sim_name=var_name + '_'+ config["compute"]["var"]

csv_file = os.path.join(script_dir, "..", "..", compute_folder, compute_sim, space, f"{sim_name}.csv")
obj_file = os.path.join(script_dir, "..", "..", geom_folder, f"{geom_name}.{geom_ext}")
samplescsv_file = os.path.join(script_dir, "..", "data", sample_csv)

print("CSV: ", csv_file)
print("MESH: ", obj_file)
print("Samples CSV: ", samplescsv_file)


# ----------------------------------------------------------------
# CSV Readers - sample points
# ----------------------------------------------------------------

# create a new 'CSV Reader'
samplescsv = CSVReader(registrationName=sample_csv, FileName=[samplescsv_file])
samplescsv.UseStringDelimiter = 0
samplescsv.HaveHeaders = 1
samplescsv.FieldDelimiterCharacters = ';'

# create a new 'Table To Points'
tableToPoints1 = TableToPoints(registrationName='TableToPoints1', Input=samplescsv)
tableToPoints1.XColumn = config["columns"]["samples"]["x"]
tableToPoints1.YColumn = config["columns"]["samples"]["y"]
tableToPoints1.ZColumn = config["columns"]["samples"]["z"]
tableToPoints1.KeepAllDataArrays = 1


# ----------------------------------------------------------------
# CSV Readers - simulation results
# ----------------------------------------------------------------

# create a new 'CSV Reader'
p_ccsv = CSVReader(registrationName=var_name, FileName=[csv_file])
p_ccsv.UseStringDelimiter = 0
p_ccsv.HaveHeaders = 0
p_ccsv.FieldDelimiterCharacters = ''
p_ccsv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints2 = TableToPoints(registrationName='TableToPoints2', Input=p_ccsv)
tableToPoints2.XColumn = config["columns"]["scalarfield"]["x"]
tableToPoints2.YColumn = config["columns"]["scalarfield"]["y"]
tableToPoints2.ZColumn = config["columns"]["scalarfield"]["z"]
tableToPoints2.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate1 = Triangulate(registrationName='Triangulate1', Input=tableToPoints2)

# create a new 'Point Data to Cell Data'
pointDatatoCellData1 = PointDatatoCellData(registrationName='PointDatatoCellData1', Input=triangulate1)
pointDatatoCellData1.PointDataArraytoprocess = config["columns"]["scalarfield"]["value"]
pointDatatoCellData1.PassPointData = 1

# ----------------------------------------------------------------
# OBJ Reader
# ----------------------------------------------------------------

# create a new 'Wavefront OBJ Reader'
areaobj = WavefrontOBJReader(registrationName=geom_name, FileName=obj_file)

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[areaobj, pointDatatoCellData1])


# ----------------------------------------------------------------
# setup the visualization in view 'renderView1'
# ----------------------------------------------------------------

# ----------------------------------------------------------------
# restore active source
SetActiveSource(appendAttributes1)
# ----------------------------------------------------------------

# get active source.
appendAttributes1 = GetActiveSource()

# get active view
renderView1 = GetActiveViewOrCreate('RenderView')

# show data from appendAttributes1
appendAttributes1Display = Show(appendAttributes1, renderView1, 'GeometryRepresentation')

# get color transfer function/color map for 'Field0'
field0LUT = GetColorTransferFunction(config["columns"]["scalarfield"]["value"])

appendAttributes1Display.Representation = 'Surface'
appendAttributes1Display.ColorArrayName = ['CELLS', config["columns"]["scalarfield"]["value"]]
appendAttributes1Display.LookupTable = field0LUT

# get color legend/bar for field0LUT in view renderView1
field0LUTColorBar = GetScalarBar(field0LUT, renderView1)
field0LUTColorBar.Title = var_name + '_' + config["compute"]["var"]
field0LUTColorBar.ComponentTitle = '['+ config["compute"]["units"]+']'
field0LUTColorBar.HorizontalTitle = 1
field0LUTColorBar.RangeLabelFormat = '%-#7.3g'


# show data from tableToPoints1
tableToPoints1Display = Show(tableToPoints1, renderView1, 'GeometryRepresentation')

# get separate color transfer function/color map for points
# set separate color map
tableToPoints1Display.UseSeparateColorMap = True
p_cLUT = GetColorTransferFunction( config["columns"]["samples"]["value"], tableToPoints1Display, separate=True)

#p_cLUT.RGBPoints = [6.85, 0.231373, 0.298039, 0.752941, 100.425, 0.865003, 0.865003, 0.865003, 194.0, 0.705882, 0.0156863, 0.14902]
#p_cLUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
tableToPoints1Display.Representation = 'Points'
tableToPoints1Display.ColorArrayName = ['POINTS', config["columns"]["samples"]["value"]]
tableToPoints1Display.LookupTable = p_cLUT
tableToPoints1Display.PointSize = 5.0

# show color legend
##tableToPoints1Display.SetScalarBarVisibility(renderView1, True)

# rescale the color map for points
p_cLUT.RescaleTransferFunctionToDataRange()

# get color legend/bar for p_cLUT in view renderView1
p_cLUTColorBar = GetScalarBar(p_cLUT, renderView1)
p_cLUTColorBar.Title = var_name
p_cLUTColorBar.ComponentTitle = '['+ unit +']'
p_cLUTColorBar.HorizontalTitle = 1
p_cLUTColorBar.RangeLabelFormat = '%-#7.3g'
p_cLUTColorBar.WindowLocation='Upper Right Corner'

appendAttributes1Display.SetScalarBarVisibility(renderView1, True)
tableToPoints1Display.SetScalarBarVisibility(renderView1, True)



# set scalar coloring
ColorBy(appendAttributes1Display, ('CELLS', config["columns"]["scalarfield"]["value"]))

# reset view to fit data bounds
renderView1.ResetCamera()
#renderView1.CameraPosition = [23.673108484339377, 26.10432893807473, 136.60254037844388]
#renderView1.CameraFocalPoint = [23.673108484339377, 26.10432893807473, 0.0]
#renderView1.CameraFocalDisk = 1.0

# To save a specific target resolution, rather than using the
# the current view (or layout) size, and override the color palette.
if config["output"]["save"]:
    directory = os.path.join(script_dir, '..', '..', project_folder, config["output"]["dir"])
    if not os.path.exists(directory): 
	    os.mkdir(directory)

    SaveScreenshot(os.path.join(directory, sim_name + '.' + config["output"]["format"]), renderView1, ImageResolution=[3000, 3000], FontScaling=True, OverrideColorPalette='WhiteBackground', TransparentBackground=False)
    #SaveScreenshot(os.path.join(directory, sim_name + '.png'), renderView1, FontScaling=True, OverrideColorPalette='WhiteBackground', TransparentBackground=False)


if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')
