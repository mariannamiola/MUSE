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
csv_file=script_dir+'/../../MUSE_test/08_anti_3D_4000_tet/out/compute/phi_anti_0.5-anti_absz_DIR2D_geom_anti/_varsapce/_stats/phi_sgs_mean.csv'
#csv_file=script_dir+'/../../MUSE_test/08_anti_3D_4000_tet/out/compute/phi_anti_0.5-anti_absz_DIR2D_geom_anti/phi_anti_0.5-anti_absz.csv'
vtk_file=script_dir+'/../../MUSE_test/08_anti_3D_4000_tet/out/geometry/volume/anti_rot.vtk'
samplescsv_file=script_dir+'/../data/samples.csv'

#print(csv_file)
#print(obj_file)


# create a new 'CSV Reader'
phi_meancsv = CSVReader(registrationName='phi_mean.csv', FileName=[csv_file])
phi_meancsv.UseStringDelimiter = 0
phi_meancsv.HaveHeaders = 0
phi_meancsv.FieldDelimiterCharacters = ''
phi_meancsv.AddTabFieldDelimiter = 1

# create a new 'CSV Reader'
samplescsv = CSVReader(registrationName='samples.csv', FileName=[samplescsv_file])
samplescsv.UseStringDelimiter = 0
samplescsv.HaveHeaders = 0
samplescsv.FieldDelimiterCharacters = ';'

# create a new 'Table To Points'
tableToPoints1 = TableToPoints(registrationName='TableToPoints1', Input=samplescsv)
tableToPoints1.XColumn = 'Field 0'
tableToPoints1.YColumn = 'Field 1'
tableToPoints1.ZColumn = 'Field 2'

# create a new 'Table To Points'
tableToPoints2 = TableToPoints(registrationName='TableToPoints2', Input=phi_meancsv)
tableToPoints2.XColumn = 'Field 0'
tableToPoints2.YColumn = 'Field 0'
tableToPoints2.ZColumn = 'Field 0'
tableToPoints2.KeepAllDataArrays = 1

# create a new 'Tetrahedralize'
tetrahedralize1 = Tetrahedralize(registrationName='Tetrahedralize1', Input=tableToPoints2)

# create a new 'Point Data to Cell Data'
pointDatatoCellData1 = PointDatatoCellData(registrationName='PointDatatoCellData1', Input=tetrahedralize1)
pointDatatoCellData1.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData1.PassPointData = 1

# create a new 'Legacy VTK Reader'
SEC_rotvtk = LegacyVTKReader(registrationName='SEC_rot.vtk', FileNames=[vtk_file])

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[SEC_rotvtk, pointDatatoCellData1])

# ----------------------------------------------------------------
# restore active source
SetActiveSource(appendAttributes1)
# ----------------------------------------------------------------

# get active source.
appendAttributes1 = GetActiveSource()

# get active view
renderView1 = GetActiveViewOrCreate('RenderView')

# show data from appendAttributes1
appendAttributes1Display = Show(appendAttributes1, renderView1, 'UnstructuredGridRepresentation')

# get color transfer function/color map for 'Field0'
field0LUT = GetColorTransferFunction('Field0')

# trace defaults for the display properties
appendAttributes1Display.Representation = 'Surface'
appendAttributes1Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes1Display.LookupTable = field0LUT

# get color legend/bar for field0LUT in view renderView1
field0LUTColorBar = GetScalarBar(field0LUT, renderView1)
field0LUTColorBar.WindowLocation = 'Any Location'
field0LUTColorBar.Position = [0.35802325581395344, 0.5654545454545454]
field0LUTColorBar.Title = 'phi'
field0LUTColorBar.ComponentTitle = '[-]'
field0LUTColorBar.HorizontalTitle = 1
field0LUTColorBar.AutomaticLabelFormat = 1
#field0LUTColorBar.UseCustomLabels = 1
field0LUTColorBar.RangeLabelFormat = '%-#6.2g'
field0LUTColorBar.ScalarBarLength = 0.33000000000000007

# show color legend
appendAttributes1Display.SetScalarBarVisibility(renderView1, True)

# set scalar coloring
ColorBy(appendAttributes1Display, ('CELLS', 'Field 0'))

# reset view to fit data bounds
renderView1.ResetCamera()
renderView1.CameraPosition = [-56.832469639374075, -78.26642904058744, 37.54072006036547]
renderView1.CameraFocalPoint = [88.74752674104539, 87.38905611461664, -16.833862749101034]
renderView1.CameraViewUp = [0.12889112479316778, 0.20518600647590404, 0.9701988356496942]


if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')
