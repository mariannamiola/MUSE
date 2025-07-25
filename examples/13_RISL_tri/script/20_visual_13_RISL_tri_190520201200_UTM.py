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
# setup views used in the visualization
# ----------------------------------------------------------------

# Create a new 'Render View'
renderView1 = CreateView('RenderView')
renderView1.ViewSize = [1365, 784]
renderView1.InteractionMode = '2D'
renderView1.AxesGrid = 'GridAxes3DActor'
renderView1.CenterOfRotation = [812736.71875, 4719403.25, 0.0]
renderView1.StereoType = 'Crystal Eyes'
renderView1.CameraPosition = [787620.5889933391, 4643905.715066378, 10000.0]
renderView1.CameraFocalPoint = [787620.5889933391, 4643905.715066378, 0.0]
renderView1.CameraFocalDisk = 1.0
renderView1.CameraParallelScale = 584651.0014614653
renderView1.Background = [0.32, 0.34, 0.43]

SetActiveView(None)

# ----------------------------------------------------------------
# setup view layouts
# ----------------------------------------------------------------

# create new layout object 'Layout #1'
layout1 = CreateLayout(name='Layout #1')
layout1.AssignView(0, renderView1)
layout1.SetSize(1365, 784)

# ----------------------------------------------------------------
# restore active view
SetActiveView(renderView1)
# ----------------------------------------------------------------

# ----------------------------------------------------------------
# setup the data processing pipelines
# ----------------------------------------------------------------

script_dir=os.path.dirname(os.path.realpath(__file__))
project_append='_190520201200_UTM'
geom_append='_t2000'

csv_file1=script_dir+'/../../MUSE_test/13_RISL_tri'+project_append+'/out/compute/EH_italy_3'+geom_append+'_1_DIR2D_italy_3'+geom_append+'_1/_varspace/_stats/EH_sgs_mean.csv'
obj_file1=script_dir+'/../../MUSE_test/13_RISL_tri'+project_append+'/out/geometry/surf/italy_3'+geom_append+'_1.obj'

csv_file2=script_dir+'/../../MUSE_test/13_RISL_tri'+project_append+'/out/compute/EH_italy_3'+geom_append+'_2_DIR2D_italy_3'+geom_append+'_2/_varspace/_stats/EH_sgs_mean.csv'
obj_file2=script_dir+'/../../MUSE_test/13_RISL_tri'+project_append+'/out/geometry/surf/italy_3'+geom_append+'_2.obj'

csv_file3=script_dir+'/../../MUSE_test/13_RISL_tri'+project_append+'/out/compute/EH_italy_3'+geom_append+'_3_DIR2D_italy_3'+geom_append+'_3/_varspace/_stats/EH_sgs_mean.csv'
obj_file3=script_dir+'/../../MUSE_test/13_RISL_tri'+project_append+'/out/geometry/surf/italy_3'+geom_append+'_3.obj'


# create a new 'CSV Reader'
eH_sgs_meancsv = CSVReader(registrationName='EH_sgs_mean.csv', FileName=[csv_file1])
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
italy_3_1_resobj = WavefrontOBJReader(registrationName='italy_3_1_res.obj', FileName=obj_file1)

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[italy_3_1_resobj, pointDatatoCellData1])




# create a new 'CSV Reader'
eH_sgs_meancsv_1 = CSVReader(registrationName='EH_sgs_mean.csv', FileName=[csv_file2])
eH_sgs_meancsv_1.UseStringDelimiter = 0
eH_sgs_meancsv_1.HaveHeaders = 0
eH_sgs_meancsv_1.FieldDelimiterCharacters = ' '
eH_sgs_meancsv_1.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints2 = TableToPoints(registrationName='TableToPoints2', Input=eH_sgs_meancsv_1)
tableToPoints2.XColumn = 'Field 0'
tableToPoints2.YColumn = 'Field 0'
tableToPoints2.ZColumn = 'Field 0'
tableToPoints2.a2DPoints = 1
tableToPoints2.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate2 = Triangulate(registrationName='Triangulate2', Input=tableToPoints2)

# create a new 'Point Data to Cell Data'
pointDatatoCellData2 = PointDatatoCellData(registrationName='PointDatatoCellData2', Input=triangulate2)
pointDatatoCellData2.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData2.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
italy_3_2_resobj = WavefrontOBJReader(registrationName='italy_3_2_res.obj', FileName=obj_file2)

# create a new 'Append Attributes'
appendAttributes2 = AppendAttributes(registrationName='AppendAttributes2', Input=[italy_3_2_resobj, pointDatatoCellData2])


# create a new 'CSV Reader'
eH_sgs_meancsv_2 = CSVReader(registrationName='EH_sgs_mean.csv', FileName=[csv_file3])
eH_sgs_meancsv_2.UseStringDelimiter = 0
eH_sgs_meancsv_2.HaveHeaders = 0
eH_sgs_meancsv_2.FieldDelimiterCharacters = ' '
eH_sgs_meancsv_2.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints3 = TableToPoints(registrationName='TableToPoints3', Input=eH_sgs_meancsv_2)
tableToPoints3.XColumn = 'Field 0'
tableToPoints3.YColumn = 'Field 0'
tableToPoints3.ZColumn = 'Field 0'
tableToPoints3.a2DPoints = 1
tableToPoints3.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate3 = Triangulate(registrationName='Triangulate3', Input=tableToPoints3)

# create a new 'Point Data to Cell Data'
pointDatatoCellData3 = PointDatatoCellData(registrationName='PointDatatoCellData3', Input=triangulate3)
pointDatatoCellData3.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData3.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
italy_3_3_resobj = WavefrontOBJReader(registrationName='italy_3_3_res.obj', FileName=obj_file3)

# create a new 'Append Attributes'
appendAttributes3 = AppendAttributes(registrationName='AppendAttributes3', Input=[italy_3_3_resobj, pointDatatoCellData3])



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


# show data from italy_3_2_resobj
italy_3_2_resobjDisplay = Show(italy_3_2_resobj, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes2Display = Show(appendAttributes2, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes2Display.Representation = 'Surface'
appendAttributes2Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes2Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes2Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]


# show data from italy_3_3_resobj
italy_3_3_resobjDisplay = Show(italy_3_3_resobj, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes3Display = Show(appendAttributes3, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes3Display.Representation = 'Surface'
appendAttributes3Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes3Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes3Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]




# setup the color legend parameters for each legend in this view

# get color legend/bar for field0LUT in view renderView1
field0LUTColorBar = GetScalarBar(field0LUT, renderView1)
field0LUTColorBar.Title = 'Field 0'
field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
field0LUTColorBar.Visibility = 1

# show color legend
appendAttributes1Display.SetScalarBarVisibility(renderView1, True)

# show color legend
appendAttributes2Display.SetScalarBarVisibility(renderView1, True)

# show color legend
appendAttributes3Display.SetScalarBarVisibility(renderView1, True)

# ----------------------------------------------------------------
# setup color maps and opacity mapes used in the visualization
# note: the Get..() functions create a new object, if needed
# ----------------------------------------------------------------

# get opacity transfer function/opacity map for 'Field0'
field0PWF = GetOpacityTransferFunction('Field0')
field0PWF.Points = [0.0836820019, 0.0, 0.5, 0.0, 14.4433432, 1.0, 0.5, 0.0]
field0PWF.ScalarRangeInitialized = 1


# ----------------------------------------------------------------
# restore active source
SetActiveSource(appendAttributes3)
# ----------------------------------------------------------------

if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')
