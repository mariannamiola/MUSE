# state file generated using paraview version 5.10.0-RC1

# uncomment the following three lines to ensure this script works in future versions
#import paraview
#paraview.compatibility.major = 5
#paraview.compatibility.minor = 10

import os

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

# ----------------------------------------------------------------
# setup the data processing pipelines
# ----------------------------------------------------------------

script_dir=os.path.dirname(os.path.realpath(__file__))
csv_file=script_dir+'/../../MUSE_test/15_soil_thickness/out/compute/D_OMNI3D_boundary_tri/_varspace/_stats/D_mean.csv'
obj_file=script_dir+'/../../MUSE_test/15_soil_thickness/out/geometry/surf/boundary_tri.obj'

print(csv_file)
print(obj_file)

# create a new 'CSV Reader'
tcsv = CSVReader(registrationName='D.csv', FileName=[csv_file])
tcsv.UseStringDelimiter = 0
tcsv.HaveHeaders = 0
tcsv.FieldDelimiterCharacters = ''
tcsv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints1 = TableToPoints(registrationName='TableToPoints1', Input=tcsv)
tableToPoints1.XColumn = 'Field 0'
tableToPoints1.YColumn = 'Field 0'
tableToPoints1.ZColumn = 'Field 0'
tableToPoints1.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate1 = Triangulate(registrationName='Triangulate1', Input=tableToPoints1)

# create a new 'Point Data to Cell Data'
pointDatatoCellData1 = PointDatatoCellData(registrationName='PointDatatoCellData1', Input=triangulate1)
pointDatatoCellData1.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData1.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
nisobj = WavefrontOBJReader(registrationName='nis.obj', FileName=obj_file)

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[nisobj, pointDatatoCellData1])

# ----------------------------------------------------------------
# restore active source
SetActiveSource(appendAttributes1)
# ----------------------------------------------------------------

# get active source.
appendAttributes1 = GetActiveSource()

# get active view
renderView1 = GetActiveViewOrCreate('RenderView')

appendAttributes1Display = Show(appendAttributes1, renderView1, 'GeometryRepresentation')
field0LUT = GetColorTransferFunction('Field0')
appendAttributes1Display.Representation = 'Surface'
appendAttributes1Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes1Display.LookupTable = field0LUT

# get color legend/bar for field0LUT in view renderView1
field0LUTColorBar = GetScalarBar(field0LUT, renderView1)
field0LUTColorBar.WindowLocation = 'Any Location'
field0LUTColorBar.Position = [0.032031356509884104, 0.04464285714285687]
field0LUTColorBar.Title = 'P'
field0LUTColorBar.ComponentTitle = '[-]'
field0LUTColorBar.HorizontalTitle = 1
field0LUTColorBar.RangeLabelFormat = '%-#7.4g'
field0LUTColorBar.ScalarBarLength = 0.23688775510204108

appendAttributes1Display.SetScalarBarVisibility(renderView1, True)

# set scalar coloring
ColorBy(appendAttributes1Display, ('CELLS', 'Field 0'))

# reset view to fit data bounds
renderView1.ResetCamera()
#renderView1.CameraPosition = [50.0, -219.4766694427885, 12.5]
#renderView1.CameraFocalPoint = [50.0, 1.0, 12.5]
#renderView1.CameraViewUp = [0.0, 0.0, 1.0]


if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')
