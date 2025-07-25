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
renderView1.ViewSize = [1456, 784]
renderView1.InteractionMode = '2D'
renderView1.AxesGrid = 'GridAxes3DActor'
renderView1.CenterOfRotation = [50.0, 1.0, -7.5]
renderView1.StereoType = 'Crystal Eyes'
renderView1.CameraPosition = [50.0, -194.34640899538383, -7.5]
renderView1.CameraFocalPoint = [50.0, 1.0, -7.5]
renderView1.CameraViewUp = [0.0, 0.0, 1.0]
renderView1.CameraFocalDisk = 1.0
renderView1.CameraParallelScale = 28.539446872217038
renderView1.Background = [0.32, 0.34, 0.43]

SetActiveView(None)

# ----------------------------------------------------------------
# setup view layouts
# ----------------------------------------------------------------

# create new layout object 'Layout #1'
layout1 = CreateLayout(name='Layout #1')
layout1.AssignView(0, renderView1)
layout1.SetSize(1456, 784)

# ----------------------------------------------------------------
# restore active view
SetActiveView(renderView1)
# ----------------------------------------------------------------

# ----------------------------------------------------------------
# setup the data processing pipelines
# ----------------------------------------------------------------

# create a new 'CSV Reader'
pdf_cat_3csv = CSVReader(registrationName='pdf_cat_3.csv', FileName=['/home/marianna/muse/examples/MUSE_test/14_sec_2D_4000_tri_ind/out/compute/phi_sec_DIR2D_sec/pdf_cat_3.csv'])
pdf_cat_3csv.UseStringDelimiter = 0
pdf_cat_3csv.HaveHeaders = 0
pdf_cat_3csv.FieldDelimiterCharacters = ' '
pdf_cat_3csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints5 = TableToPoints(registrationName='TableToPoints5', Input=pdf_cat_3csv)
tableToPoints5.XColumn = 'Field 0'
tableToPoints5.YColumn = 'Field 0'
tableToPoints5.ZColumn = 'Field 0'
tableToPoints5.KeepAllDataArrays = 1

# create a new 'CSV Reader'
pdf_cat_6csv = CSVReader(registrationName='pdf_cat_6.csv', FileName=['/home/marianna/muse/examples/MUSE_test/14_sec_2D_4000_tri_ind/out/compute/phi_sec_DIR2D_sec/pdf_cat_6.csv'])
pdf_cat_6csv.UseStringDelimiter = 0
pdf_cat_6csv.HaveHeaders = 0
pdf_cat_6csv.FieldDelimiterCharacters = ' '
pdf_cat_6csv.AddTabFieldDelimiter = 1

# create a new 'Triangulate'
triangulate5 = Triangulate(registrationName='Triangulate5', Input=tableToPoints5)

# create a new 'CSV Reader'
pdf_cat_4csv = CSVReader(registrationName='pdf_cat_4.csv', FileName=['/home/marianna/muse/examples/MUSE_test/14_sec_2D_4000_tri_ind/out/compute/phi_sec_DIR2D_sec/pdf_cat_4.csv'])
pdf_cat_4csv.UseStringDelimiter = 0
pdf_cat_4csv.HaveHeaders = 0
pdf_cat_4csv.FieldDelimiterCharacters = ' '
pdf_cat_4csv.AddTabFieldDelimiter = 1

# create a new 'CSV Reader'
pdf_cat_2csv = CSVReader(registrationName='pdf_cat_2.csv', FileName=['/home/marianna/muse/examples/MUSE_test/14_sec_2D_4000_tri_ind/out/compute/phi_sec_DIR2D_sec/pdf_cat_2.csv'])
pdf_cat_2csv.UseStringDelimiter = 0
pdf_cat_2csv.HaveHeaders = 0
pdf_cat_2csv.FieldDelimiterCharacters = ' '
pdf_cat_2csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints4 = TableToPoints(registrationName='TableToPoints4', Input=pdf_cat_2csv)
tableToPoints4.XColumn = 'Field 0'
tableToPoints4.YColumn = 'Field 0'
tableToPoints4.ZColumn = 'Field 0'
tableToPoints4.KeepAllDataArrays = 1

# create a new 'CSV Reader'
amga_class_v_4000copiacsv = CSVReader(registrationName='amga_class_v_4000 (copia).csv', FileName=['/home/marianna/muse/examples/14_sec_2D_4000_tri_ind/data/amga_class_v_4000 (copia).csv'])
amga_class_v_4000copiacsv.UseStringDelimiter = 0
amga_class_v_4000copiacsv.FieldDelimiterCharacters = ';'
amga_class_v_4000copiacsv.AddTabFieldDelimiter = 1

# create a new 'Triangulate'
triangulate4 = Triangulate(registrationName='Triangulate4', Input=tableToPoints4)

# create a new 'CSV Reader'
pdf_cat_5csv = CSVReader(registrationName='pdf_cat_5.csv', FileName=['/home/marianna/muse/examples/MUSE_test/14_sec_2D_4000_tri_ind/out/compute/phi_sec_DIR2D_sec/pdf_cat_5.csv'])
pdf_cat_5csv.UseStringDelimiter = 0
pdf_cat_5csv.HaveHeaders = 0
pdf_cat_5csv.FieldDelimiterCharacters = ' '
pdf_cat_5csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints10 = TableToPoints(registrationName='TableToPoints10', Input=amga_class_v_4000copiacsv)
tableToPoints10.XColumn = 'X'
tableToPoints10.YColumn = 'Y'
tableToPoints10.ZColumn = 'Z'
tableToPoints10.KeepAllDataArrays = 1

# create a new 'Wavefront OBJ Reader'
sec_rotobj = WavefrontOBJReader(registrationName='sec_rot.obj', FileName='/home/marianna/muse/examples/MUSE_test/14_sec_2D_4000_tri_ind/out/geometry/surf/sec_rot.obj')

# create a new 'Table To Points'
tableToPoints8 = TableToPoints(registrationName='TableToPoints8', Input=pdf_cat_6csv)
tableToPoints8.XColumn = 'Field 0'
tableToPoints8.YColumn = 'Field 0'
tableToPoints8.ZColumn = 'Field 0'
tableToPoints8.KeepAllDataArrays = 1

# create a new 'CSV Reader'
pdf_cat_7csv = CSVReader(registrationName='pdf_cat_7.csv', FileName=['/home/marianna/muse/examples/MUSE_test/14_sec_2D_4000_tri_ind/out/compute/phi_sec_DIR2D_sec/pdf_cat_7.csv'])
pdf_cat_7csv.UseStringDelimiter = 0
pdf_cat_7csv.HaveHeaders = 0
pdf_cat_7csv.FieldDelimiterCharacters = ' '
pdf_cat_7csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints9 = TableToPoints(registrationName='TableToPoints9', Input=pdf_cat_7csv)
tableToPoints9.XColumn = 'Field 0'
tableToPoints9.YColumn = 'Field 0'
tableToPoints9.ZColumn = 'Field 0'
tableToPoints9.KeepAllDataArrays = 1

# create a new 'Point Data to Cell Data'
pointDatatoCellData5 = PointDatatoCellData(registrationName='PointDatatoCellData5', Input=triangulate5)
pointDatatoCellData5.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData5.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat3 = AppendAttributes(registrationName='pdf_cat3', Input=[sec_rotobj, pointDatatoCellData5])

# create a new 'Table To Points'
tableToPoints7 = TableToPoints(registrationName='TableToPoints7', Input=pdf_cat_5csv)
tableToPoints7.XColumn = 'Field 0'
tableToPoints7.YColumn = 'Field 0'
tableToPoints7.ZColumn = 'Field 0'
tableToPoints7.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate7 = Triangulate(registrationName='Triangulate7', Input=tableToPoints7)

# create a new 'Triangulate'
triangulate9 = Triangulate(registrationName='Triangulate9', Input=tableToPoints9)

# create a new 'Point Data to Cell Data'
pointDatatoCellData9 = PointDatatoCellData(registrationName='PointDatatoCellData9', Input=triangulate9)
pointDatatoCellData9.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData9.PassPointData = 1

# create a new 'Triangulate'
triangulate8 = Triangulate(registrationName='Triangulate8', Input=tableToPoints8)

# create a new 'Point Data to Cell Data'
pointDatatoCellData8 = PointDatatoCellData(registrationName='PointDatatoCellData8', Input=triangulate8)
pointDatatoCellData8.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData8.PassPointData = 1

# create a new 'Table To Points'
tableToPoints6 = TableToPoints(registrationName='TableToPoints6', Input=pdf_cat_4csv)
tableToPoints6.XColumn = 'Field 0'
tableToPoints6.YColumn = 'Field 0'
tableToPoints6.ZColumn = 'Field 0'
tableToPoints6.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate6 = Triangulate(registrationName='Triangulate6', Input=tableToPoints6)

# create a new 'Point Data to Cell Data'
pointDatatoCellData6 = PointDatatoCellData(registrationName='PointDatatoCellData6', Input=triangulate6)
pointDatatoCellData6.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData6.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat6 = AppendAttributes(registrationName='pdf_cat6', Input=[sec_rotobj, pointDatatoCellData8])

# create a new 'Append Attributes'
pdf_cat4 = AppendAttributes(registrationName='pdf_cat4', Input=[sec_rotobj, pointDatatoCellData6])

# create a new 'Point Data to Cell Data'
pointDatatoCellData4 = PointDatatoCellData(registrationName='PointDatatoCellData4', Input=triangulate4)
pointDatatoCellData4.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData4.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat2 = AppendAttributes(registrationName='pdf_cat2', Input=[sec_rotobj, pointDatatoCellData4])

# create a new 'CSV Reader'
pdf_cat_1csv = CSVReader(registrationName='pdf_cat_1.csv', FileName=['/home/marianna/muse/examples/MUSE_test/14_sec_2D_4000_tri_ind/out/compute/phi_sec_DIR2D_sec/pdf_cat_1.csv'])
pdf_cat_1csv.UseStringDelimiter = 0
pdf_cat_1csv.HaveHeaders = 0
pdf_cat_1csv.FieldDelimiterCharacters = ' '
pdf_cat_1csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints3 = TableToPoints(registrationName='TableToPoints3', Input=pdf_cat_1csv)
tableToPoints3.XColumn = 'Field 0'
tableToPoints3.YColumn = 'Field 0'
tableToPoints3.ZColumn = 'Field 0'
tableToPoints3.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate3 = Triangulate(registrationName='Triangulate3', Input=tableToPoints3)

# create a new 'Point Data to Cell Data'
pointDatatoCellData3 = PointDatatoCellData(registrationName='PointDatatoCellData3', Input=triangulate3)
pointDatatoCellData3.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData3.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat1 = AppendAttributes(registrationName='pdf_cat1', Input=[sec_rotobj, pointDatatoCellData3])

# create a new 'CSV Reader'
pdf_cat_0csv = CSVReader(registrationName='pdf_cat_0.csv', FileName=['/home/marianna/muse/examples/MUSE_test/14_sec_2D_4000_tri_ind/out/compute/phi_sec_DIR2D_sec/pdf_cat_0.csv'])
pdf_cat_0csv.UseStringDelimiter = 0
pdf_cat_0csv.HaveHeaders = 0
pdf_cat_0csv.FieldDelimiterCharacters = ' '
pdf_cat_0csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints2 = TableToPoints(registrationName='TableToPoints2', Input=pdf_cat_0csv)
tableToPoints2.XColumn = 'Field 0'
tableToPoints2.YColumn = 'Field 0'
tableToPoints2.ZColumn = 'Field 0'
tableToPoints2.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate2 = Triangulate(registrationName='Triangulate2', Input=tableToPoints2)

# create a new 'Point Data to Cell Data'
pointDatatoCellData2 = PointDatatoCellData(registrationName='PointDatatoCellData2', Input=triangulate2)
pointDatatoCellData2.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData2.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat0 = AppendAttributes(registrationName='pdf_cat0', Input=[sec_rotobj, pointDatatoCellData2])

# create a new 'Append Attributes'
pdf_cat7 = AppendAttributes(registrationName='pdf_cat7', Input=[sec_rotobj, pointDatatoCellData9])

# create a new 'CSV Reader'
sisimcsv = CSVReader(registrationName='sisim.csv', FileName=['/home/marianna/muse/examples/MUSE_test/14_sec_2D_4000_tri_ind/out/compute/phi_sec_DIR2D_sec/sisim.csv'])
sisimcsv.UseStringDelimiter = 0
sisimcsv.HaveHeaders = 0
sisimcsv.FieldDelimiterCharacters = ' '
sisimcsv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints1 = TableToPoints(registrationName='TableToPoints1', Input=sisimcsv)
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

# create a new 'Append Attributes'
best = AppendAttributes(registrationName='best', Input=[sec_rotobj, pointDatatoCellData1])

# create a new 'Point Data to Cell Data'
pointDatatoCellData7 = PointDatatoCellData(registrationName='PointDatatoCellData7', Input=triangulate7)
pointDatatoCellData7.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData7.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat5 = AppendAttributes(registrationName='pdf_cat5', Input=[sec_rotobj, pointDatatoCellData7])

# ----------------------------------------------------------------
# setup the visualization in view 'renderView1'
# ----------------------------------------------------------------

# show data from sec_rotobj
sec_rotobjDisplay = Show(sec_rotobj, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
sec_rotobjDisplay.Representation = 'Surface'
sec_rotobjDisplay.ColorArrayName = [None, '']
sec_rotobjDisplay.SelectTCoordArray = 'None'
sec_rotobjDisplay.SelectNormalArray = 'None'
sec_rotobjDisplay.SelectTangentArray = 'None'
sec_rotobjDisplay.OSPRayScaleFunction = 'PiecewiseFunction'
sec_rotobjDisplay.SelectOrientationVectors = 'None'
sec_rotobjDisplay.ScaleFactor = 10.0
sec_rotobjDisplay.SelectScaleArray = 'None'
sec_rotobjDisplay.GlyphType = 'Arrow'
sec_rotobjDisplay.GlyphTableIndexArray = 'None'
sec_rotobjDisplay.GaussianRadius = 0.5
sec_rotobjDisplay.SetScaleArray = [None, '']
sec_rotobjDisplay.ScaleTransferFunction = 'PiecewiseFunction'
sec_rotobjDisplay.OpacityArray = [None, '']
sec_rotobjDisplay.OpacityTransferFunction = 'PiecewiseFunction'
sec_rotobjDisplay.DataAxesGrid = 'GridAxesRepresentation'
sec_rotobjDisplay.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
sec_rotobjDisplay.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# show data from best
bestDisplay = Show(best, renderView1, 'GeometryRepresentation')

# get color transfer function/color map for 'Field0'
field0LUT = GetColorTransferFunction('Field0')
field0LUT.RGBPoints = [1.0, 0.231373, 0.298039, 0.752941, 4.5, 0.865003, 0.865003, 0.865003, 8.0, 0.705882, 0.0156863, 0.14902]
field0LUT.NumberOfTableValues = 8
field0LUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
bestDisplay.Representation = 'Surface'
bestDisplay.ColorArrayName = ['CELLS', 'Field 0']
bestDisplay.LookupTable = field0LUT
bestDisplay.SelectTCoordArray = 'None'
bestDisplay.SelectNormalArray = 'None'
bestDisplay.SelectTangentArray = 'None'
bestDisplay.OSPRayScaleFunction = 'PiecewiseFunction'
bestDisplay.SelectOrientationVectors = 'None'
bestDisplay.ScaleFactor = 10.0
bestDisplay.SelectScaleArray = 'None'
bestDisplay.GlyphType = 'Arrow'
bestDisplay.GlyphTableIndexArray = 'None'
bestDisplay.GaussianRadius = 0.5
bestDisplay.SetScaleArray = [None, '']
bestDisplay.ScaleTransferFunction = 'PiecewiseFunction'
bestDisplay.OpacityArray = [None, '']
bestDisplay.OpacityTransferFunction = 'PiecewiseFunction'
bestDisplay.DataAxesGrid = 'GridAxesRepresentation'
bestDisplay.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
bestDisplay.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# show data from pdf_cat0
pdf_cat0Display = Show(pdf_cat0, renderView1, 'GeometryRepresentation')

# get separate color transfer function/color map for 'Field0'
separate_pdf_cat0Display_Field0LUT = GetColorTransferFunction('Field0', pdf_cat0Display, separate=True)
separate_pdf_cat0Display_Field0LUT.RGBPoints = [5.23993e-11, 0.231373, 0.298039, 0.752941, 0.5000000000261996, 0.865003, 0.865003, 0.865003, 1.0, 0.705882, 0.0156863, 0.14902]
separate_pdf_cat0Display_Field0LUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
pdf_cat0Display.Representation = 'Surface'
pdf_cat0Display.ColorArrayName = ['CELLS', 'Field 0']
pdf_cat0Display.LookupTable = separate_pdf_cat0Display_Field0LUT
pdf_cat0Display.SelectTCoordArray = 'None'
pdf_cat0Display.SelectNormalArray = 'None'
pdf_cat0Display.SelectTangentArray = 'None'
pdf_cat0Display.OSPRayScaleFunction = 'PiecewiseFunction'
pdf_cat0Display.SelectOrientationVectors = 'None'
pdf_cat0Display.ScaleFactor = 10.0
pdf_cat0Display.SelectScaleArray = 'None'
pdf_cat0Display.GlyphType = 'Arrow'
pdf_cat0Display.GlyphTableIndexArray = 'None'
pdf_cat0Display.GaussianRadius = 0.5
pdf_cat0Display.SetScaleArray = [None, '']
pdf_cat0Display.ScaleTransferFunction = 'PiecewiseFunction'
pdf_cat0Display.OpacityArray = [None, '']
pdf_cat0Display.OpacityTransferFunction = 'PiecewiseFunction'
pdf_cat0Display.DataAxesGrid = 'GridAxesRepresentation'
pdf_cat0Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
pdf_cat0Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set separate color map
pdf_cat0Display.UseSeparateColorMap = True

# show data from tableToPoints3
tableToPoints3Display = Show(tableToPoints3, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
tableToPoints3Display.Representation = 'Surface'
tableToPoints3Display.ColorArrayName = [None, '']
tableToPoints3Display.SelectTCoordArray = 'None'
tableToPoints3Display.SelectNormalArray = 'None'
tableToPoints3Display.SelectTangentArray = 'None'
tableToPoints3Display.OSPRayScaleArray = 'Field 0'
tableToPoints3Display.OSPRayScaleFunction = 'PiecewiseFunction'
tableToPoints3Display.SelectOrientationVectors = 'None'
tableToPoints3Display.ScaleFactor = 0.09999999999476007
tableToPoints3Display.SelectScaleArray = 'Field 0'
tableToPoints3Display.GlyphType = 'Arrow'
tableToPoints3Display.GlyphTableIndexArray = 'Field 0'
tableToPoints3Display.GaussianRadius = 0.004999999999738004
tableToPoints3Display.SetScaleArray = ['POINTS', 'Field 0']
tableToPoints3Display.ScaleTransferFunction = 'PiecewiseFunction'
tableToPoints3Display.OpacityArray = ['POINTS', 'Field 0']
tableToPoints3Display.OpacityTransferFunction = 'PiecewiseFunction'
tableToPoints3Display.DataAxesGrid = 'GridAxesRepresentation'
tableToPoints3Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
tableToPoints3Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
tableToPoints3Display.ScaleTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
tableToPoints3Display.OpacityTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# show data from triangulate3
triangulate3Display = Show(triangulate3, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
triangulate3Display.Representation = 'Surface'
triangulate3Display.ColorArrayName = [None, '']
triangulate3Display.SelectTCoordArray = 'None'
triangulate3Display.SelectNormalArray = 'None'
triangulate3Display.SelectTangentArray = 'None'
triangulate3Display.OSPRayScaleArray = 'Field 0'
triangulate3Display.OSPRayScaleFunction = 'PiecewiseFunction'
triangulate3Display.SelectOrientationVectors = 'None'
triangulate3Display.ScaleFactor = 0.09999999999476007
triangulate3Display.SelectScaleArray = 'Field 0'
triangulate3Display.GlyphType = 'Arrow'
triangulate3Display.GlyphTableIndexArray = 'Field 0'
triangulate3Display.GaussianRadius = 0.004999999999738004
triangulate3Display.SetScaleArray = ['POINTS', 'Field 0']
triangulate3Display.ScaleTransferFunction = 'PiecewiseFunction'
triangulate3Display.OpacityArray = ['POINTS', 'Field 0']
triangulate3Display.OpacityTransferFunction = 'PiecewiseFunction'
triangulate3Display.DataAxesGrid = 'GridAxesRepresentation'
triangulate3Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
triangulate3Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
triangulate3Display.ScaleTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
triangulate3Display.OpacityTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# show data from pointDatatoCellData3
pointDatatoCellData3Display = Show(pointDatatoCellData3, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
pointDatatoCellData3Display.Representation = 'Surface'
pointDatatoCellData3Display.ColorArrayName = [None, '']
pointDatatoCellData3Display.SelectTCoordArray = 'None'
pointDatatoCellData3Display.SelectNormalArray = 'None'
pointDatatoCellData3Display.SelectTangentArray = 'None'
pointDatatoCellData3Display.OSPRayScaleArray = 'Field 0'
pointDatatoCellData3Display.OSPRayScaleFunction = 'PiecewiseFunction'
pointDatatoCellData3Display.SelectOrientationVectors = 'None'
pointDatatoCellData3Display.ScaleFactor = 0.09999999999476007
pointDatatoCellData3Display.SelectScaleArray = 'Field 0'
pointDatatoCellData3Display.GlyphType = 'Arrow'
pointDatatoCellData3Display.GlyphTableIndexArray = 'Field 0'
pointDatatoCellData3Display.GaussianRadius = 0.004999999999738004
pointDatatoCellData3Display.SetScaleArray = ['POINTS', 'Field 0']
pointDatatoCellData3Display.ScaleTransferFunction = 'PiecewiseFunction'
pointDatatoCellData3Display.OpacityArray = ['POINTS', 'Field 0']
pointDatatoCellData3Display.OpacityTransferFunction = 'PiecewiseFunction'
pointDatatoCellData3Display.DataAxesGrid = 'GridAxesRepresentation'
pointDatatoCellData3Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
pointDatatoCellData3Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
pointDatatoCellData3Display.ScaleTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
pointDatatoCellData3Display.OpacityTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# show data from pdf_cat1
pdf_cat1Display = Show(pdf_cat1, renderView1, 'GeometryRepresentation')

# get separate color transfer function/color map for 'Field0'
separate_pdf_cat1Display_Field0LUT = GetColorTransferFunction('Field0', pdf_cat1Display, separate=True)
separate_pdf_cat1Display_Field0LUT.RGBPoints = [5.23993e-11, 0.231373, 0.298039, 0.752941, 0.5000000000261996, 0.865003, 0.865003, 0.865003, 1.0, 0.705882, 0.0156863, 0.14902]
separate_pdf_cat1Display_Field0LUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
pdf_cat1Display.Representation = 'Surface'
pdf_cat1Display.ColorArrayName = ['CELLS', 'Field 0']
pdf_cat1Display.LookupTable = separate_pdf_cat1Display_Field0LUT
pdf_cat1Display.SelectTCoordArray = 'None'
pdf_cat1Display.SelectNormalArray = 'None'
pdf_cat1Display.SelectTangentArray = 'None'
pdf_cat1Display.OSPRayScaleFunction = 'PiecewiseFunction'
pdf_cat1Display.SelectOrientationVectors = 'None'
pdf_cat1Display.ScaleFactor = 10.0
pdf_cat1Display.SelectScaleArray = 'None'
pdf_cat1Display.GlyphType = 'Arrow'
pdf_cat1Display.GlyphTableIndexArray = 'None'
pdf_cat1Display.GaussianRadius = 0.5
pdf_cat1Display.SetScaleArray = [None, '']
pdf_cat1Display.ScaleTransferFunction = 'PiecewiseFunction'
pdf_cat1Display.OpacityArray = [None, '']
pdf_cat1Display.OpacityTransferFunction = 'PiecewiseFunction'
pdf_cat1Display.DataAxesGrid = 'GridAxesRepresentation'
pdf_cat1Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
pdf_cat1Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set separate color map
pdf_cat1Display.UseSeparateColorMap = True

# show data from tableToPoints4
tableToPoints4Display = Show(tableToPoints4, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
tableToPoints4Display.Representation = 'Surface'
tableToPoints4Display.ColorArrayName = [None, '']
tableToPoints4Display.SelectTCoordArray = 'None'
tableToPoints4Display.SelectNormalArray = 'None'
tableToPoints4Display.SelectTangentArray = 'None'
tableToPoints4Display.OSPRayScaleArray = 'Field 0'
tableToPoints4Display.OSPRayScaleFunction = 'PiecewiseFunction'
tableToPoints4Display.SelectOrientationVectors = 'None'
tableToPoints4Display.ScaleFactor = 0.0999999999943651
tableToPoints4Display.SelectScaleArray = 'Field 0'
tableToPoints4Display.GlyphType = 'Arrow'
tableToPoints4Display.GlyphTableIndexArray = 'Field 0'
tableToPoints4Display.GaussianRadius = 0.004999999999718254
tableToPoints4Display.SetScaleArray = ['POINTS', 'Field 0']
tableToPoints4Display.ScaleTransferFunction = 'PiecewiseFunction'
tableToPoints4Display.OpacityArray = ['POINTS', 'Field 0']
tableToPoints4Display.OpacityTransferFunction = 'PiecewiseFunction'
tableToPoints4Display.DataAxesGrid = 'GridAxesRepresentation'
tableToPoints4Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
tableToPoints4Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
tableToPoints4Display.ScaleTransferFunction.Points = [5.63492e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
tableToPoints4Display.OpacityTransferFunction.Points = [5.63492e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# show data from triangulate4
triangulate4Display = Show(triangulate4, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
triangulate4Display.Representation = 'Surface'
triangulate4Display.ColorArrayName = [None, '']
triangulate4Display.SelectTCoordArray = 'None'
triangulate4Display.SelectNormalArray = 'None'
triangulate4Display.SelectTangentArray = 'None'
triangulate4Display.OSPRayScaleArray = 'Field 0'
triangulate4Display.OSPRayScaleFunction = 'PiecewiseFunction'
triangulate4Display.SelectOrientationVectors = 'None'
triangulate4Display.ScaleFactor = 0.0999999999943651
triangulate4Display.SelectScaleArray = 'Field 0'
triangulate4Display.GlyphType = 'Arrow'
triangulate4Display.GlyphTableIndexArray = 'Field 0'
triangulate4Display.GaussianRadius = 0.004999999999718254
triangulate4Display.SetScaleArray = ['POINTS', 'Field 0']
triangulate4Display.ScaleTransferFunction = 'PiecewiseFunction'
triangulate4Display.OpacityArray = ['POINTS', 'Field 0']
triangulate4Display.OpacityTransferFunction = 'PiecewiseFunction'
triangulate4Display.DataAxesGrid = 'GridAxesRepresentation'
triangulate4Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
triangulate4Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
triangulate4Display.ScaleTransferFunction.Points = [5.63492e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
triangulate4Display.OpacityTransferFunction.Points = [5.63492e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# show data from pointDatatoCellData4
pointDatatoCellData4Display = Show(pointDatatoCellData4, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
pointDatatoCellData4Display.Representation = 'Surface'
pointDatatoCellData4Display.ColorArrayName = [None, '']
pointDatatoCellData4Display.SelectTCoordArray = 'None'
pointDatatoCellData4Display.SelectNormalArray = 'None'
pointDatatoCellData4Display.SelectTangentArray = 'None'
pointDatatoCellData4Display.OSPRayScaleArray = 'Field 0'
pointDatatoCellData4Display.OSPRayScaleFunction = 'PiecewiseFunction'
pointDatatoCellData4Display.SelectOrientationVectors = 'None'
pointDatatoCellData4Display.ScaleFactor = 0.0999999999943651
pointDatatoCellData4Display.SelectScaleArray = 'Field 0'
pointDatatoCellData4Display.GlyphType = 'Arrow'
pointDatatoCellData4Display.GlyphTableIndexArray = 'Field 0'
pointDatatoCellData4Display.GaussianRadius = 0.004999999999718254
pointDatatoCellData4Display.SetScaleArray = ['POINTS', 'Field 0']
pointDatatoCellData4Display.ScaleTransferFunction = 'PiecewiseFunction'
pointDatatoCellData4Display.OpacityArray = ['POINTS', 'Field 0']
pointDatatoCellData4Display.OpacityTransferFunction = 'PiecewiseFunction'
pointDatatoCellData4Display.DataAxesGrid = 'GridAxesRepresentation'
pointDatatoCellData4Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
pointDatatoCellData4Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
pointDatatoCellData4Display.ScaleTransferFunction.Points = [5.63492e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
pointDatatoCellData4Display.OpacityTransferFunction.Points = [5.63492e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# show data from pdf_cat2
pdf_cat2Display = Show(pdf_cat2, renderView1, 'GeometryRepresentation')

# get separate color transfer function/color map for 'Field0'
separate_pdf_cat2Display_Field0LUT = GetColorTransferFunction('Field0', pdf_cat2Display, separate=True)
separate_pdf_cat2Display_Field0LUT.RGBPoints = [5.63492e-11, 0.231373, 0.298039, 0.752941, 0.5000000000281746, 0.865003, 0.865003, 0.865003, 1.0, 0.705882, 0.0156863, 0.14902]
separate_pdf_cat2Display_Field0LUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
pdf_cat2Display.Representation = 'Surface'
pdf_cat2Display.ColorArrayName = ['CELLS', 'Field 0']
pdf_cat2Display.LookupTable = separate_pdf_cat2Display_Field0LUT
pdf_cat2Display.SelectTCoordArray = 'None'
pdf_cat2Display.SelectNormalArray = 'None'
pdf_cat2Display.SelectTangentArray = 'None'
pdf_cat2Display.OSPRayScaleFunction = 'PiecewiseFunction'
pdf_cat2Display.SelectOrientationVectors = 'None'
pdf_cat2Display.ScaleFactor = 10.0
pdf_cat2Display.SelectScaleArray = 'None'
pdf_cat2Display.GlyphType = 'Arrow'
pdf_cat2Display.GlyphTableIndexArray = 'None'
pdf_cat2Display.GaussianRadius = 0.5
pdf_cat2Display.SetScaleArray = [None, '']
pdf_cat2Display.ScaleTransferFunction = 'PiecewiseFunction'
pdf_cat2Display.OpacityArray = [None, '']
pdf_cat2Display.OpacityTransferFunction = 'PiecewiseFunction'
pdf_cat2Display.DataAxesGrid = 'GridAxesRepresentation'
pdf_cat2Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
pdf_cat2Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set separate color map
pdf_cat2Display.UseSeparateColorMap = True

# show data from pdf_cat3
pdf_cat3Display = Show(pdf_cat3, renderView1, 'GeometryRepresentation')

# get separate color transfer function/color map for 'Field0'
separate_pdf_cat3Display_Field0LUT = GetColorTransferFunction('Field0', pdf_cat3Display, separate=True)
separate_pdf_cat3Display_Field0LUT.RGBPoints = [5.23993e-11, 0.231373, 0.298039, 0.752941, 0.5000000000261996, 0.865003, 0.865003, 0.865003, 1.0, 0.705882, 0.0156863, 0.14902]
separate_pdf_cat3Display_Field0LUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
pdf_cat3Display.Representation = 'Surface'
pdf_cat3Display.ColorArrayName = ['CELLS', 'Field 0']
pdf_cat3Display.LookupTable = separate_pdf_cat3Display_Field0LUT
pdf_cat3Display.SelectTCoordArray = 'None'
pdf_cat3Display.SelectNormalArray = 'None'
pdf_cat3Display.SelectTangentArray = 'None'
pdf_cat3Display.OSPRayScaleFunction = 'PiecewiseFunction'
pdf_cat3Display.SelectOrientationVectors = 'None'
pdf_cat3Display.ScaleFactor = 10.0
pdf_cat3Display.SelectScaleArray = 'None'
pdf_cat3Display.GlyphType = 'Arrow'
pdf_cat3Display.GlyphTableIndexArray = 'None'
pdf_cat3Display.GaussianRadius = 0.5
pdf_cat3Display.SetScaleArray = [None, '']
pdf_cat3Display.ScaleTransferFunction = 'PiecewiseFunction'
pdf_cat3Display.OpacityArray = [None, '']
pdf_cat3Display.OpacityTransferFunction = 'PiecewiseFunction'
pdf_cat3Display.DataAxesGrid = 'GridAxesRepresentation'
pdf_cat3Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
pdf_cat3Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set separate color map
pdf_cat3Display.UseSeparateColorMap = True

# show data from pdf_cat4
pdf_cat4Display = Show(pdf_cat4, renderView1, 'GeometryRepresentation')

# get separate color transfer function/color map for 'Field0'
separate_pdf_cat4Display_Field0LUT = GetColorTransferFunction('Field0', pdf_cat4Display, separate=True)
separate_pdf_cat4Display_Field0LUT.RGBPoints = [6.51583e-11, 0.231373, 0.298039, 0.752941, 0.5000000000325792, 0.865003, 0.865003, 0.865003, 1.0, 0.705882, 0.0156863, 0.14902]
separate_pdf_cat4Display_Field0LUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
pdf_cat4Display.Representation = 'Surface'
pdf_cat4Display.ColorArrayName = ['CELLS', 'Field 0']
pdf_cat4Display.LookupTable = separate_pdf_cat4Display_Field0LUT
pdf_cat4Display.SelectTCoordArray = 'None'
pdf_cat4Display.SelectNormalArray = 'None'
pdf_cat4Display.SelectTangentArray = 'None'
pdf_cat4Display.OSPRayScaleFunction = 'PiecewiseFunction'
pdf_cat4Display.SelectOrientationVectors = 'None'
pdf_cat4Display.ScaleFactor = 10.0
pdf_cat4Display.SelectScaleArray = 'None'
pdf_cat4Display.GlyphType = 'Arrow'
pdf_cat4Display.GlyphTableIndexArray = 'None'
pdf_cat4Display.GaussianRadius = 0.5
pdf_cat4Display.SetScaleArray = [None, '']
pdf_cat4Display.ScaleTransferFunction = 'PiecewiseFunction'
pdf_cat4Display.OpacityArray = [None, '']
pdf_cat4Display.OpacityTransferFunction = 'PiecewiseFunction'
pdf_cat4Display.DataAxesGrid = 'GridAxesRepresentation'
pdf_cat4Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
pdf_cat4Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set separate color map
pdf_cat4Display.UseSeparateColorMap = True

# show data from tableToPoints7
tableToPoints7Display = Show(tableToPoints7, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
tableToPoints7Display.Representation = 'Surface'
tableToPoints7Display.ColorArrayName = [None, '']
tableToPoints7Display.SelectTCoordArray = 'None'
tableToPoints7Display.SelectNormalArray = 'None'
tableToPoints7Display.SelectTangentArray = 'None'
tableToPoints7Display.OSPRayScaleArray = 'Field 0'
tableToPoints7Display.OSPRayScaleFunction = 'PiecewiseFunction'
tableToPoints7Display.SelectOrientationVectors = 'None'
tableToPoints7Display.ScaleFactor = 0.09999999999416966
tableToPoints7Display.SelectScaleArray = 'Field 0'
tableToPoints7Display.GlyphType = 'Arrow'
tableToPoints7Display.GlyphTableIndexArray = 'Field 0'
tableToPoints7Display.GaussianRadius = 0.004999999999708482
tableToPoints7Display.SetScaleArray = ['POINTS', 'Field 0']
tableToPoints7Display.ScaleTransferFunction = 'PiecewiseFunction'
tableToPoints7Display.OpacityArray = ['POINTS', 'Field 0']
tableToPoints7Display.OpacityTransferFunction = 'PiecewiseFunction'
tableToPoints7Display.DataAxesGrid = 'GridAxesRepresentation'
tableToPoints7Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
tableToPoints7Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
tableToPoints7Display.ScaleTransferFunction.Points = [5.83035e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
tableToPoints7Display.OpacityTransferFunction.Points = [5.83035e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# show data from triangulate7
triangulate7Display = Show(triangulate7, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
triangulate7Display.Representation = 'Surface'
triangulate7Display.ColorArrayName = [None, '']
triangulate7Display.SelectTCoordArray = 'None'
triangulate7Display.SelectNormalArray = 'None'
triangulate7Display.SelectTangentArray = 'None'
triangulate7Display.OSPRayScaleArray = 'Field 0'
triangulate7Display.OSPRayScaleFunction = 'PiecewiseFunction'
triangulate7Display.SelectOrientationVectors = 'None'
triangulate7Display.ScaleFactor = 0.09999999999416966
triangulate7Display.SelectScaleArray = 'Field 0'
triangulate7Display.GlyphType = 'Arrow'
triangulate7Display.GlyphTableIndexArray = 'Field 0'
triangulate7Display.GaussianRadius = 0.004999999999708482
triangulate7Display.SetScaleArray = ['POINTS', 'Field 0']
triangulate7Display.ScaleTransferFunction = 'PiecewiseFunction'
triangulate7Display.OpacityArray = ['POINTS', 'Field 0']
triangulate7Display.OpacityTransferFunction = 'PiecewiseFunction'
triangulate7Display.DataAxesGrid = 'GridAxesRepresentation'
triangulate7Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
triangulate7Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
triangulate7Display.ScaleTransferFunction.Points = [5.83035e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
triangulate7Display.OpacityTransferFunction.Points = [5.83035e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# show data from pointDatatoCellData7
pointDatatoCellData7Display = Show(pointDatatoCellData7, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
pointDatatoCellData7Display.Representation = 'Surface'
pointDatatoCellData7Display.ColorArrayName = [None, '']
pointDatatoCellData7Display.SelectTCoordArray = 'None'
pointDatatoCellData7Display.SelectNormalArray = 'None'
pointDatatoCellData7Display.SelectTangentArray = 'None'
pointDatatoCellData7Display.OSPRayScaleArray = 'Field 0'
pointDatatoCellData7Display.OSPRayScaleFunction = 'PiecewiseFunction'
pointDatatoCellData7Display.SelectOrientationVectors = 'None'
pointDatatoCellData7Display.ScaleFactor = 0.09999999999416966
pointDatatoCellData7Display.SelectScaleArray = 'Field 0'
pointDatatoCellData7Display.GlyphType = 'Arrow'
pointDatatoCellData7Display.GlyphTableIndexArray = 'Field 0'
pointDatatoCellData7Display.GaussianRadius = 0.004999999999708482
pointDatatoCellData7Display.SetScaleArray = ['POINTS', 'Field 0']
pointDatatoCellData7Display.ScaleTransferFunction = 'PiecewiseFunction'
pointDatatoCellData7Display.OpacityArray = ['POINTS', 'Field 0']
pointDatatoCellData7Display.OpacityTransferFunction = 'PiecewiseFunction'
pointDatatoCellData7Display.DataAxesGrid = 'GridAxesRepresentation'
pointDatatoCellData7Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
pointDatatoCellData7Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
pointDatatoCellData7Display.ScaleTransferFunction.Points = [5.83035e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
pointDatatoCellData7Display.OpacityTransferFunction.Points = [5.83035e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# show data from pdf_cat5
pdf_cat5Display = Show(pdf_cat5, renderView1, 'GeometryRepresentation')

# get separate color transfer function/color map for 'Field0'
separate_pdf_cat5Display_Field0LUT = GetColorTransferFunction('Field0', pdf_cat5Display, separate=True)
separate_pdf_cat5Display_Field0LUT.RGBPoints = [5.83035e-11, 0.231373, 0.298039, 0.752941, 0.5000000000291518, 0.865003, 0.865003, 0.865003, 1.0, 0.705882, 0.0156863, 0.14902]
separate_pdf_cat5Display_Field0LUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
pdf_cat5Display.Representation = 'Surface'
pdf_cat5Display.ColorArrayName = ['CELLS', 'Field 0']
pdf_cat5Display.LookupTable = separate_pdf_cat5Display_Field0LUT
pdf_cat5Display.SelectTCoordArray = 'None'
pdf_cat5Display.SelectNormalArray = 'None'
pdf_cat5Display.SelectTangentArray = 'None'
pdf_cat5Display.OSPRayScaleFunction = 'PiecewiseFunction'
pdf_cat5Display.SelectOrientationVectors = 'None'
pdf_cat5Display.ScaleFactor = 10.0
pdf_cat5Display.SelectScaleArray = 'None'
pdf_cat5Display.GlyphType = 'Arrow'
pdf_cat5Display.GlyphTableIndexArray = 'None'
pdf_cat5Display.GaussianRadius = 0.5
pdf_cat5Display.SetScaleArray = [None, '']
pdf_cat5Display.ScaleTransferFunction = 'PiecewiseFunction'
pdf_cat5Display.OpacityArray = [None, '']
pdf_cat5Display.OpacityTransferFunction = 'PiecewiseFunction'
pdf_cat5Display.DataAxesGrid = 'GridAxesRepresentation'
pdf_cat5Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
pdf_cat5Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set separate color map
pdf_cat5Display.UseSeparateColorMap = True

# show data from pdf_cat6
pdf_cat6Display = Show(pdf_cat6, renderView1, 'GeometryRepresentation')

# get separate color transfer function/color map for 'Field0'
separate_pdf_cat6Display_Field0LUT = GetColorTransferFunction('Field0', pdf_cat6Display, separate=True)
separate_pdf_cat6Display_Field0LUT.RGBPoints = [5.23993e-11, 0.231373, 0.298039, 0.752941, 0.5000000000261996, 0.865003, 0.865003, 0.865003, 1.0, 0.705882, 0.0156863, 0.14902]
separate_pdf_cat6Display_Field0LUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
pdf_cat6Display.Representation = 'Surface'
pdf_cat6Display.ColorArrayName = ['CELLS', 'Field 0']
pdf_cat6Display.LookupTable = separate_pdf_cat6Display_Field0LUT
pdf_cat6Display.SelectTCoordArray = 'None'
pdf_cat6Display.SelectNormalArray = 'None'
pdf_cat6Display.SelectTangentArray = 'None'
pdf_cat6Display.OSPRayScaleFunction = 'PiecewiseFunction'
pdf_cat6Display.SelectOrientationVectors = 'None'
pdf_cat6Display.ScaleFactor = 10.0
pdf_cat6Display.SelectScaleArray = 'None'
pdf_cat6Display.GlyphType = 'Arrow'
pdf_cat6Display.GlyphTableIndexArray = 'None'
pdf_cat6Display.GaussianRadius = 0.5
pdf_cat6Display.SetScaleArray = [None, '']
pdf_cat6Display.ScaleTransferFunction = 'PiecewiseFunction'
pdf_cat6Display.OpacityArray = [None, '']
pdf_cat6Display.OpacityTransferFunction = 'PiecewiseFunction'
pdf_cat6Display.DataAxesGrid = 'GridAxesRepresentation'
pdf_cat6Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
pdf_cat6Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set separate color map
pdf_cat6Display.UseSeparateColorMap = True

# show data from tableToPoints9
tableToPoints9Display = Show(tableToPoints9, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
tableToPoints9Display.Representation = 'Surface'
tableToPoints9Display.ColorArrayName = [None, '']
tableToPoints9Display.SelectTCoordArray = 'None'
tableToPoints9Display.SelectNormalArray = 'None'
tableToPoints9Display.SelectTangentArray = 'None'
tableToPoints9Display.OSPRayScaleArray = 'Field 0'
tableToPoints9Display.OSPRayScaleFunction = 'PiecewiseFunction'
tableToPoints9Display.SelectOrientationVectors = 'None'
tableToPoints9Display.ScaleFactor = 0.09999999999476007
tableToPoints9Display.SelectScaleArray = 'Field 0'
tableToPoints9Display.GlyphType = 'Arrow'
tableToPoints9Display.GlyphTableIndexArray = 'Field 0'
tableToPoints9Display.GaussianRadius = 0.004999999999738004
tableToPoints9Display.SetScaleArray = ['POINTS', 'Field 0']
tableToPoints9Display.ScaleTransferFunction = 'PiecewiseFunction'
tableToPoints9Display.OpacityArray = ['POINTS', 'Field 0']
tableToPoints9Display.OpacityTransferFunction = 'PiecewiseFunction'
tableToPoints9Display.DataAxesGrid = 'GridAxesRepresentation'
tableToPoints9Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
tableToPoints9Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
tableToPoints9Display.ScaleTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
tableToPoints9Display.OpacityTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# show data from triangulate9
triangulate9Display = Show(triangulate9, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
triangulate9Display.Representation = 'Surface'
triangulate9Display.ColorArrayName = [None, '']
triangulate9Display.SelectTCoordArray = 'None'
triangulate9Display.SelectNormalArray = 'None'
triangulate9Display.SelectTangentArray = 'None'
triangulate9Display.OSPRayScaleArray = 'Field 0'
triangulate9Display.OSPRayScaleFunction = 'PiecewiseFunction'
triangulate9Display.SelectOrientationVectors = 'None'
triangulate9Display.ScaleFactor = 0.09999999999476007
triangulate9Display.SelectScaleArray = 'Field 0'
triangulate9Display.GlyphType = 'Arrow'
triangulate9Display.GlyphTableIndexArray = 'Field 0'
triangulate9Display.GaussianRadius = 0.004999999999738004
triangulate9Display.SetScaleArray = ['POINTS', 'Field 0']
triangulate9Display.ScaleTransferFunction = 'PiecewiseFunction'
triangulate9Display.OpacityArray = ['POINTS', 'Field 0']
triangulate9Display.OpacityTransferFunction = 'PiecewiseFunction'
triangulate9Display.DataAxesGrid = 'GridAxesRepresentation'
triangulate9Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
triangulate9Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
triangulate9Display.ScaleTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
triangulate9Display.OpacityTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# show data from pointDatatoCellData9
pointDatatoCellData9Display = Show(pointDatatoCellData9, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
pointDatatoCellData9Display.Representation = 'Surface'
pointDatatoCellData9Display.ColorArrayName = [None, '']
pointDatatoCellData9Display.SelectTCoordArray = 'None'
pointDatatoCellData9Display.SelectNormalArray = 'None'
pointDatatoCellData9Display.SelectTangentArray = 'None'
pointDatatoCellData9Display.OSPRayScaleArray = 'Field 0'
pointDatatoCellData9Display.OSPRayScaleFunction = 'PiecewiseFunction'
pointDatatoCellData9Display.SelectOrientationVectors = 'None'
pointDatatoCellData9Display.ScaleFactor = 0.09999999999476007
pointDatatoCellData9Display.SelectScaleArray = 'Field 0'
pointDatatoCellData9Display.GlyphType = 'Arrow'
pointDatatoCellData9Display.GlyphTableIndexArray = 'Field 0'
pointDatatoCellData9Display.GaussianRadius = 0.004999999999738004
pointDatatoCellData9Display.SetScaleArray = ['POINTS', 'Field 0']
pointDatatoCellData9Display.ScaleTransferFunction = 'PiecewiseFunction'
pointDatatoCellData9Display.OpacityArray = ['POINTS', 'Field 0']
pointDatatoCellData9Display.OpacityTransferFunction = 'PiecewiseFunction'
pointDatatoCellData9Display.DataAxesGrid = 'GridAxesRepresentation'
pointDatatoCellData9Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
pointDatatoCellData9Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
pointDatatoCellData9Display.ScaleTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
pointDatatoCellData9Display.OpacityTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# show data from pdf_cat7
pdf_cat7Display = Show(pdf_cat7, renderView1, 'GeometryRepresentation')

# get separate color transfer function/color map for 'Field0'
separate_pdf_cat7Display_Field0LUT = GetColorTransferFunction('Field0', pdf_cat7Display, separate=True)
separate_pdf_cat7Display_Field0LUT.RGBPoints = [5.23993e-11, 0.231373, 0.298039, 0.752941, 0.5000000000261996, 0.865003, 0.865003, 0.865003, 1.0, 0.705882, 0.0156863, 0.14902]
separate_pdf_cat7Display_Field0LUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
pdf_cat7Display.Representation = 'Surface'
pdf_cat7Display.ColorArrayName = ['CELLS', 'Field 0']
pdf_cat7Display.LookupTable = separate_pdf_cat7Display_Field0LUT
pdf_cat7Display.SelectTCoordArray = 'None'
pdf_cat7Display.SelectNormalArray = 'None'
pdf_cat7Display.SelectTangentArray = 'None'
pdf_cat7Display.OSPRayScaleFunction = 'PiecewiseFunction'
pdf_cat7Display.SelectOrientationVectors = 'None'
pdf_cat7Display.ScaleFactor = 10.0
pdf_cat7Display.SelectScaleArray = 'None'
pdf_cat7Display.GlyphType = 'Arrow'
pdf_cat7Display.GlyphTableIndexArray = 'None'
pdf_cat7Display.GaussianRadius = 0.5
pdf_cat7Display.SetScaleArray = [None, '']
pdf_cat7Display.ScaleTransferFunction = 'PiecewiseFunction'
pdf_cat7Display.OpacityArray = [None, '']
pdf_cat7Display.OpacityTransferFunction = 'PiecewiseFunction'
pdf_cat7Display.DataAxesGrid = 'GridAxesRepresentation'
pdf_cat7Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
pdf_cat7Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set separate color map
pdf_cat7Display.UseSeparateColorMap = True

# show data from tableToPoints10
tableToPoints10Display = Show(tableToPoints10, renderView1, 'GeometryRepresentation')

# get color transfer function/color map for 'phi'
phiLUT = GetColorTransferFunction('phi')
phiLUT.RGBPoints = [1.0, 0.231373, 0.298039, 0.752941, 4.5, 0.865003, 0.865003, 0.865003, 8.0, 0.705882, 0.0156863, 0.14902]
phiLUT.ScalarRangeInitialized = 1.0

# trace defaults for the display properties.
tableToPoints10Display.Representation = 'Points'
tableToPoints10Display.ColorArrayName = ['POINTS', 'phi']
tableToPoints10Display.LookupTable = phiLUT
tableToPoints10Display.PointSize = 8.0
tableToPoints10Display.SelectTCoordArray = 'None'
tableToPoints10Display.SelectNormalArray = 'None'
tableToPoints10Display.SelectTangentArray = 'None'
tableToPoints10Display.OSPRayScaleArray = 'X'
tableToPoints10Display.OSPRayScaleFunction = 'PiecewiseFunction'
tableToPoints10Display.SelectOrientationVectors = 'None'
tableToPoints10Display.ScaleFactor = 8.620000000000001
tableToPoints10Display.SelectScaleArray = 'None'
tableToPoints10Display.GlyphType = 'Arrow'
tableToPoints10Display.GlyphTableIndexArray = 'None'
tableToPoints10Display.GaussianRadius = 0.43100000000000005
tableToPoints10Display.SetScaleArray = ['POINTS', 'X']
tableToPoints10Display.ScaleTransferFunction = 'PiecewiseFunction'
tableToPoints10Display.OpacityArray = ['POINTS', 'X']
tableToPoints10Display.OpacityTransferFunction = 'PiecewiseFunction'
tableToPoints10Display.DataAxesGrid = 'GridAxesRepresentation'
tableToPoints10Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
tableToPoints10Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
tableToPoints10Display.ScaleTransferFunction.Points = [5.7, 0.0, 0.5, 0.0, 91.9, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
tableToPoints10Display.OpacityTransferFunction.Points = [5.7, 0.0, 0.5, 0.0, 91.9, 1.0, 0.5, 0.0]

# setup the color legend parameters for each legend in this view

# get color legend/bar for field0LUT in view renderView1
field0LUTColorBar = GetScalarBar(field0LUT, renderView1)
field0LUTColorBar.WindowLocation = 'Upper Left Corner'
field0LUTColorBar.Position = [0.0027472527472527475, 0.6543367346938775]
field0LUTColorBar.Title = 'Field 0'
field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
field0LUTColorBar.Visibility = 1

# get color legend/bar for separate_pdf_cat0Display_Field0LUT in view renderView1
separate_pdf_cat0Display_Field0LUTColorBar = GetScalarBar(separate_pdf_cat0Display_Field0LUT, renderView1)
separate_pdf_cat0Display_Field0LUTColorBar.WindowLocation = 'Upper Right Corner'
separate_pdf_cat0Display_Field0LUTColorBar.Position = [0.9203296703296703, 0.6543367346938775]
separate_pdf_cat0Display_Field0LUTColorBar.Title = 'Field 0'
separate_pdf_cat0Display_Field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
separate_pdf_cat0Display_Field0LUTColorBar.Visibility = 0

# get color legend/bar for separate_pdf_cat1Display_Field0LUT in view renderView1
separate_pdf_cat1Display_Field0LUTColorBar = GetScalarBar(separate_pdf_cat1Display_Field0LUT, renderView1)
separate_pdf_cat1Display_Field0LUTColorBar.WindowLocation = 'Lower Left Corner'
separate_pdf_cat1Display_Field0LUTColorBar.Title = 'Field 0'
separate_pdf_cat1Display_Field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
separate_pdf_cat1Display_Field0LUTColorBar.Visibility = 0

# get color legend/bar for separate_pdf_cat2Display_Field0LUT in view renderView1
separate_pdf_cat2Display_Field0LUTColorBar = GetScalarBar(separate_pdf_cat2Display_Field0LUT, renderView1)
separate_pdf_cat2Display_Field0LUTColorBar.Title = 'Field 0'
separate_pdf_cat2Display_Field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
separate_pdf_cat2Display_Field0LUTColorBar.Visibility = 0

# get color legend/bar for separate_pdf_cat3Display_Field0LUT in view renderView1
separate_pdf_cat3Display_Field0LUTColorBar = GetScalarBar(separate_pdf_cat3Display_Field0LUT, renderView1)
separate_pdf_cat3Display_Field0LUTColorBar.Title = 'Field 0'
separate_pdf_cat3Display_Field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
separate_pdf_cat3Display_Field0LUTColorBar.Visibility = 0

# get color legend/bar for separate_pdf_cat4Display_Field0LUT in view renderView1
separate_pdf_cat4Display_Field0LUTColorBar = GetScalarBar(separate_pdf_cat4Display_Field0LUT, renderView1)
separate_pdf_cat4Display_Field0LUTColorBar.Title = 'Field 0'
separate_pdf_cat4Display_Field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
separate_pdf_cat4Display_Field0LUTColorBar.Visibility = 0

# get color legend/bar for separate_pdf_cat5Display_Field0LUT in view renderView1
separate_pdf_cat5Display_Field0LUTColorBar = GetScalarBar(separate_pdf_cat5Display_Field0LUT, renderView1)
separate_pdf_cat5Display_Field0LUTColorBar.Title = 'Field 0'
separate_pdf_cat5Display_Field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
separate_pdf_cat5Display_Field0LUTColorBar.Visibility = 0

# get color legend/bar for separate_pdf_cat6Display_Field0LUT in view renderView1
separate_pdf_cat6Display_Field0LUTColorBar = GetScalarBar(separate_pdf_cat6Display_Field0LUT, renderView1)
separate_pdf_cat6Display_Field0LUTColorBar.Title = 'Field 0'
separate_pdf_cat6Display_Field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
separate_pdf_cat6Display_Field0LUTColorBar.Visibility = 0

# get color legend/bar for separate_pdf_cat7Display_Field0LUT in view renderView1
separate_pdf_cat7Display_Field0LUTColorBar = GetScalarBar(separate_pdf_cat7Display_Field0LUT, renderView1)
separate_pdf_cat7Display_Field0LUTColorBar.WindowLocation = 'Upper Right Corner'
separate_pdf_cat7Display_Field0LUTColorBar.Title = 'Field 0'
separate_pdf_cat7Display_Field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
separate_pdf_cat7Display_Field0LUTColorBar.Visibility = 0

# get color transfer function/color map for 'Field2'
field2LUT = GetColorTransferFunction('Field2')
field2LUT.RGBPoints = [-15.0, 0.231373, 0.298039, 0.752941, -7.55, 0.865003, 0.865003, 0.865003, -0.09999999999999964, 0.705882, 0.0156863, 0.14902]
field2LUT.ScalarRangeInitialized = 1.0

# get color legend/bar for field2LUT in view renderView1
field2LUTColorBar = GetScalarBar(field2LUT, renderView1)
field2LUTColorBar.WindowLocation = 'Upper Right Corner'
field2LUTColorBar.Title = 'Field 2'
field2LUTColorBar.ComponentTitle = ''

# set color bar visibility
field2LUTColorBar.Visibility = 0

# get color legend/bar for phiLUT in view renderView1
phiLUTColorBar = GetScalarBar(phiLUT, renderView1)
phiLUTColorBar.Title = 'phi'
phiLUTColorBar.ComponentTitle = ''

# set color bar visibility
phiLUTColorBar.Visibility = 1

# show color legend
bestDisplay.SetScalarBarVisibility(renderView1, True)

# hide data in view
Hide(pdf_cat0, renderView1)

# hide data in view
Hide(tableToPoints3, renderView1)

# hide data in view
Hide(triangulate3, renderView1)

# hide data in view
Hide(pointDatatoCellData3, renderView1)

# hide data in view
Hide(pdf_cat1, renderView1)

# hide data in view
Hide(tableToPoints4, renderView1)

# hide data in view
Hide(triangulate4, renderView1)

# hide data in view
Hide(pointDatatoCellData4, renderView1)

# hide data in view
Hide(pdf_cat2, renderView1)

# hide data in view
Hide(pdf_cat3, renderView1)

# hide data in view
Hide(pdf_cat4, renderView1)

# hide data in view
Hide(tableToPoints7, renderView1)

# hide data in view
Hide(triangulate7, renderView1)

# hide data in view
Hide(pointDatatoCellData7, renderView1)

# hide data in view
Hide(pdf_cat5, renderView1)

# hide data in view
Hide(pdf_cat6, renderView1)

# hide data in view
Hide(tableToPoints9, renderView1)

# hide data in view
Hide(triangulate9, renderView1)

# hide data in view
Hide(pointDatatoCellData9, renderView1)

# hide data in view
Hide(pdf_cat7, renderView1)

# show color legend
tableToPoints10Display.SetScalarBarVisibility(renderView1, True)

# ----------------------------------------------------------------
# setup color maps and opacity mapes used in the visualization
# note: the Get..() functions create a new object, if needed
# ----------------------------------------------------------------

# get separate opacity transfer function/opacity map for 'Field0'
separate_pdf_cat3Display_Field0PWF = GetOpacityTransferFunction('Field0', pdf_cat3Display, separate=True)
separate_pdf_cat3Display_Field0PWF.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]
separate_pdf_cat3Display_Field0PWF.ScalarRangeInitialized = 1

# get separate opacity transfer function/opacity map for 'Field0'
separate_pdf_cat0Display_Field0PWF = GetOpacityTransferFunction('Field0', pdf_cat0Display, separate=True)
separate_pdf_cat0Display_Field0PWF.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]
separate_pdf_cat0Display_Field0PWF.ScalarRangeInitialized = 1

# get opacity transfer function/opacity map for 'Field2'
field2PWF = GetOpacityTransferFunction('Field2')
field2PWF.Points = [-15.0, 0.0, 0.5, 0.0, -0.09999999999999964, 1.0, 0.5, 0.0]
field2PWF.ScalarRangeInitialized = 1

# get separate opacity transfer function/opacity map for 'Field0'
separate_pdf_cat4Display_Field0PWF = GetOpacityTransferFunction('Field0', pdf_cat4Display, separate=True)
separate_pdf_cat4Display_Field0PWF.Points = [6.51583e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]
separate_pdf_cat4Display_Field0PWF.ScalarRangeInitialized = 1

# get separate opacity transfer function/opacity map for 'Field0'
separate_pdf_cat6Display_Field0PWF = GetOpacityTransferFunction('Field0', pdf_cat6Display, separate=True)
separate_pdf_cat6Display_Field0PWF.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]
separate_pdf_cat6Display_Field0PWF.ScalarRangeInitialized = 1

# get separate opacity transfer function/opacity map for 'Field0'
separate_pdf_cat5Display_Field0PWF = GetOpacityTransferFunction('Field0', pdf_cat5Display, separate=True)
separate_pdf_cat5Display_Field0PWF.Points = [5.83035e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]
separate_pdf_cat5Display_Field0PWF.ScalarRangeInitialized = 1

# get separate opacity transfer function/opacity map for 'Field0'
separate_pdf_cat2Display_Field0PWF = GetOpacityTransferFunction('Field0', pdf_cat2Display, separate=True)
separate_pdf_cat2Display_Field0PWF.Points = [5.63492e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]
separate_pdf_cat2Display_Field0PWF.ScalarRangeInitialized = 1

# get separate opacity transfer function/opacity map for 'Field0'
separate_pdf_cat1Display_Field0PWF = GetOpacityTransferFunction('Field0', pdf_cat1Display, separate=True)
separate_pdf_cat1Display_Field0PWF.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]
separate_pdf_cat1Display_Field0PWF.ScalarRangeInitialized = 1

# get opacity transfer function/opacity map for 'Field0'
field0PWF = GetOpacityTransferFunction('Field0')
field0PWF.Points = [1.0, 0.0, 0.5, 0.0, 8.0, 1.0, 0.5, 0.0]
field0PWF.ScalarRangeInitialized = 1

# get opacity transfer function/opacity map for 'phi'
phiPWF = GetOpacityTransferFunction('phi')
phiPWF.Points = [1.0, 0.0, 0.5, 0.0, 8.0, 1.0, 0.5, 0.0]
phiPWF.ScalarRangeInitialized = 1

# get separate opacity transfer function/opacity map for 'Field0'
separate_pdf_cat7Display_Field0PWF = GetOpacityTransferFunction('Field0', pdf_cat7Display, separate=True)
separate_pdf_cat7Display_Field0PWF.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]
separate_pdf_cat7Display_Field0PWF.ScalarRangeInitialized = 1

# ----------------------------------------------------------------
# restore active source
SetActiveSource(best)
# ----------------------------------------------------------------


if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')