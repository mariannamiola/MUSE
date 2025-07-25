# state file generated using paraview version 5.10.0-RC1

# uncomment the following three lines to ensure this script works in future versions
#import paraview
#paraview.compatibility.major = 5
#paraview.compatibility.minor = 10

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

# ----------------------------------------------------------------
# setup views used in the visualization
# ----------------------------------------------------------------

# Create a new 'Render View'
renderView1 = CreateView('RenderView')
renderView1.ViewSize = [1542, 784]
renderView1.AxesGrid = 'GridAxes3DActor'
renderView1.CenterOfRotation = [419105.0, 5114907.5, 0.0]
renderView1.StereoType = 'Crystal Eyes'
renderView1.CameraPosition = [419105.0, 5114907.5, 460.109315336415]
renderView1.CameraFocalPoint = [419105.0, 5114907.5, 0.0]
renderView1.CameraViewAngle = 25.0
renderView1.CameraFocalDisk = 1.0
renderView1.CameraParallelScale = 119.08505363814554
renderView1.Background = [0.32, 0.34, 0.43]

SetActiveView(None)

# ----------------------------------------------------------------
# setup view layouts
# ----------------------------------------------------------------

# create new layout object 'Layout #1'
layout1 = CreateLayout(name='Layout #1')
layout1.AssignView(0, renderView1)
layout1.SetSize(1542, 784)

# ----------------------------------------------------------------
# restore active view
SetActiveView(renderView1)
# ----------------------------------------------------------------

# ----------------------------------------------------------------
# setup the data processing pipelines
# ----------------------------------------------------------------

# create a new 'CSV Reader'
cO2_fluxcsv = CSVReader(registrationName='CO2_flux.csv', FileName=['/home/marianna/muse/examples/17_IAVCEI_Lazaresti/script/../../MUSE_test/17_IAVCEI_Lazaresti/out/compute/CO2_flux_DIR2D_domain/_varspace/_stats/CO2_flux_mean.csv'])
cO2_fluxcsv.UseStringDelimiter = 0
cO2_fluxcsv.HaveHeaders = 0
cO2_fluxcsv.FieldDelimiterCharacters = ''
cO2_fluxcsv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints2 = TableToPoints(registrationName='TableToPoints2', Input=cO2_fluxcsv)
tableToPoints2.XColumn = 'Field 0'
tableToPoints2.YColumn = 'Field 0'
tableToPoints2.ZColumn = 'Field 0'
tableToPoints2.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate1 = Triangulate(registrationName='Triangulate1', Input=tableToPoints2)

# create a new 'Point Data to Cell Data'
pointDatatoCellData1 = PointDatatoCellData(registrationName='PointDatatoCellData1', Input=triangulate1)
pointDatatoCellData1.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData1.PassPointData = 1

# create a new 'CSV Reader'
samplescsv = CSVReader(registrationName='samples.csv', FileName=['/home/marianna/muse/examples/17_IAVCEI_Lazaresti/script/../data/samples.csv'])
samplescsv.UseStringDelimiter = 0
samplescsv.FieldDelimiterCharacters = ';'

# create a new 'Table To Points'
tableToPoints1 = TableToPoints(registrationName='TableToPoints1', Input=samplescsv)
tableToPoints1.XColumn = 'x_utm'
tableToPoints1.YColumn = 'y_utm'
tableToPoints1.ZColumn = 'CO2_flux'
tableToPoints1.a2DPoints = 1
tableToPoints1.KeepAllDataArrays = 1

# create a new 'Wavefront OBJ Reader'
domainobj = WavefrontOBJReader(registrationName='domain.obj', FileName='/home/marianna/muse/examples/17_IAVCEI_Lazaresti/script/../../MUSE_test/17_IAVCEI_Lazaresti/out/geometry/surf/domain.obj')

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[domainobj, pointDatatoCellData1])

# ----------------------------------------------------------------
# setup the visualization in view 'renderView1'
# ----------------------------------------------------------------

# show data from appendAttributes1
appendAttributes1Display = Show(appendAttributes1, renderView1, 'GeometryRepresentation')

# get color transfer function/color map for 'Field0'
field0LUT = GetColorTransferFunction('Field0')
field0LUT.RGBPoints = [6.8156, 0.231373, 0.298039, 0.752941, 966.3469650000001, 0.865003, 0.865003, 0.865003, 1925.87833, 0.705882, 0.0156863, 0.14902]
field0LUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
appendAttributes1Display.Representation = 'Surface'
appendAttributes1Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes1Display.LookupTable = field0LUT
appendAttributes1Display.SelectTCoordArray = 'None'
appendAttributes1Display.SelectNormalArray = 'None'
appendAttributes1Display.SelectTangentArray = 'None'
appendAttributes1Display.OSPRayScaleFunction = 'PiecewiseFunction'
appendAttributes1Display.SelectOrientationVectors = 'None'
appendAttributes1Display.ScaleFactor = 18.5
appendAttributes1Display.SelectScaleArray = 'None'
appendAttributes1Display.GlyphType = 'Arrow'
appendAttributes1Display.GlyphTableIndexArray = 'None'
appendAttributes1Display.GaussianRadius = 0.925
appendAttributes1Display.SetScaleArray = [None, '']
appendAttributes1Display.ScaleTransferFunction = 'PiecewiseFunction'
appendAttributes1Display.OpacityArray = [None, '']
appendAttributes1Display.OpacityTransferFunction = 'PiecewiseFunction'
appendAttributes1Display.DataAxesGrid = 'GridAxesRepresentation'
appendAttributes1Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes1Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# show data from tableToPoints1
tableToPoints1Display = Show(tableToPoints1, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
tableToPoints1Display.Representation = 'Points'
tableToPoints1Display.AmbientColor = [1.0, 1.0, 0.0]
tableToPoints1Display.ColorArrayName = ['POINTS', '']
tableToPoints1Display.DiffuseColor = [1.0, 1.0, 0.0]
tableToPoints1Display.PointSize = 5.0
tableToPoints1Display.SelectTCoordArray = 'None'
tableToPoints1Display.SelectNormalArray = 'None'
tableToPoints1Display.SelectTangentArray = 'None'
tableToPoints1Display.OSPRayScaleArray = 'CO2_flux'
tableToPoints1Display.OSPRayScaleFunction = 'PiecewiseFunction'
tableToPoints1Display.SelectOrientationVectors = 'None'
tableToPoints1Display.ScaleFactor = 17.797790644038468
tableToPoints1Display.SelectScaleArray = 'CO2_flux'
tableToPoints1Display.GlyphType = 'Arrow'
tableToPoints1Display.GlyphTableIndexArray = 'CO2_flux'
tableToPoints1Display.GaussianRadius = 0.8898895322019235
tableToPoints1Display.SetScaleArray = ['POINTS', 'CO2_flux']
tableToPoints1Display.ScaleTransferFunction = 'PiecewiseFunction'
tableToPoints1Display.OpacityArray = ['POINTS', 'CO2_flux']
tableToPoints1Display.OpacityTransferFunction = 'PiecewiseFunction'
tableToPoints1Display.DataAxesGrid = 'GridAxesRepresentation'
tableToPoints1Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
tableToPoints1Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
tableToPoints1Display.ScaleTransferFunction.Points = [4.686, 0.0, 0.5, 0.0, 4097.4964, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
tableToPoints1Display.OpacityTransferFunction.Points = [4.686, 0.0, 0.5, 0.0, 4097.4964, 1.0, 0.5, 0.0]

# setup the color legend parameters for each legend in this view

# get color legend/bar for field0LUT in view renderView1
field0LUTColorBar = GetScalarBar(field0LUT, renderView1)
field0LUTColorBar.WindowLocation = 'Any Location'
field0LUTColorBar.Position = [0.23897535667963676, 0.6224489795918368]
field0LUTColorBar.Title = 'CO2_flux'
field0LUTColorBar.ComponentTitle = '[g/m2 d]'
field0LUTColorBar.HorizontalTitle = 1
field0LUTColorBar.AutomaticLabelFormat = 0
field0LUTColorBar.LabelFormat = '%-#6.5g'
field0LUTColorBar.RangeLabelFormat = '%-#6.5g'
field0LUTColorBar.ScalarBarLength = 0.32999999999999996

# set color bar visibility
field0LUTColorBar.Visibility = 1

# get color transfer function/color map for 'CO2_flux'
cO2_fluxLUT = GetColorTransferFunction('CO2_flux')
cO2_fluxLUT.RGBPoints = [4.686, 0.231373, 0.298039, 0.752941, 2051.0912, 0.865003, 0.865003, 0.865003, 4097.4964, 0.705882, 0.0156863, 0.14902]
cO2_fluxLUT.ScalarRangeInitialized = 1.0

# get color legend/bar for cO2_fluxLUT in view renderView1
cO2_fluxLUTColorBar = GetScalarBar(cO2_fluxLUT, renderView1)
cO2_fluxLUTColorBar.Title = 'CO2_flux'
cO2_fluxLUTColorBar.ComponentTitle = ''

# set color bar visibility
cO2_fluxLUTColorBar.Visibility = 0

# get color transfer function/color map for 'point'
pointLUT = GetColorTransferFunction('point')
pointLUT.InterpretValuesAsCategories = 1
pointLUT.RGBPoints = [21.4889134, 0.231373, 0.298039, 0.752941, 125.8544292, 0.865003, 0.865003, 0.865003, 230.219945, 0.705882, 0.0156863, 0.14902]

# get color legend/bar for pointLUT in view renderView1
pointLUTColorBar = GetScalarBar(pointLUT, renderView1)
pointLUTColorBar.Title = 'point'
pointLUTColorBar.ComponentTitle = ''

# set color bar visibility
pointLUTColorBar.Visibility = 0

# show color legend
appendAttributes1Display.SetScalarBarVisibility(renderView1, True)

# ----------------------------------------------------------------
# setup color maps and opacity mapes used in the visualization
# note: the Get..() functions create a new object, if needed
# ----------------------------------------------------------------

# get opacity transfer function/opacity map for 'CO2_flux'
cO2_fluxPWF = GetOpacityTransferFunction('CO2_flux')
cO2_fluxPWF.Points = [4.686, 0.0, 0.5, 0.0, 4097.4964, 1.0, 0.5, 0.0]
cO2_fluxPWF.ScalarRangeInitialized = 1

# get opacity transfer function/opacity map for 'point'
pointPWF = GetOpacityTransferFunction('point')
pointPWF.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# get opacity transfer function/opacity map for 'Field0'
field0PWF = GetOpacityTransferFunction('Field0')
field0PWF.Points = [6.8156, 0.0, 0.5, 0.0, 1925.87833, 1.0, 0.5, 0.0]
field0PWF.ScalarRangeInitialized = 1

# ----------------------------------------------------------------
# restore active source
SetActiveSource(tableToPoints1)
# ----------------------------------------------------------------


if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')