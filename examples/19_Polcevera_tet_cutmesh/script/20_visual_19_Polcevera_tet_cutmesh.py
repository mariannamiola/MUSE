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
project_name='19_Polcevera_tet_cutmesh'

geom_input='piana' ##'subDEM'##'piana-piana_dz'
var_name='cat'
sub=geom_input+'-'+geom_input+'_dz'
vario='DIR3Dxy'
geom_name=geom_input+'-'+geom_input+'_dz'

compute_sim=var_name+'_'+sub+'_'+vario+'_geom_'+geom_name

project_folder=work_folder +'/'+project_name
geom_folder=project_folder+'/out/geometry/volume'

compute_folder=project_folder+'/out/compute'

sim_name='/_stats/cat_best'
pdf_name='pdf_cat_'

csv_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+sim_name+'.csv'

pdf0_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+pdf_name+'1.csv'
pdf1_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+pdf_name+'3.csv'
pdf2_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+pdf_name+'4.csv'
pdf3_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+pdf_name+'5.csv'
pdf4_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+pdf_name+'6.csv'

vtk_file=script_dir+'/../../'+geom_folder+'/'+geom_name+'.vtk'
samplescsv_file=script_dir+'/../data/dataset/samples.csv'

print(csv_file)
print(vtk_file)

# create a new 'CSV Reader'
samplescsv = CSVReader(registrationName='samples.csv', FileName=[samplescsv_file])
samplescsv.UseStringDelimiter = 0
samplescsv.HaveHeaders = 0
samplescsv.FieldDelimiterCharacters = ';'

# create a new 'Table To Points'
tableToPoints2 = TableToPoints(registrationName='TableToPoints2', Input=samplescsv)
tableToPoints2.XColumn = 'Field 0'
tableToPoints2.YColumn = 'Field 1'
tableToPoints2.ZColumn = 'Field 2'


############################## BEST
# create a new 'CSV Reader'
sisimcsv = CSVReader(registrationName='sisim.csv', FileName=[csv_file])
sisimcsv.HaveHeaders = 0
sisimcsv.FieldDelimiterCharacters = ''
sisimcsv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints1 = TableToPoints(registrationName='TableToPoints2', Input=sisimcsv)
tableToPoints1.XColumn = 'Field 0'
tableToPoints1.YColumn = 'Field 0'
tableToPoints1.ZColumn = 'Field 0'
tableToPoints1.KeepAllDataArrays = 1

# create a new 'Triangulate'
tetrahedralize1 = Tetrahedralize(registrationName='Tetrahedralize1', Input=tableToPoints1)

# create a new 'Point Data to Cell Data'
pointDatatoCellData1 = PointDatatoCellData(registrationName='PointDatatoCellData1', Input=tetrahedralize1)
pointDatatoCellData1.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData1.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
cubevtk = LegacyVTKReader(registrationName=geom_name+'.vtk', FileNames=[vtk_file])

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[cubevtk, pointDatatoCellData1])


############################## PDF_CAT0
# create a new 'CSV Reader'
pdf_cat_0csv = CSVReader(registrationName='pdf_cat_0.csv', FileName=[pdf0_file])
pdf_cat_0csv.UseStringDelimiter = 0
pdf_cat_0csv.HaveHeaders = 0
pdf_cat_0csv.FieldDelimiterCharacters = ' '
pdf_cat_0csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
top0 = TableToPoints(registrationName='top0', Input=pdf_cat_0csv)
top0.XColumn = 'Field 0'
top0.YColumn = 'Field 0'
top0.ZColumn = 'Field 0'
top0.KeepAllDataArrays = 1

# create a new 'Triangulate'
tet0 = Tetrahedralize(registrationName='tet0', Input=top0)

# create a new 'Point Data to Cell Data'
pdcd0 = PointDatatoCellData(registrationName='pdcd0', Input=tet0)
pdcd0.PointDataArraytoprocess = ['Field 0']
pdcd0.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat0 = AppendAttributes(registrationName='pdf_cat0', Input=[cubevtk, pdcd0])


############################## PDF_CAT1
# create a new 'CSV Reader'
pdf_cat_1csv = CSVReader(registrationName='pdf_cat_1.csv', FileName=[pdf1_file])
pdf_cat_1csv.UseStringDelimiter = 0
pdf_cat_1csv.HaveHeaders = 0
pdf_cat_1csv.FieldDelimiterCharacters = ' '
pdf_cat_1csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
top1 = TableToPoints(registrationName='top0', Input=pdf_cat_1csv)
top1.XColumn = 'Field 0'
top1.YColumn = 'Field 0'
top1.ZColumn = 'Field 0'
top1.KeepAllDataArrays = 1

# create a new 'Triangulate'
tet1 = Tetrahedralize(registrationName='tet1', Input=top1)

# create a new 'Point Data to Cell Data'
pdcd1 = PointDatatoCellData(registrationName='pdcd1', Input=tet1)
pdcd1.PointDataArraytoprocess = ['Field 0']
pdcd1.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat1 = AppendAttributes(registrationName='pdf_cat1', Input=[cubevtk, pdcd1])


############################## PDF_CAT2
# create a new 'CSV Reader'
pdf_cat_2csv = CSVReader(registrationName='pdf_cat_2.csv', FileName=[pdf2_file])
pdf_cat_2csv.UseStringDelimiter = 0
pdf_cat_2csv.HaveHeaders = 0
pdf_cat_2csv.FieldDelimiterCharacters = ' '
pdf_cat_2csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
top2 = TableToPoints(registrationName='top0', Input=pdf_cat_2csv)
top2.XColumn = 'Field 0'
top2.YColumn = 'Field 0'
top2.ZColumn = 'Field 0'
top2.KeepAllDataArrays = 1

# create a new 'Triangulate'
tet2 = Tetrahedralize(registrationName='tet2', Input=top2)

# create a new 'Point Data to Cell Data'
pdcd2 = PointDatatoCellData(registrationName='pdcd2', Input=tet2)
pdcd2.PointDataArraytoprocess = ['Field 0']
pdcd2.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat2 = AppendAttributes(registrationName='pdf_cat2', Input=[cubevtk, pdcd2])


############################## PDF_CAT3
# create a new 'CSV Reader'
pdf_cat_3csv = CSVReader(registrationName='pdf_cat_3.csv', FileName=[pdf3_file])
pdf_cat_3csv.UseStringDelimiter = 0
pdf_cat_3csv.HaveHeaders = 0
pdf_cat_3csv.FieldDelimiterCharacters = ' '
pdf_cat_3csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
top3 = TableToPoints(registrationName='top0', Input=pdf_cat_3csv)
top3.XColumn = 'Field 0'
top3.YColumn = 'Field 0'
top3.ZColumn = 'Field 0'
top3.KeepAllDataArrays = 1

# create a new 'Triangulate'
tet3 = Tetrahedralize(registrationName='tet3', Input=top3)

# create a new 'Point Data to Cell Data'
pdcd3 = PointDatatoCellData(registrationName='pdcd3', Input=tet3)
pdcd3.PointDataArraytoprocess = ['Field 0']
pdcd3.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat3 = AppendAttributes(registrationName='pdf_cat3', Input=[cubevtk, pdcd3])


############################## PDF_CAT4
# create a new 'CSV Reader'
pdf_cat_4csv = CSVReader(registrationName='pdf_cat_4.csv', FileName=[pdf4_file])
pdf_cat_4csv.UseStringDelimiter = 0
pdf_cat_4csv.HaveHeaders = 0
pdf_cat_4csv.FieldDelimiterCharacters = ' '
pdf_cat_4csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
top4 = TableToPoints(registrationName='top0', Input=pdf_cat_4csv)
top4.XColumn = 'Field 0'
top4.YColumn = 'Field 0'
top4.ZColumn = 'Field 0'
top4.KeepAllDataArrays = 1

# create a new 'Triangulate'
tet4 = Tetrahedralize(registrationName='tet4', Input=top4)

# create a new 'Point Data to Cell Data'
pdcd4 = PointDatatoCellData(registrationName='pdcd4', Input=tet4)
pdcd4.PointDataArraytoprocess = ['Field 0']
pdcd4.PassPointData = 1

# create a new 'Append Attributes'
pdf_cat4 = AppendAttributes(registrationName='pdf_cat4', Input=[cubevtk, pdcd4])




# ----------------------------------------------------------------
# restore active source
SetActiveSource(appendAttributes1)
# ----------------------------------------------------------------

# get active source.
appendAttributes1 = GetActiveSource()

# get active view
renderView1 = GetActiveViewOrCreate('RenderView')

appendAttributes1Display = Show(appendAttributes1, renderView1, 'GeometryRepresentation')

# get 2D transfer function for 'Field0'
field0TF2D = GetTransferFunction2D('Field0')

# get color transfer function/color map for 'Field0'
field0LUT = GetColorTransferFunction('Field0')
field0LUT.InterpretValuesAsCategories = 1
field0LUT.AnnotationsInitialized = 1
field0LUT.TransferFunction2D = field0TF2D
field0LUT.RGBPoints = [1.0, 0.231373, 0.298039, 0.752941, 4.5, 0.865003, 0.865003, 0.865003, 8.0, 0.705882, 0.0156863, 0.14902]
field0LUT.NumberOfTableValues = 8
field0LUT.ScalarRangeInitialized = 1.0
field0LUT.Annotations = ['1', 'Argille', '2', 'Argille litoidi', '3', 'Marne', '4', 'Riporto', '5', 'Roccia', '6', 'Sedimento']
field0LUT.ActiveAnnotatedValues = ['1', '3', '4', '5', '6']
field0LUT.IndexedColors = [1.0, 1.0, 1.0, 0.9607843137254902, 0.9647058823529412, 0.34901960784313724, 0.796078431372549, 0.6745098039215687, 0.2784313725490196, 0.5490196078431373, 0.4117647058823529, 0.2823529411764706, 0.2549019607843137, 0.6705882352941176, 0.9490196078431372, 0.2627450980392157, 0.26666666666666666, 0.9294117647058824]
field0LUT.IndexedOpacities = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0]

# trace defaults for the display properties.
appendAttributes1Display.Representation = 'Surface'
appendAttributes1Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes1Display.LookupTable = field0LUT
appendAttributes1Display.SelectTCoordArray = 'None'
appendAttributes1Display.SelectNormalArray = 'None'
appendAttributes1Display.SelectTangentArray = 'None'
appendAttributes1Display.OSPRayScaleFunction = 'Piecewise Function'
appendAttributes1Display.Assembly = ''
appendAttributes1Display.SelectOrientationVectors = 'None'
appendAttributes1Display.ScaleFactor = 1174.6229999999516
appendAttributes1Display.SelectScaleArray = 'None'
appendAttributes1Display.GlyphType = 'Arrow'
appendAttributes1Display.GlyphTableIndexArray = 'None'
appendAttributes1Display.GaussianRadius = 58.73114999999758
appendAttributes1Display.SetScaleArray = [None, '']
appendAttributes1Display.ScaleTransferFunction = 'Piecewise Function'
appendAttributes1Display.OpacityArray = [None, '']
appendAttributes1Display.OpacityTransferFunction = 'Piecewise Function'
appendAttributes1Display.DataAxesGrid = 'Grid Axes Representation'
appendAttributes1Display.PolarAxes = 'Polar Axes Representation'
appendAttributes1Display.SelectInputVectors = [None, '']
appendAttributes1Display.WriteLog = ''

# init the 'Piecewise Function' selected for 'OSPRayScaleFunction'
appendAttributes1Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# show data from pdf_cat0
pdf_cat0Display = Show(pdf_cat0, renderView1, 'UnstructuredGridRepresentation')

# get separate 2D transfer function for 'Field0'
separate_pdf_cat0Display_Field0TF2D = GetTransferFunction2D('Field0', pdf_cat0Display, separate=True)
separate_pdf_cat0Display_Field0TF2D.ScalarRangeInitialized = 1

# get separate color transfer function/color map for 'Field0'
separate_pdf_cat0Display_Field0LUT = GetColorTransferFunction('Field0', pdf_cat0Display, separate=True)
separate_pdf_cat0Display_Field0LUT.TransferFunction2D = separate_pdf_cat0Display_Field0TF2D
separate_pdf_cat0Display_Field0LUT.ScalarRangeInitialized = 1.0

# get separate opacity transfer function/opacity map for 'Field0'
separate_pdf_cat0Display_Field0PWF = GetOpacityTransferFunction('Field0', pdf_cat0Display, separate=True)
separate_pdf_cat0Display_Field0PWF.ScalarRangeInitialized = 1

# trace defaults for the display properties.
pdf_cat0Display.Representation = 'Surface'
pdf_cat0Display.ColorArrayName = ['CELLS', 'Field 0']
pdf_cat0Display.LookupTable = separate_pdf_cat0Display_Field0LUT
pdf_cat0Display.SelectTCoordArray = 'None'
pdf_cat0Display.SelectNormalArray = 'None'
pdf_cat0Display.SelectTangentArray = 'None'
pdf_cat0Display.OSPRayScaleFunction = 'Piecewise Function'
pdf_cat0Display.Assembly = ''
pdf_cat0Display.SelectOrientationVectors = 'None'
pdf_cat0Display.ScaleFactor = 1174.6229999999516
pdf_cat0Display.SelectScaleArray = 'None'
pdf_cat0Display.GlyphType = 'Arrow'
pdf_cat0Display.GlyphTableIndexArray = 'None'
pdf_cat0Display.GaussianRadius = 58.73114999999758
pdf_cat0Display.SetScaleArray = [None, '']
pdf_cat0Display.ScaleTransferFunction = 'Piecewise Function'
pdf_cat0Display.OpacityArray = [None, '']
pdf_cat0Display.OpacityTransferFunction = 'Piecewise Function'
pdf_cat0Display.DataAxesGrid = 'Grid Axes Representation'
pdf_cat0Display.PolarAxes = 'Polar Axes Representation'
pdf_cat0Display.ScalarOpacityFunction = separate_pdf_cat0Display_Field0PWF
pdf_cat0Display.ScalarOpacityUnitDistance = 300.23406504500673
pdf_cat0Display.OpacityArrayName = ['CELLS', 'Field 0']
pdf_cat0Display.SelectInputVectors = [None, '']
pdf_cat0Display.WriteLog = ''

# init the 'Piecewise Function' selected for 'OSPRayScaleFunction'
pdf_cat0Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set separate color map
pdf_cat0Display.UseSeparateColorMap = True

# show data from pdf_cat1
pdf_cat1Display = Show(pdf_cat1, renderView1, 'UnstructuredGridRepresentation')

# get separate 2D transfer function for 'Field0'
separate_pdf_cat1Display_Field0TF2D = GetTransferFunction2D('Field0', pdf_cat1Display, separate=True)
separate_pdf_cat1Display_Field0TF2D.ScalarRangeInitialized = 1

# get separate color transfer function/color map for 'Field0'
separate_pdf_cat1Display_Field0LUT = GetColorTransferFunction('Field0', pdf_cat1Display, separate=True)
separate_pdf_cat1Display_Field0LUT.TransferFunction2D = separate_pdf_cat1Display_Field0TF2D
separate_pdf_cat1Display_Field0LUT.ScalarRangeInitialized = 1.0

# get separate opacity transfer function/opacity map for 'Field0'
separate_pdf_cat1Display_Field0PWF = GetOpacityTransferFunction('Field0', pdf_cat1Display, separate=True)
separate_pdf_cat1Display_Field0PWF.ScalarRangeInitialized = 1

# trace defaults for the display properties.
pdf_cat1Display.Representation = 'Surface'
pdf_cat1Display.ColorArrayName = ['CELLS', 'Field 0']
pdf_cat1Display.LookupTable = separate_pdf_cat1Display_Field0LUT
pdf_cat1Display.SelectTCoordArray = 'None'
pdf_cat1Display.SelectNormalArray = 'None'
pdf_cat1Display.SelectTangentArray = 'None'
pdf_cat1Display.OSPRayScaleFunction = 'Piecewise Function'
pdf_cat1Display.Assembly = ''
pdf_cat1Display.SelectOrientationVectors = 'None'
pdf_cat1Display.ScaleFactor = 1174.6229999999516
pdf_cat1Display.SelectScaleArray = 'None'
pdf_cat1Display.GlyphType = 'Arrow'
pdf_cat1Display.GlyphTableIndexArray = 'None'
pdf_cat1Display.GaussianRadius = 58.73114999999758
pdf_cat1Display.SetScaleArray = [None, '']
pdf_cat1Display.ScaleTransferFunction = 'Piecewise Function'
pdf_cat1Display.OpacityArray = [None, '']
pdf_cat1Display.OpacityTransferFunction = 'Piecewise Function'
pdf_cat1Display.DataAxesGrid = 'Grid Axes Representation'
pdf_cat1Display.PolarAxes = 'Polar Axes Representation'
pdf_cat1Display.ScalarOpacityFunction = separate_pdf_cat1Display_Field0PWF
pdf_cat1Display.ScalarOpacityUnitDistance = 300.23406504500673
pdf_cat1Display.OpacityArrayName = ['CELLS', 'Field 0']
pdf_cat1Display.SelectInputVectors = [None, '']
pdf_cat1Display.WriteLog = ''

# init the 'Piecewise Function' selected for 'OSPRayScaleFunction'
pdf_cat1Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set separate color map
pdf_cat1Display.UseSeparateColorMap = True

# show data from pdf_cat2
pdf_cat2Display = Show(pdf_cat2, renderView1, 'UnstructuredGridRepresentation')

# get separate 2D transfer function for 'Field0'
separate_pdf_cat2Display_Field0TF2D = GetTransferFunction2D('Field0', pdf_cat2Display, separate=True)
separate_pdf_cat2Display_Field0TF2D.ScalarRangeInitialized = 1

# get separate color transfer function/color map for 'Field0'
separate_pdf_cat2Display_Field0LUT = GetColorTransferFunction('Field0', pdf_cat2Display, separate=True)
separate_pdf_cat2Display_Field0LUT.TransferFunction2D = separate_pdf_cat2Display_Field0TF2D
separate_pdf_cat2Display_Field0LUT.ScalarRangeInitialized = 1.0

# get separate opacity transfer function/opacity map for 'Field0'
separate_pdf_cat2Display_Field0PWF = GetOpacityTransferFunction('Field0', pdf_cat2Display, separate=True)
separate_pdf_cat2Display_Field0PWF.ScalarRangeInitialized = 1

# trace defaults for the display properties.
pdf_cat2Display.Representation = 'Surface'
pdf_cat2Display.ColorArrayName = ['CELLS', 'Field 0']
pdf_cat2Display.LookupTable = separate_pdf_cat2Display_Field0LUT
pdf_cat2Display.SelectTCoordArray = 'None'
pdf_cat2Display.SelectNormalArray = 'None'
pdf_cat2Display.SelectTangentArray = 'None'
pdf_cat2Display.OSPRayScaleFunction = 'Piecewise Function'
pdf_cat2Display.Assembly = ''
pdf_cat2Display.SelectOrientationVectors = 'None'
pdf_cat2Display.ScaleFactor = 1174.6229999999516
pdf_cat2Display.SelectScaleArray = 'None'
pdf_cat2Display.GlyphType = 'Arrow'
pdf_cat2Display.GlyphTableIndexArray = 'None'
pdf_cat2Display.GaussianRadius = 58.73114999999758
pdf_cat2Display.SetScaleArray = [None, '']
pdf_cat2Display.ScaleTransferFunction = 'Piecewise Function'
pdf_cat2Display.OpacityArray = [None, '']
pdf_cat2Display.OpacityTransferFunction = 'Piecewise Function'
pdf_cat2Display.DataAxesGrid = 'Grid Axes Representation'
pdf_cat2Display.PolarAxes = 'Polar Axes Representation'
pdf_cat2Display.ScalarOpacityFunction = separate_pdf_cat2Display_Field0PWF
pdf_cat2Display.ScalarOpacityUnitDistance = 300.23406504500673
pdf_cat2Display.OpacityArrayName = ['CELLS', 'Field 0']
pdf_cat2Display.SelectInputVectors = [None, '']
pdf_cat2Display.WriteLog = ''

# init the 'Piecewise Function' selected for 'OSPRayScaleFunction'
pdf_cat2Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set separate color map
pdf_cat2Display.UseSeparateColorMap = True

# show data from pdf_cat3
pdf_cat3Display = Show(pdf_cat3, renderView1, 'UnstructuredGridRepresentation')

# get separate 2D transfer function for 'Field0'
separate_pdf_cat3Display_Field0TF2D = GetTransferFunction2D('Field0', pdf_cat3Display, separate=True)
separate_pdf_cat3Display_Field0TF2D.ScalarRangeInitialized = 1

# get separate color transfer function/color map for 'Field0'
separate_pdf_cat3Display_Field0LUT = GetColorTransferFunction('Field0', pdf_cat3Display, separate=True)
separate_pdf_cat3Display_Field0LUT.TransferFunction2D = separate_pdf_cat3Display_Field0TF2D
separate_pdf_cat3Display_Field0LUT.ScalarRangeInitialized = 1.0

# get separate opacity transfer function/opacity map for 'Field0'
separate_pdf_cat3Display_Field0PWF = GetOpacityTransferFunction('Field0', pdf_cat3Display, separate=True)
separate_pdf_cat3Display_Field0PWF.ScalarRangeInitialized = 1

# trace defaults for the display properties.
pdf_cat3Display.Representation = 'Surface'
pdf_cat3Display.ColorArrayName = ['CELLS', 'Field 0']
pdf_cat3Display.LookupTable = separate_pdf_cat3Display_Field0LUT
pdf_cat3Display.SelectTCoordArray = 'None'
pdf_cat3Display.SelectNormalArray = 'None'
pdf_cat3Display.SelectTangentArray = 'None'
pdf_cat3Display.OSPRayScaleFunction = 'Piecewise Function'
pdf_cat3Display.Assembly = ''
pdf_cat3Display.SelectOrientationVectors = 'None'
pdf_cat3Display.ScaleFactor = 1174.6229999999516
pdf_cat3Display.SelectScaleArray = 'None'
pdf_cat3Display.GlyphType = 'Arrow'
pdf_cat3Display.GlyphTableIndexArray = 'None'
pdf_cat3Display.GaussianRadius = 58.73114999999758
pdf_cat3Display.SetScaleArray = [None, '']
pdf_cat3Display.ScaleTransferFunction = 'Piecewise Function'
pdf_cat3Display.OpacityArray = [None, '']
pdf_cat3Display.OpacityTransferFunction = 'Piecewise Function'
pdf_cat3Display.DataAxesGrid = 'Grid Axes Representation'
pdf_cat3Display.PolarAxes = 'Polar Axes Representation'
pdf_cat3Display.ScalarOpacityFunction = separate_pdf_cat3Display_Field0PWF
pdf_cat3Display.ScalarOpacityUnitDistance = 300.23406504500673
pdf_cat3Display.OpacityArrayName = ['CELLS', 'Field 0']
pdf_cat3Display.SelectInputVectors = [None, '']
pdf_cat3Display.WriteLog = ''

# init the 'Piecewise Function' selected for 'OSPRayScaleFunction'
pdf_cat3Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set separate color map
pdf_cat3Display.UseSeparateColorMap = True

# show data from pdf_cat4
pdf_cat4Display = Show(pdf_cat4, renderView1, 'UnstructuredGridRepresentation')

# get separate 2D transfer function for 'Field0'
separate_pdf_cat4Display_Field0TF2D = GetTransferFunction2D('Field0', pdf_cat4Display, separate=True)
separate_pdf_cat4Display_Field0TF2D.ScalarRangeInitialized = 1

# get separate color transfer function/color map for 'Field0'
separate_pdf_cat4Display_Field0LUT = GetColorTransferFunction('Field0', pdf_cat4Display, separate=True)
separate_pdf_cat4Display_Field0LUT.TransferFunction2D = separate_pdf_cat4Display_Field0TF2D
separate_pdf_cat4Display_Field0LUT.RGBPoints = [0.0, 0.231373, 0.298039, 0.752941, 0.49999999999999994, 0.865003, 0.865003, 0.865003, 1.0, 0.705882, 0.0156863, 0.14902]
separate_pdf_cat4Display_Field0LUT.ScalarRangeInitialized = 1.0

# get separate opacity transfer function/opacity map for 'Field0'
separate_pdf_cat4Display_Field0PWF = GetOpacityTransferFunction('Field0', pdf_cat4Display, separate=True)
separate_pdf_cat4Display_Field0PWF.ScalarRangeInitialized = 1

# trace defaults for the display properties.
pdf_cat4Display.Representation = 'Surface'
pdf_cat4Display.ColorArrayName = ['CELLS', 'Field 0']
pdf_cat4Display.LookupTable = separate_pdf_cat4Display_Field0LUT
pdf_cat4Display.SelectTCoordArray = 'None'
pdf_cat4Display.SelectNormalArray = 'None'
pdf_cat4Display.SelectTangentArray = 'None'
pdf_cat4Display.OSPRayScaleFunction = 'Piecewise Function'
pdf_cat4Display.Assembly = ''
pdf_cat4Display.SelectOrientationVectors = 'None'
pdf_cat4Display.ScaleFactor = 1174.6229999999516
pdf_cat4Display.SelectScaleArray = 'None'
pdf_cat4Display.GlyphType = 'Arrow'
pdf_cat4Display.GlyphTableIndexArray = 'None'
pdf_cat4Display.GaussianRadius = 58.73114999999758
pdf_cat4Display.SetScaleArray = [None, '']
pdf_cat4Display.ScaleTransferFunction = 'Piecewise Function'
pdf_cat4Display.OpacityArray = [None, '']
pdf_cat4Display.OpacityTransferFunction = 'Piecewise Function'
pdf_cat4Display.DataAxesGrid = 'Grid Axes Representation'
pdf_cat4Display.PolarAxes = 'Polar Axes Representation'
pdf_cat4Display.ScalarOpacityFunction = separate_pdf_cat4Display_Field0PWF
pdf_cat4Display.ScalarOpacityUnitDistance = 300.23406504500673
pdf_cat4Display.OpacityArrayName = ['CELLS', 'Field 0']
pdf_cat4Display.SelectInputVectors = [None, '']
pdf_cat4Display.WriteLog = ''

# init the 'Piecewise Function' selected for 'OSPRayScaleFunction'
pdf_cat4Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set separate color map
pdf_cat4Display.UseSeparateColorMap = True

# setup the color legend parameters for each legend in this view

# get color legend/bar for field0LUT in view renderView1
field0LUTColorBar = GetScalarBar(field0LUT, renderView1)
field0LUTColorBar.WindowLocation = 'Any Location'
field0LUTColorBar.Position = [0.8457086354647326, 0.5645571245186137]
field0LUTColorBar.Title = 'Classes'
field0LUTColorBar.ComponentTitle = '[-]'
field0LUTColorBar.HorizontalTitle = 1
field0LUTColorBar.ScalarBarLength = 0.32999999999999985

# set color bar visibility
field0LUTColorBar.Visibility = 1

# get color legend/bar for separate_pdf_cat0Display_Field0LUT in view renderView1
separate_pdf_cat0Display_Field0LUTColorBar = GetScalarBar(separate_pdf_cat0Display_Field0LUT, renderView1)
separate_pdf_cat0Display_Field0LUTColorBar.WindowLocation = 'Any Location'
separate_pdf_cat0Display_Field0LUTColorBar.Position = [0.8470665787738958, 0.1437740693196406]
separate_pdf_cat0Display_Field0LUTColorBar.Title = 'Field 0'
separate_pdf_cat0Display_Field0LUTColorBar.ComponentTitle = ''
separate_pdf_cat0Display_Field0LUTColorBar.ScalarBarLength = 0.33000000000000007

# set color bar visibility
separate_pdf_cat0Display_Field0LUTColorBar.Visibility = 0

# get color legend/bar for separate_pdf_cat1Display_Field0LUT in view renderView1
separate_pdf_cat1Display_Field0LUTColorBar = GetScalarBar(separate_pdf_cat1Display_Field0LUT, renderView1)
separate_pdf_cat1Display_Field0LUTColorBar.WindowLocation = 'Upper Right Corner'
separate_pdf_cat1Display_Field0LUTColorBar.Title = 'Field 0'
separate_pdf_cat1Display_Field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
separate_pdf_cat1Display_Field0LUTColorBar.Visibility = 0

# get color legend/bar for separate_pdf_cat2Display_Field0LUT in view renderView1
separate_pdf_cat2Display_Field0LUTColorBar = GetScalarBar(separate_pdf_cat2Display_Field0LUT, renderView1)
separate_pdf_cat2Display_Field0LUTColorBar.WindowLocation = 'Any Location'
separate_pdf_cat2Display_Field0LUTColorBar.Position = [0.03230059327620303, 0.6033376123234917]
separate_pdf_cat2Display_Field0LUTColorBar.Title = 'Field 0'
separate_pdf_cat2Display_Field0LUTColorBar.ComponentTitle = ''
separate_pdf_cat2Display_Field0LUTColorBar.ScalarBarLength = 0.33000000000000007

# set color bar visibility
separate_pdf_cat2Display_Field0LUTColorBar.Visibility = 0

# get color legend/bar for separate_pdf_cat3Display_Field0LUT in view renderView1
separate_pdf_cat3Display_Field0LUTColorBar = GetScalarBar(separate_pdf_cat3Display_Field0LUT, renderView1)
separate_pdf_cat3Display_Field0LUTColorBar.WindowLocation = 'Lower Left Corner'
separate_pdf_cat3Display_Field0LUTColorBar.Title = 'Field 0'
separate_pdf_cat3Display_Field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
separate_pdf_cat3Display_Field0LUTColorBar.Visibility = 0

# get color legend/bar for separate_pdf_cat4Display_Field0LUT in view renderView1
separate_pdf_cat4Display_Field0LUTColorBar = GetScalarBar(separate_pdf_cat4Display_Field0LUT, renderView1)
separate_pdf_cat4Display_Field0LUTColorBar.WindowLocation = 'Any Location'
separate_pdf_cat4Display_Field0LUTColorBar.Position = [0.7435728411338166, 0.03851091142490376]
separate_pdf_cat4Display_Field0LUTColorBar.Title = 'Field 0'
separate_pdf_cat4Display_Field0LUTColorBar.ComponentTitle = ''
separate_pdf_cat4Display_Field0LUTColorBar.ScalarBarLength = 0.32999999999999996

# set color bar visibility
separate_pdf_cat4Display_Field0LUTColorBar.Visibility = 0

# show color legend
appendAttributes1Display.SetScalarBarVisibility(renderView1, True)

# hide data in view
Hide(pdf_cat0, renderView1)

# hide data in view
Hide(pdf_cat1, renderView1)

# hide data in view
Hide(pdf_cat2, renderView1)

# hide data in view
Hide(pdf_cat3, renderView1)

# hide data in view
Hide(pdf_cat4, renderView1)

# reset view to fit data bounds
renderView1.ResetCamera()
#renderView1.CameraPosition = [50.0, -219.4766694427885, 12.5]
#renderView1.CameraFocalPoint = [50.0, 1.0, 12.5]
#renderView1.CameraViewUp = [0.0, 0.0, 1.0]




if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')
