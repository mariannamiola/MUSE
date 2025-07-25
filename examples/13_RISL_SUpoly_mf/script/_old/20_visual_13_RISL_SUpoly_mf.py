# state file generated using paraview version 5.10.0-RC1

# uncomment the following three lines to ensure this script works in future versions
#import paraview
#paraview.compatibility.major = 5
#paraview.compatibility.minor = 10

import sys
import os

###frame=sys.argv[1]

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

# ----------------------------------------------------------------
# setup views used in the visualization
# ----------------------------------------------------------------

# Create a new 'Render View'
renderView1 = CreateView('RenderView')
renderView1.ViewSize = [2152, 1140]
renderView1.InteractionMode = '2D'
renderView1.AxesGrid = 'Grid Axes 3D Actor'
renderView1.CenterOfRotation = [482452.01819983905, 4897499.754385281, 0.0]
renderView1.StereoType = 'Crystal Eyes'
renderView1.CameraPosition = [482452.01819983905, 4897499.754385281, 442890.03861068015]
renderView1.CameraFocalPoint = [482452.01819983905, 4897499.754385281, 0.0]
renderView1.CameraFocalDisk = 1.0
renderView1.CameraParallelScale = 60959.94846395798
renderView1.LegendGrid = 'Legend Grid Actor'
renderView1.PolarGrid = 'Polar Grid Actor'
renderView1.BackEnd = 'OSPRay raycaster'
renderView1.OSPRayMaterialLibrary = materialLibrary1

SetActiveView(None)

# ----------------------------------------------------------------
# setup view layouts
# ----------------------------------------------------------------

# create new layout object 'Layout #1'
layout1 = CreateLayout(name='Layout #1')
layout1.AssignView(0, renderView1)
layout1.SetSize(2152, 1140)

# ----------------------------------------------------------------
# restore active view
SetActiveView(renderView1)
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


# create a new 'CSV Reader'
eH_sgs_meancsv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file1])
eH_sgs_meancsv.UseStringDelimiter = 0
eH_sgs_meancsv.HaveHeaders = 0
eH_sgs_meancsv.FieldDelimiterCharacters = ' '
eH_sgs_meancsv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints1 = TableToPoints(registrationName='TableToPoints1', Input=eH_sgs_meancsv)
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

# create a new 'Wavefront OBJ Reader'
italy_3_1_resobj = WavefrontOBJReader(registrationName=geom_name+'.obj', FileName=obj_file1)

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[italy_3_1_resobj, pointDatatoCellData1])





# ----------------------------------------------------------------
# setup the visualization in view 'renderView1'
# ----------------------------------------------------------------

# show data from italy_3_1_resobj
italy_3_1_resobjDisplay = Show(italy_3_1_resobj, renderView1, 'GeometryRepresentation')

# show data from appendAttributes1
appendAttributes1Display = Show(appendAttributes1, renderView1, 'GeometryRepresentation')

# get color transfer function/color map for 'Field0'
field0LUT = GetColorTransferFunction('Field0')
field0LUT.RGBPoints = [0.0836820019, 0.231373, 0.298039, 0.752941, 7.2635126009499995, 0.865003, 0.865003, 0.865003, 14.4433432, 0.705882, 0.0156863, 0.14902]
field0LUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
appendAttributes1Display.Representation = 'Surface'
appendAttributes1Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes1Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes1Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
ColorBy(appendAttributes1Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes1Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RescaleTransferFunction(0.0, 30.0)

################################################



# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RescaleTransferFunction(0.0, 30.0)


# setup the color legend parameters for each legend in this view

# get color legend/bar for field0LUT in view renderView1
field0LUTColorBar = GetScalarBar(field0LUT, renderView1)
field0LUTColorBar.Title = 'Field 0'
field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
field0LUTColorBar.Visibility = 1

# show color legend
appendAttributes1Display.SetScalarBarVisibility(renderView1, True)

# ----------------------------------------------------------------
# setup color maps and opacity mapes used in the visualization
# note: the Get..() functions create a new object, if needed
# ----------------------------------------------------------------

# get opacity transfer function/opacity map for 'Field0'
field0PWF = GetOpacityTransferFunction('Field0')
field0PWF.Points = [0.0836820019, 0.0, 0.5, 0.0, 14.4433432, 1.0, 0.5, 0.0]
field0PWF.ScalarRangeInitialized = 1


# To save a specific target resolution, rather than using the
# the current view (or layout) size, and override the color palette.
directory = script_dir+'/../../'+project_folder+'/_fig'
if not os.path.exists(directory): 
	os.mkdir(directory)
	
SaveScreenshot(directory+'/'+sim_name+'_'+project_append+'.png', renderView1, ImageResolution=[960, 784], FontScaling=True, OverrideColorPalette='WhiteBackground', TransparentBackground=False)



# ----------------------------------------------------------------
# restore active source
SetActiveSource(appendAttributes1)
# ----------------------------------------------------------------

if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')
    
    
    
