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

work_folder='MUSE_test'
project_name='21_DGM_PrecAgri_hex'

var_name='H'
sub=''
vario='DIR3Dxy'
geom_name='area'

compute_sim=var_name+'_'+vario+'_'+geom_name

project_folder=work_folder +'/'+project_name
geom_folder=project_folder+'/out/geometry/volume'

compute_folder=project_folder+'/out/compute'
project_append=sys.argv[1]
##project_append='20240403'
compute_folder=compute_folder+'/'+project_append

space='/_varspace/_stats'
stats_name='mean'
sim_name=var_name+'_'+stats_name

csv_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+space+'/'+sim_name+'.csv'
vtk_file=script_dir+'/../../'+geom_folder+'/'+geom_name+'.vtk'
samplescsv_file=script_dir+'/../data/samples.csv'

print(csv_file)
print(vtk_file)


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
cubevtk = LegacyVTKReader(registrationName=geom_name+'.vtk', FileNames=[vtk_file])

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[cubevtk, pointDatatoCellData1])

# create a new 'Transform'
transform1 = Transform(registrationName='Transform1', Input=appendAttributes1)
transform1.Transform = 'Transform'

# init the 'Transform' selected for 'Transform'
transform1.Transform.Scale = [1.0, 1.0, 10.0]

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

# show color legend
appendAttributes1Display.SetScalarBarVisibility(renderView1, True)

# set scalar coloring
ColorBy(appendAttributes1Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
appendAttributes1Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
field0LUT = GetColorTransferFunction('Field 0')
field0LUT.RescaleTransferFunction(0, 100.0)

# show data from transform1
transform1Display = Show(transform1, renderView1, 'UnstructuredGridRepresentation')

# trace defaults for the display properties.
transform1Display.Representation = 'Surface'
transform1Display.ColorArrayName = ['CELLS', 'Field 0']
transform1Display.LookupTable = field0LUT
transform1Display.SelectTCoordArray = 'None'
transform1Display.SelectNormalArray = 'None'
transform1Display.SelectTangentArray = 'None'
transform1Display.OSPRayScaleFunction = 'PiecewiseFunction'
transform1Display.SelectOrientationVectors = 'None'
transform1Display.ScaleFactor = 44.0
transform1Display.SelectScaleArray = 'None'
transform1Display.GlyphType = 'Arrow'
transform1Display.GlyphTableIndexArray = 'None'
transform1Display.GaussianRadius = 2.2
transform1Display.SetScaleArray = [None, '']
transform1Display.ScaleTransferFunction = 'PiecewiseFunction'
transform1Display.OpacityArray = [None, '']
transform1Display.OpacityTransferFunction = 'PiecewiseFunction'
transform1Display.DataAxesGrid = 'GridAxesRepresentation'
transform1Display.PolarAxes = 'PolarAxesRepresentation'
#transform1Display.ScalarOpacityFunction = field0PWF
#transform1Display.ScalarOpacityUnitDistance = 82.40250048444182
#transform1Display.OpacityArrayName = ['CELLS', 'Field 0']

# reset view to fit data bounds
renderView1.ResetCamera()
#renderView1.CameraPosition = [1139082.9389216616, 4519185.75737473, 1481.7432105269102]
#renderView1.CameraFocalPoint = [1139520.000000002, 4520355.000000002, 1280.0299987795752]
#renderView1.CameraViewUp = [0.02127103633773175, 0.16223988566605416, 0.9865220537383768]

# To save a specific target resolution, rather than using the
# the current view (or layout) size, and override the color palette.
directory = script_dir+'/../../'+project_folder+'/_fig'
if not os.path.exists(directory): 
	os.mkdir(directory)
	
SaveScreenshot(directory+'/'+sim_name+'_'+project_append+'.png', renderView1, FontScaling=True, OverrideColorPalette='WhiteBackground', TransparentBackground=False)

if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')
