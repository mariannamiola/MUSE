# state file generated using paraview version 5.13.0
import paraview
paraview.compatibility.major = 5
paraview.compatibility.minor = 13

import sys
import os

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

# ----------------------------------------------------------------
# setup views used in the visualization
# ----------------------------------------------------------------

# get the material library
materialLibrary1 = GetMaterialLibrary()

# Create a new 'Render View'
renderView3 = CreateView('RenderView')
renderView3.ViewSize = [2152, 1140]
renderView3.InteractionMode = '2D'
renderView3.AxesGrid = 'Grid Axes 3D Actor'
renderView3.CenterOfRotation = [482452.01819983905, 4897499.754385281, 0.0]
renderView3.StereoType = 'Crystal Eyes'
renderView3.CameraPosition = [482452.01819983905, 4897499.754385281, 442890.03861068015]
renderView3.CameraFocalPoint = [482452.01819983905, 4897499.754385281, 0.0]
renderView3.CameraFocalDisk = 1.0
renderView3.CameraParallelScale = 60959.94846395798
renderView3.LegendGrid = 'Legend Grid Actor'
renderView3.PolarGrid = 'Polar Grid Actor'
renderView3.BackEnd = 'OSPRay raycaster'
renderView3.OSPRayMaterialLibrary = materialLibrary1

SetActiveView(None)

# ----------------------------------------------------------------
# setup view layouts
# ----------------------------------------------------------------

# create new layout object 'Layout #1'
layout1 = CreateLayout(name='Layout #1')
layout1.AssignView(0, renderView3)
layout1.SetSize(2152, 1140)

# ----------------------------------------------------------------
# restore active view
SetActiveView(renderView3)
# ----------------------------------------------------------------

# ----------------------------------------------------------------
# setup the data processing pipelines
# ----------------------------------------------------------------

script_dir=os.path.dirname(os.path.realpath(__file__))
#print(script_dir)

work_folder='MUSE_test'
project_name='13_RISL_SUpoly_mf'

var_name='forecast'
vario='DIR2D'
geom_name='su_liguria'

compute_sim1=var_name+'_'+vario+'_'+geom_name

project_folder=work_folder +'/'+project_name
geom_folder=project_folder+'/out/geometry/surf'

compute_folder=project_folder+'/out/compute'
##project_append=sys.argv[1]
project_append='080320180000'
compute_folder=compute_folder+'/'+project_append

space='/_varspace'
stats_name='mean'
sim_name=var_name+'_'+stats_name


csv_file1=script_dir+'/../../'+compute_folder+'/'+compute_sim1+space+'/_stats/'+sim_name+'.csv'
obj_file1=script_dir+'/../../'+geom_folder+'/'+geom_name+'.obj'



# create a new 'Wavefront OBJ Reader'
su_liguriaobj = WavefrontOBJReader(registrationName='su_liguria.obj', FileName=obj_file1)

# create a new 'CSV Reader'
forecast_meancsv = CSVReader(registrationName='forecast_mean.csv', FileName=[csv_file1])
forecast_meancsv.UseStringDelimiter = 0
forecast_meancsv.HaveHeaders = 0
forecast_meancsv.FieldDelimiterCharacters = ' '
forecast_meancsv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints1 = TableToPoints(registrationName='TableToPoints1', Input=forecast_meancsv)
tableToPoints1.XColumn = 'Field 0'
tableToPoints1.YColumn = 'Field 0'
tableToPoints1.ZColumn = 'Field 0'
tableToPoints1.a2DPoints = 1
tableToPoints1.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate1 = Triangulate(registrationName='Triangulate1', Input=tableToPoints1)

# create a new 'Point Data to Cell Data'
pointDatatoCellData1 = PointDatatoCellData(registrationName='PointDatatoCellData1', Input=triangulate1)
pointDatatoCellData1.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData1.PassPointData = 1

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[su_liguriaobj, pointDatatoCellData1])

# ----------------------------------------------------------------
# setup the visualization in view 'renderView3'
# ----------------------------------------------------------------

# show data from su_liguriaobj
su_liguriaobjDisplay = Show(su_liguriaobj, renderView3, 'GeometryRepresentation')

# trace defaults for the display properties.
su_liguriaobjDisplay.Representation = 'Surface'
su_liguriaobjDisplay.ColorArrayName = ['POINTS', '']
su_liguriaobjDisplay.SelectNormalArray = 'None'
su_liguriaobjDisplay.SelectTangentArray = 'None'
su_liguriaobjDisplay.SelectTCoordArray = 'None'
su_liguriaobjDisplay.TextureTransform = 'Transform2'
su_liguriaobjDisplay.OSPRayScaleFunction = 'Piecewise Function'
su_liguriaobjDisplay.Assembly = ''
su_liguriaobjDisplay.SelectedBlockSelectors = ['']
su_liguriaobjDisplay.SelectOrientationVectors = 'None'
su_liguriaobjDisplay.ScaleFactor = 20673.89891583929
su_liguriaobjDisplay.SelectScaleArray = 'None'
su_liguriaobjDisplay.GlyphType = 'Arrow'
su_liguriaobjDisplay.GlyphTableIndexArray = 'None'
su_liguriaobjDisplay.GaussianRadius = 1033.6949457919645
su_liguriaobjDisplay.SetScaleArray = ['POINTS', '']
su_liguriaobjDisplay.ScaleTransferFunction = 'Piecewise Function'
su_liguriaobjDisplay.OpacityArray = ['POINTS', '']
su_liguriaobjDisplay.OpacityTransferFunction = 'Piecewise Function'
su_liguriaobjDisplay.DataAxesGrid = 'Grid Axes Representation'
su_liguriaobjDisplay.PolarAxes = 'Polar Axes Representation'
su_liguriaobjDisplay.SelectInputVectors = ['POINTS', '']
su_liguriaobjDisplay.WriteLog = ''

# show data from appendAttributes1
appendAttributes1Display = Show(appendAttributes1, renderView3, 'GeometryRepresentation')

# get 2D transfer function for 'Field0'
field0TF2D = GetTransferFunction2D('Field0')

# get color transfer function/color map for 'Field0'
field0LUT = GetColorTransferFunction('Field0')
field0LUT.TransferFunction2D = field0TF2D
field0LUT.RGBPoints = [0.0093, 0.231373, 0.298039, 0.752941, 0.166666787, 0.865003, 0.865003, 0.865003, 0.324033574, 0.705882, 0.0156863, 0.14902]
field0LUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
appendAttributes1Display.Representation = 'Surface'
appendAttributes1Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes1Display.LookupTable = field0LUT
appendAttributes1Display.SelectNormalArray = 'None'
appendAttributes1Display.SelectTangentArray = 'None'
appendAttributes1Display.SelectTCoordArray = 'None'
appendAttributes1Display.TextureTransform = 'Transform2'
appendAttributes1Display.OSPRayScaleFunction = 'Piecewise Function'
appendAttributes1Display.Assembly = ''
appendAttributes1Display.SelectedBlockSelectors = ['']
appendAttributes1Display.SelectOrientationVectors = 'None'
appendAttributes1Display.ScaleFactor = 20673.89891583929
appendAttributes1Display.SelectScaleArray = 'None'
appendAttributes1Display.GlyphType = 'Arrow'
appendAttributes1Display.GlyphTableIndexArray = 'None'
appendAttributes1Display.GaussianRadius = 1033.6949457919645
appendAttributes1Display.SetScaleArray = ['POINTS', '']
appendAttributes1Display.ScaleTransferFunction = 'Piecewise Function'
appendAttributes1Display.OpacityArray = ['POINTS', '']
appendAttributes1Display.OpacityTransferFunction = 'Piecewise Function'
appendAttributes1Display.DataAxesGrid = 'Grid Axes Representation'
appendAttributes1Display.PolarAxes = 'Polar Axes Representation'
appendAttributes1Display.SelectInputVectors = ['POINTS', '']
appendAttributes1Display.WriteLog = ''

# init the 'Piecewise Function' selected for 'OSPRayScaleFunction'
appendAttributes1Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# setup the color legend parameters for each legend in this view

# get color legend/bar for field0LUT in view renderView3
field0LUTColorBar = GetScalarBar(field0LUT, renderView3)
field0LUTColorBar.Position = [0.8996863382899628, 0.016666666666666666]
field0LUTColorBar.Title = 'Field 0'
field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
field0LUTColorBar.Visibility = 1

# show color legend
appendAttributes1Display.SetScalarBarVisibility(renderView3, True)

# ----------------------------------------------------------------
# setup color maps and opacity maps used in the visualization
# note: the Get..() functions create a new object, if needed
# ----------------------------------------------------------------

# get opacity transfer function/opacity map for 'Field0'
field0PWF = GetOpacityTransferFunction('Field0')
field0PWF.Points = [0.0836820019, 0.0, 0.5, 0.0, 14.4433432, 1.0, 0.5, 0.0]
field0PWF.ScalarRangeInitialized = 1

# ----------------------------------------------------------------
# setup animation scene, tracks and keyframes
# note: the Get..() functions create a new object, if needed
# ----------------------------------------------------------------

# get the time-keeper
timeKeeper1 = GetTimeKeeper()

# initialize the timekeeper

# get time animation track
timeAnimationCue1 = GetTimeTrack()

# initialize the animation track

# get animation scene
animationScene1 = GetAnimationScene()

# initialize the animation scene
animationScene1.ViewModules = renderView3
animationScene1.Cues = timeAnimationCue1
animationScene1.AnimationTime = 0.0

# initialize the animation scene

# ----------------------------------------------------------------
# restore active source
SetActiveSource(appendAttributes1)
# ----------------------------------------------------------------


##--------------------------------------------
## You may need to add some code at the end of this python script depending on your usage, eg:
#
## Render all views to see them appears
# RenderAllViews()
#
## Interact with the view, usefull when running from pvpython
# Interact()
#
## Save a screenshot of the active view
# SaveScreenshot("path/to/screenshot.png")
directory = script_dir+'/../../'+project_folder+'/_fig'
if not os.path.exists(directory): 
	os.mkdir(directory)
	
SaveScreenshot(directory+'/'+sim_name+'_'+project_append+'.png', renderView3, ImageResolution=[2152, 1140], FontScaling=True, OverrideColorPalette='WhiteBackground', TransparentBackground=False)
#
## Save a screenshot of a layout (multiple splitted view)
# SaveScreenshot("path/to/screenshot.png", GetLayout())
#
## Save all "Extractors" from the pipeline browser
# SaveExtracts()
#
## Save a animation of the current active view
# SaveAnimation()
#
## Please refer to the documentation of paraview.simple
## https://www.paraview.org/paraview-docs/latest/python/paraview.simple.html
##--------------------------------------------