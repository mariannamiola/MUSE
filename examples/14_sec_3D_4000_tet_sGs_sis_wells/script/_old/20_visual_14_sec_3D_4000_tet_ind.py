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
renderView1.ViewSize = [1456, 784]
renderView1.InteractionMode = '3D'
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

script_dir=os.path.dirname(os.path.realpath(__file__))

work_folder='MUSE_test'
project_name='14_sec_3D_4000_hex_sGs_sis'

var_name='phi'
sub=''
vario='DIR2D'
geom_name='SEC'

compute_sim=var_name+'_'+vario+'_'+geom_name

project_folder=work_folder +'/'+project_name
geom_folder=project_folder+'/out/geometry/volume'
compute_folder=project_folder+'/out/compute'

sim_name='sisim'
pdf_name='pdf_cat_'

csv_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+sim_name+'.csv'
pdf0_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+pdf_name+'0.csv'
pdf1_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+pdf_name+'1.csv'
pdf2_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+pdf_name+'2.csv'
pdf3_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+pdf_name+'3.csv'
pdf4_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+pdf_name+'4.csv'
pdf5_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+pdf_name+'5.csv'
pdf6_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+pdf_name+'6.csv'
pdf7_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+pdf_name+'7.csv'

vtk_file=script_dir+'/../../'+geom_folder+'/'+geom_name+'_rot.vtk'
samplescsv_file=script_dir+'/../data/samples.csv'

print(csv_file)
print(vtk_file)


# create a new 'Legacy VTK Reader'
gridvtk = LegacyVTKReader(registrationName=geom_name+'.vtk', FileNames=[vtk_file])

############################## BEST
# create a new 'CSV Reader'
sisimcsv = CSVReader(registrationName='sisim.csv', FileName=[csv_file])
sisimcsv.HaveHeaders = 0
sisimcsv.FieldDelimiterCharacters = ''
sisimcsv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints1 = TableToPoints(registrationName='TableToPoints1', Input=sisimcsv)
tableToPoints1.XColumn = 'Field 0'
tableToPoints1.YColumn = 'Field 0'
tableToPoints1.ZColumn = 'Field 0'
tableToPoints1.KeepAllDataArrays = 1

# create a new 'Tetrahedralize'
tetrahedralize1 = Tetrahedralize(registrationName='Tetrahedralize1', Input=tableToPoints1)

# create a new 'Point Data to Cell Data'
pointDatatoCellData1 = PointDatatoCellData(registrationName='PointDatatoCellData1', Input=tetrahedralize1)
pointDatatoCellData1.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData1.PassPointData = 1

# create a new 'Append Attributes'
best = AppendAttributes(registrationName='best', Input=[gridvtk, pointDatatoCellData1])


############################## PDF_CAT0
# create a new 'CSV Reader'
pdf_cat_0csv = CSVReader(registrationName='pdf_cat_0.csv', FileName=[pdf0_file])
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

# create a new 'Tetrahedralize'
tetrahedralize2 = Tetrahedralize(registrationName='Tetrahedralize2', Input=tableToPoints2)

# create a new 'Point Data to Cell Data'
pointDatatoCellData2 = PointDatatoCellData(registrationName='PointDatatoCellData2', Input=tetrahedralize2)
pointDatatoCellData2.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData2.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat0 = AppendAttributes(registrationName='pdf_cat0', Input=[gridvtk, pointDatatoCellData2])


############################################## PDF_CAT1
# create a new 'CSV Reader'
pdf_cat_1csv = CSVReader(registrationName='pdf_cat_1.csv', FileName=[pdf1_file])
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

# create a new 'Tetrahedralize'
tetrahedralize3 = Tetrahedralize(registrationName='Tetrahedralize3', Input=tableToPoints3)

# create a new 'Point Data to Cell Data'
pointDatatoCellData3 = PointDatatoCellData(registrationName='PointDatatoCellData3', Input=tetrahedralize3)
pointDatatoCellData3.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData3.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat1 = AppendAttributes(registrationName='pdf_cat1', Input=[gridvtk, pointDatatoCellData3])



############################################## PDF_CAT2
# create a new 'CSV Reader'
pdf_cat_2csv = CSVReader(registrationName='pdf_cat_2.csv', FileName=[pdf2_file])
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

# create a new 'Tetrahedralize'
tetrahedralize4 = Tetrahedralize(registrationName='Tetrahedralize4', Input=tableToPoints4)

# create a new 'Point Data to Cell Data'
pointDatatoCellData4 = PointDatatoCellData(registrationName='PointDatatoCellData4', Input=tetrahedralize4)
pointDatatoCellData4.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData4.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat2 = AppendAttributes(registrationName='pdf_cat2', Input=[gridvtk, pointDatatoCellData4])



############################################## PDF_CAT3
# create a new 'CSV Reader'
pdf_cat_3csv = CSVReader(registrationName='pdf_cat_3.csv', FileName=[pdf3_file])
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

# create a new 'Tetrahedralize'
tetrahedralize5 = Tetrahedralize(registrationName='Tetrahedralize5', Input=tableToPoints5)

# create a new 'Point Data to Cell Data'
pointDatatoCellData5 = PointDatatoCellData(registrationName='PointDatatoCellData5', Input=tetrahedralize5)
pointDatatoCellData5.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData5.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat3 = AppendAttributes(registrationName='pdf_cat3', Input=[gridvtk, pointDatatoCellData5])


############################################## PDF_CAT4
# create a new 'CSV Reader'
pdf_cat_4csv = CSVReader(registrationName='pdf_cat_4.csv', FileName=[pdf4_file])
pdf_cat_4csv.UseStringDelimiter = 0
pdf_cat_4csv.HaveHeaders = 0
pdf_cat_4csv.FieldDelimiterCharacters = ' '
pdf_cat_4csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints6 = TableToPoints(registrationName='TableToPoints6', Input=pdf_cat_4csv)
tableToPoints6.XColumn = 'Field 0'
tableToPoints6.YColumn = 'Field 0'
tableToPoints6.ZColumn = 'Field 0'
tableToPoints6.KeepAllDataArrays = 1

# create a new 'Tetrahedralize'
tetrahedralize6 = Tetrahedralize(registrationName='Tetrahedralize6', Input=tableToPoints6)

# create a new 'Point Data to Cell Data'
pointDatatoCellData6 = PointDatatoCellData(registrationName='PointDatatoCellData6', Input=tetrahedralize6)
pointDatatoCellData6.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData6.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat4 = AppendAttributes(registrationName='pdf_cat4', Input=[gridvtk, pointDatatoCellData6])


############################################## PDF_CAT5
# create a new 'CSV Reader'
pdf_cat_5csv = CSVReader(registrationName='pdf_cat_5.csv', FileName=[pdf5_file])
pdf_cat_5csv.UseStringDelimiter = 0
pdf_cat_5csv.HaveHeaders = 0
pdf_cat_5csv.FieldDelimiterCharacters = ' '
pdf_cat_5csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints7 = TableToPoints(registrationName='TableToPoints7', Input=pdf_cat_5csv)
tableToPoints7.XColumn = 'Field 0'
tableToPoints7.YColumn = 'Field 0'
tableToPoints7.ZColumn = 'Field 0'
tableToPoints7.KeepAllDataArrays = 1

# create a new 'Tetrahedralize'
tetrahedralize7 = Tetrahedralize(registrationName='Tetrahedralize7', Input=tableToPoints7)

# create a new 'Point Data to Cell Data'
pointDatatoCellData7 = PointDatatoCellData(registrationName='PointDatatoCellData7', Input=tetrahedralize7)
pointDatatoCellData7.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData7.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat5 = AppendAttributes(registrationName='pdf_cat5', Input=[gridvtk, pointDatatoCellData7])


############################################## PDF_CAT6
# create a new 'CSV Reader'
pdf_cat_6csv = CSVReader(registrationName='pdf_cat_6.csv', FileName=[pdf6_file])
pdf_cat_6csv.UseStringDelimiter = 0
pdf_cat_6csv.HaveHeaders = 0
pdf_cat_6csv.FieldDelimiterCharacters = ' '
pdf_cat_6csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints8 = TableToPoints(registrationName='TableToPoints8', Input=pdf_cat_6csv)
tableToPoints8.XColumn = 'Field 0'
tableToPoints8.YColumn = 'Field 0'
tableToPoints8.ZColumn = 'Field 0'
tableToPoints8.KeepAllDataArrays = 1

# create a new 'Tetrahedralize'
tetrahedralize8 = Tetrahedralize(registrationName='Tetrahedralize8', Input=tableToPoints8)

# create a new 'Point Data to Cell Data'
pointDatatoCellData8 = PointDatatoCellData(registrationName='PointDatatoCellData8', Input=tetrahedralize8)
pointDatatoCellData8.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData8.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat6 = AppendAttributes(registrationName='pdf_cat6', Input=[gridvtk, pointDatatoCellData8])

############################################## PDF_CAT7
# create a new 'CSV Reader'
pdf_cat_7csv = CSVReader(registrationName='pdf_cat_7.csv', FileName=[pdf7_file])
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

# create a new 'Tetrahedralize'
tetrahedralize9 = Tetrahedralize(registrationName='Tetrahedralize9', Input=tableToPoints9)

# create a new 'Point Data to Cell Data'
pointDatatoCellData9 = PointDatatoCellData(registrationName='PointDatatoCellData9', Input=tetrahedralize9)
pointDatatoCellData9.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData9.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat7 = AppendAttributes(registrationName='pdf_cat7', Input=[gridvtk, pointDatatoCellData9])


############################## SAMPLES
# create a new 'CSV Reader'
samplescsv = CSVReader(registrationName='samples.csv', FileName=[samplescsv_file])
samplescsv.UseStringDelimiter = 0
samplescsv.HaveHeaders = 1
samplescsv.FieldDelimiterCharacters = ';'

# create a new 'Table To Points'
tableToPoints10 = TableToPoints(registrationName='TableToPoints10', Input=samplescsv)
tableToPoints10.XColumn = 'X'
tableToPoints10.YColumn = 'Y'
tableToPoints10.ZColumn = 'Z'
tableToPoints10.KeepAllDataArrays = 1



# ----------------------------------------------------------------
# setup the visualization in view 'renderView1'
# ----------------------------------------------------------------

# show data from gridvtk
gridvtkDisplay = Show(gridvtk, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
gridvtkDisplay.Representation = 'Surface'
gridvtkDisplay.ColorArrayName = [None, '']
gridvtkDisplay.SelectTCoordArray = 'None'
gridvtkDisplay.SelectNormalArray = 'None'
gridvtkDisplay.SelectTangentArray = 'None'
gridvtkDisplay.OSPRayScaleFunction = 'PiecewiseFunction'
gridvtkDisplay.SelectOrientationVectors = 'None'
gridvtkDisplay.ScaleFactor = 10.0
gridvtkDisplay.SelectScaleArray = 'None'
gridvtkDisplay.GlyphType = 'Arrow'
gridvtkDisplay.GlyphTableIndexArray = 'None'
gridvtkDisplay.GaussianRadius = 0.5
gridvtkDisplay.SetScaleArray = [None, '']
gridvtkDisplay.ScaleTransferFunction = 'PiecewiseFunction'
gridvtkDisplay.OpacityArray = [None, '']
gridvtkDisplay.OpacityTransferFunction = 'PiecewiseFunction'
gridvtkDisplay.DataAxesGrid = 'GridAxesRepresentation'
gridvtkDisplay.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
gridvtkDisplay.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# show data from best
bestDisplay = Show(best, renderView1, 'GeometryRepresentation')

# get color transfer function/color map for 'Field0'
field0LUT = GetColorTransferFunction('Field0')
field0LUT.InterpretValuesAsCategories = 1
field0LUT.AnnotationsInitialized = 1
field0LUT.RGBPoints = [1.0, 0.231373, 0.298039, 0.752941, 4.5, 0.865003, 0.865003, 0.865003, 8.0, 0.705882, 0.0156863, 0.14902]
field0LUT.NumberOfTableValues = 8
field0LUT.ScalarRangeInitialized = 1.0
field0LUT.Annotations = ['1', '(1) S     ', '2', '(2) SiS   ', '3', '(3) G    ', '4', '(4) CS  ', '5', '(5) St    ', '6', '(6) CC ', '7', '(7) SaS ', '8', '(8) R     ']
field0LUT.ActiveAnnotatedValues = ['1', '2', '3', '4', '5', '6', '7', '8']
field0LUT.IndexedColors = [1.0, 1.0, 1.0, 0.9607843137254902, 0.9647058823529412, 0.34901960784313724, 0.796078431372549, 0.6745098039215687, 0.2784313725490196, 0.5490196078431373, 0.4117647058823529, 0.2823529411764706, 0.2549019607843137, 0.6705882352941176, 0.9490196078431372, 0.2627450980392157, 0.26666666666666666, 0.9294117647058824, 0.26666666666666666, 0.6745098039215687, 0.3058823529411765, 0.26666666666666666, 0.26666666666666666, 0.26666666666666666]
field0LUT.IndexedOpacities = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]


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

# show data from tetrahedralize3
tetrahedralize3Display = Show(tetrahedralize3, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
tetrahedralize3Display.Representation = 'Surface'
tetrahedralize3Display.ColorArrayName = [None, '']
tetrahedralize3Display.SelectTCoordArray = 'None'
tetrahedralize3Display.SelectNormalArray = 'None'
tetrahedralize3Display.SelectTangentArray = 'None'
tetrahedralize3Display.OSPRayScaleArray = 'Field 0'
tetrahedralize3Display.OSPRayScaleFunction = 'PiecewiseFunction'
tetrahedralize3Display.SelectOrientationVectors = 'None'
tetrahedralize3Display.ScaleFactor = 0.09999999999476007
tetrahedralize3Display.SelectScaleArray = 'Field 0'
tetrahedralize3Display.GlyphType = 'Arrow'
tetrahedralize3Display.GlyphTableIndexArray = 'Field 0'
tetrahedralize3Display.GaussianRadius = 0.004999999999738004
tetrahedralize3Display.SetScaleArray = ['POINTS', 'Field 0']
tetrahedralize3Display.ScaleTransferFunction = 'PiecewiseFunction'
tetrahedralize3Display.OpacityArray = ['POINTS', 'Field 0']
tetrahedralize3Display.OpacityTransferFunction = 'PiecewiseFunction'
tetrahedralize3Display.DataAxesGrid = 'GridAxesRepresentation'
tetrahedralize3Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
tetrahedralize3Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
tetrahedralize3Display.ScaleTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
tetrahedralize3Display.OpacityTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

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

# show data from tetrahedralize4
tetrahedralize4Display = Show(tetrahedralize4, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
tetrahedralize4Display.Representation = 'Surface'
tetrahedralize4Display.ColorArrayName = [None, '']
tetrahedralize4Display.SelectTCoordArray = 'None'
tetrahedralize4Display.SelectNormalArray = 'None'
tetrahedralize4Display.SelectTangentArray = 'None'
tetrahedralize4Display.OSPRayScaleArray = 'Field 0'
tetrahedralize4Display.OSPRayScaleFunction = 'PiecewiseFunction'
tetrahedralize4Display.SelectOrientationVectors = 'None'
tetrahedralize4Display.ScaleFactor = 0.0999999999943651
tetrahedralize4Display.SelectScaleArray = 'Field 0'
tetrahedralize4Display.GlyphType = 'Arrow'
tetrahedralize4Display.GlyphTableIndexArray = 'Field 0'
tetrahedralize4Display.GaussianRadius = 0.004999999999718254
tetrahedralize4Display.SetScaleArray = ['POINTS', 'Field 0']
tetrahedralize4Display.ScaleTransferFunction = 'PiecewiseFunction'
tetrahedralize4Display.OpacityArray = ['POINTS', 'Field 0']
tetrahedralize4Display.OpacityTransferFunction = 'PiecewiseFunction'
tetrahedralize4Display.DataAxesGrid = 'GridAxesRepresentation'
tetrahedralize4Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
tetrahedralize4Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
tetrahedralize4Display.ScaleTransferFunction.Points = [5.63492e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
tetrahedralize4Display.OpacityTransferFunction.Points = [5.63492e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

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

# show data from tetrahedralize7
tetrahedralize7Display = Show(tetrahedralize7, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
tetrahedralize7Display.Representation = 'Surface'
tetrahedralize7Display.ColorArrayName = [None, '']
tetrahedralize7Display.SelectTCoordArray = 'None'
tetrahedralize7Display.SelectNormalArray = 'None'
tetrahedralize7Display.SelectTangentArray = 'None'
tetrahedralize7Display.OSPRayScaleArray = 'Field 0'
tetrahedralize7Display.OSPRayScaleFunction = 'PiecewiseFunction'
tetrahedralize7Display.SelectOrientationVectors = 'None'
tetrahedralize7Display.ScaleFactor = 0.09999999999416966
tetrahedralize7Display.SelectScaleArray = 'Field 0'
tetrahedralize7Display.GlyphType = 'Arrow'
tetrahedralize7Display.GlyphTableIndexArray = 'Field 0'
tetrahedralize7Display.GaussianRadius = 0.004999999999708482
tetrahedralize7Display.SetScaleArray = ['POINTS', 'Field 0']
tetrahedralize7Display.ScaleTransferFunction = 'PiecewiseFunction'
tetrahedralize7Display.OpacityArray = ['POINTS', 'Field 0']
tetrahedralize7Display.OpacityTransferFunction = 'PiecewiseFunction'
tetrahedralize7Display.DataAxesGrid = 'GridAxesRepresentation'
tetrahedralize7Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
tetrahedralize7Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
tetrahedralize7Display.ScaleTransferFunction.Points = [5.83035e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
tetrahedralize7Display.OpacityTransferFunction.Points = [5.83035e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

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

# show data from tetrahedralize9
tetrahedralize9Display = Show(tetrahedralize9, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
tetrahedralize9Display.Representation = 'Surface'
tetrahedralize9Display.ColorArrayName = [None, '']
tetrahedralize9Display.SelectTCoordArray = 'None'
tetrahedralize9Display.SelectNormalArray = 'None'
tetrahedralize9Display.SelectTangentArray = 'None'
tetrahedralize9Display.OSPRayScaleArray = 'Field 0'
tetrahedralize9Display.OSPRayScaleFunction = 'PiecewiseFunction'
tetrahedralize9Display.SelectOrientationVectors = 'None'
tetrahedralize9Display.ScaleFactor = 0.09999999999476007
tetrahedralize9Display.SelectScaleArray = 'Field 0'
tetrahedralize9Display.GlyphType = 'Arrow'
tetrahedralize9Display.GlyphTableIndexArray = 'Field 0'
tetrahedralize9Display.GaussianRadius = 0.004999999999738004
tetrahedralize9Display.SetScaleArray = ['POINTS', 'Field 0']
tetrahedralize9Display.ScaleTransferFunction = 'PiecewiseFunction'
tetrahedralize9Display.OpacityArray = ['POINTS', 'Field 0']
tetrahedralize9Display.OpacityTransferFunction = 'PiecewiseFunction'
tetrahedralize9Display.DataAxesGrid = 'GridAxesRepresentation'
tetrahedralize9Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
tetrahedralize9Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
tetrahedralize9Display.ScaleTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
tetrahedralize9Display.OpacityTransferFunction.Points = [5.23993e-11, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0]

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
phiLUT.InterpretValuesAsCategories = 1
phiLUT.AnnotationsInitialized = 1
phiLUT.RGBPoints = [1.0, 0.231373, 0.298039, 0.752941, 4.5, 0.865003, 0.865003, 0.865003, 8.0, 0.705882, 0.0156863, 0.14902]
phiLUT.NumberOfTableValues = 8
phiLUT.ScalarRangeInitialized = 1.0
phiLUT.Annotations = ['1', '1', '2', '2', '3', '3', '4', '4', '5', '5', '6', '6', '7', '7', '8', '8']
phiLUT.ActiveAnnotatedValues = ['1', '2', '3', '4', '5', '6', '7', '8']
phiLUT.IndexedColors = [1.0, 1.0, 1.0, 0.9607843137254902, 0.9647058823529412, 0.34901960784313724, 0.796078431372549, 0.6745098039215687, 0.2784313725490196, 0.5490196078431373, 0.4117647058823529, 0.2823529411764706, 0.2549019607843137, 0.6705882352941176, 0.9490196078431372, 0.2627450980392157, 0.26666666666666666, 0.9294117647058824, 0.26666666666666666, 0.6745098039215687, 0.3058823529411765, 0.26666666666666666, 0.26666666666666666, 0.26666666666666666]
phiLUT.IndexedOpacities = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]

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
field0LUTColorBar.WindowLocation = 'Any Location'
field0LUTColorBar.Position = [0.047717062350796056, 0.45790816326530626]
field0LUTColorBar.Title = 'Classes'
field0LUTColorBar.ComponentTitle = '[-]'
field0LUTColorBar.HorizontalTitle = 1
field0LUTColorBar.ScalarBarLength = 0.27132653061224465

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
Hide(tetrahedralize3, renderView1)

# hide data in view
Hide(pointDatatoCellData3, renderView1)

# hide data in view
Hide(pdf_cat1, renderView1)

# hide data in view
Hide(tableToPoints4, renderView1)

# hide data in view
Hide(tetrahedralize4, renderView1)

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
Hide(tetrahedralize7, renderView1)

# hide data in view
Hide(pointDatatoCellData7, renderView1)

# hide data in view
Hide(pdf_cat5, renderView1)

# hide data in view
Hide(pdf_cat6, renderView1)

# hide data in view
Hide(tableToPoints9, renderView1)

# hide data in view
Hide(tetrahedralize9, renderView1)

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





# To save a specific target resolution, rather than using the
# the current view (or layout) size, and override the color palette.
directory = script_dir+'/../../'+project_folder+'/_fig'
if not os.path.exists(directory): 
	os.mkdir(directory)
	
SaveScreenshot(directory+'/'+sim_name+'.png', renderView1, FontScaling=True, OverrideColorPalette='WhiteBackground', TransparentBackground=False)



if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')
