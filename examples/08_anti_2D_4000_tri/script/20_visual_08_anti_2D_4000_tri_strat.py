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
project_name='08_anti_2D_4000_tri'

var_name='phi'
sub='anti'
vario='DIR2D'
geom_name='geom_anti'

compute_sim=var_name+'_'+sub+'_'+vario+'_'+geom_name

project_folder=work_folder +'/'+project_name
geom_folder=project_folder+'/out/geometry/surf'
compute_folder=project_folder+'/out/compute'

space='/_varspace/_stats'
stats_name='mean'
sim_name=var_name+'_'+stats_name

csv_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+space+'/'+sim_name+'.csv'
obj_file=script_dir+'/../../'+geom_folder+'/'+geom_name+'_rot.obj'
samplescsv_file=script_dir+'/../data/samples.csv'

print(csv_file)
print(obj_file)

# create a new 'CSV Reader'
phi_meansimcsv = CSVReader(registrationName='phi_meansim.csv', FileName=[csv_file])
phi_meansimcsv.HaveHeaders = 0
phi_meansimcsv.FieldDelimiterCharacters = ''
phi_meansimcsv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints1 = TableToPoints(registrationName='TableToPoints1', Input=phi_meansimcsv)
tableToPoints1.XColumn = 'Field 0'
tableToPoints1.YColumn = 'Field 0'
tableToPoints1.ZColumn = 'Field 0'
tableToPoints1.KeepAllDataArrays = 1

# create a new 'CSV Reader'
samplescsv = CSVReader(registrationName='samples.csv', FileName=[samplescsv_file])
samplescsv.UseStringDelimiter = 0
samplescsv.HaveHeaders = 1
samplescsv.FieldDelimiterCharacters = ';'

# create a new 'Table To Points'
tableToPoints2 = TableToPoints(registrationName='TableToPoints2', Input=samplescsv)
tableToPoints2.XColumn = 'X'
tableToPoints2.YColumn = 'Y'
tableToPoints2.ZColumn = 'Z'

# create a new 'Triangulate'
triangulate1 = Triangulate(registrationName='Triangulate1', Input=tableToPoints1)

# create a new 'Point Data to Cell Data'
pointDatatoCellData1 = PointDatatoCellData(registrationName='PointDatatoCellData1', Input=triangulate1)
pointDatatoCellData1.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData1.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
geom_anti_rotobj = WavefrontOBJReader(registrationName='geom_anti_rot.obj', FileName=obj_file)

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[geom_anti_rotobj, pointDatatoCellData1])

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
field0LUT = GetColorTransferFunction('Field0')

# trace defaults for the display properties.
appendAttributes1Display.Representation = 'Surface'
appendAttributes1Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes1Display.LookupTable = field0LUT

# show data from tableToPoints1
tableToPoints2Display = Show(tableToPoints2, renderView1, 'GeometryRepresentation')

# get color transfer function/color map for 'phi'
phiLUT = GetColorTransferFunction('phi')
phiLUT.RGBPoints = [0.1, 0.231373, 0.298039, 0.752941, 0.45000000000000007, 0.865003, 0.865003, 0.865003, 0.8, 0.705882, 0.0156863, 0.14902]
phiLUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
tableToPoints2Display.Representation = 'Points'
tableToPoints2Display.ColorArrayName = ['POINTS', 'phi']
tableToPoints2Display.LookupTable = phiLUT
tableToPoints2Display.PointSize = 10.0
tableToPoints2Display.SelectTCoordArray = 'None'
tableToPoints2Display.SelectNormalArray = 'None'
tableToPoints2Display.SelectTangentArray = 'None'
tableToPoints2Display.OSPRayScaleArray = 'phi'
tableToPoints2Display.OSPRayScaleFunction = 'PiecewiseFunction'
tableToPoints2Display.SelectOrientationVectors = 'None'
tableToPoints2Display.ScaleFactor = 8.620000000000001
tableToPoints2Display.SelectScaleArray = 'None'
tableToPoints2Display.GlyphType = 'Arrow'
tableToPoints2Display.GlyphTableIndexArray = 'None'
tableToPoints2Display.GaussianRadius = 0.43100000000000005
tableToPoints2Display.SetScaleArray = ['POINTS', 'phi']
tableToPoints2Display.ScaleTransferFunction = 'PiecewiseFunction'
tableToPoints2Display.OpacityArray = ['POINTS', 'phi']
tableToPoints2Display.OpacityTransferFunction = 'PiecewiseFunction'
tableToPoints2Display.DataAxesGrid = 'GridAxesRepresentation'
tableToPoints2Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
tableToPoints2Display.ScaleTransferFunction.Points = [0.1, 0.0, 0.5, 0.0, 0.8, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
tableToPoints2Display.OpacityTransferFunction.Points = [0.1, 0.0, 0.5, 0.0, 0.8, 1.0, 0.5, 0.0]

# setup the color legend parameters for each legend in this view

# get color legend/bar for field0LUT in view renderView1
field0LUTColorBar = GetScalarBar(field0LUT, renderView1)
field0LUTColorBar.WindowLocation = 'Any Location'
field0LUTColorBar.Position = [0.019775605966931446, 0.2950017839303553]
field0LUTColorBar.Title = 'phi'
field0LUTColorBar.ComponentTitle = '[-]'
field0LUTColorBar.HorizontalTitle = 1
#field0LUTColorBar.AutomaticLabelFormat = 1
#field0LUTColorBar.UseCustomLabels = 1
field0LUTColorBar.RangeLabelFormat = '%-#7.4g'
field0LUTColorBar.ScalarBarLength = 0.2496428571428571

# set color bar visibility
field0LUTColorBar.Visibility = 1

# get color legend/bar for phiLUT in view renderView1
phiLUTColorBar = GetScalarBar(phiLUT, renderView1)
phiLUTColorBar.Title = 'phi'
phiLUTColorBar.ComponentTitle = ''

# set color bar visibility
phiLUTColorBar.Visibility = 1

appendAttributes1Display.SetScalarBarVisibility(renderView1, True)

# set scalar coloring
ColorBy(appendAttributes1Display, ('CELLS', 'Field 0'))

# reset view to fit data bounds
renderView1.ResetCamera()
renderView1.CameraPosition = [50.0, -219.4766694427885, 12.5]
renderView1.CameraFocalPoint = [50.0, 1.0, 12.5]
renderView1.CameraViewUp = [0.0, 0.0, 1.0]



if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')
