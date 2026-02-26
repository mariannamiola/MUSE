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
project_name='06_sec_2D_4000_tri_ind'

var_name='phi'
sub='sec'
vario='DIR2D'
geom_name='sec'

compute_sim=var_name+'_'+sub+'_'+vario+'_'+geom_name

project_folder=work_folder +'/'+project_name
geom_folder=project_folder+'/out/geometry/surf'
compute_folder=project_folder+'/out/compute'

sim_name=var_name+'_best'

csv_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+'/'+sim_name+'.csv'
obj_file=script_dir+'/../../'+geom_folder+'/'+geom_name+'_rot.obj'
samplescsv_file=script_dir+'/../data/samples.csv'

print(csv_file)
print(obj_file)

# create a new 'CSV Reader'
krigingcsv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file])
krigingcsv.HaveHeaders = 0
krigingcsv.FieldDelimiterCharacters = ''
krigingcsv.AddTabFieldDelimiter = 1

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

# create a new 'Table To Points'
tableToPoints1 = TableToPoints(registrationName='TableToPoints2', Input=krigingcsv)
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

# create a new 'Wavefront OBJ Reader'
sec_rotobj = WavefrontOBJReader(registrationName='sec_rot.obj', FileName=obj_file)

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[sec_rotobj, pointDatatoCellData1])

# ----------------------------------------------------------------
# restore active source
SetActiveSource(appendAttributes1)
# ----------------------------------------------------------------

# get active source.
appendAttributes1 = GetActiveSource()

# get active view
renderView1 = GetActiveViewOrCreate('RenderView')

appendAttributes1Display = Show(appendAttributes1, renderView1, 'GeometryRepresentation')

# get color transfer function/color map for 'Field0'
field0LUT = GetColorTransferFunction('Field0')
field0LUT.InterpretValuesAsCategories = 1
field0LUT.AnnotationsInitialized = 1
field0LUT.RGBPoints = [1.0, 0.231373, 0.298039, 0.752941, 4.5, 0.865003, 0.865003, 0.865003, 8.0, 0.705882, 0.0156863, 0.14902]
field0LUT.NumberOfTableValues = 8
field0LUT.ScalarRangeInitialized = 1.0
field0LUT.Annotations = ['1', '1', '2', '2', '3', '3', '4', '4', '5', '5', '6', '6', '7', '7', '8', '8']
field0LUT.ActiveAnnotatedValues = ['1', '2', '3', '4', '5', '6', '7', '8']
field0LUT.IndexedColors = [1.0, 1.0, 1.0, 0.9607843137254902, 0.9647058823529412, 0.34901960784313724, 0.796078431372549, 0.6745098039215687, 0.2784313725490196, 0.5490196078431373, 0.4117647058823529, 0.2823529411764706, 0.2549019607843137, 0.6705882352941176, 0.9490196078431372, 0.2627450980392157, 0.26666666666666666, 0.9294117647058824, 0.26666666666666666, 0.6745098039215687, 0.3058823529411765, 0.26666666666666666, 0.26666666666666666, 0.26666666666666666]
field0LUT.IndexedOpacities = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]

appendAttributes1Display.Representation = 'Surface'
appendAttributes1Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes1Display.LookupTable = field0LUT

# get color legend/bar for field0LUT in view renderView1
field0LUTColorBar = GetScalarBar(field0LUT, renderView1)
#field0LUTColorBar.WindowLocation = 'Any Location'
#field0LUTColorBar.Position = [0.0457979107996452, 0.2950017839303553]
field0LUTColorBar.Title = 'Classes'
field0LUTColorBar.ComponentTitle = '[-]'
field0LUTColorBar.HorizontalTitle = 1

appendAttributes1Display.SetScalarBarVisibility(renderView1, True)

# set scalar coloring
ColorBy(appendAttributes1Display, ('CELLS', 'Field 0'))

# reset view to fit data bounds
renderView1.ResetCamera()
renderView1.CameraPosition = [50.0, -219.4766694427885, 12.5]
renderView1.CameraFocalPoint = [50.0, 1.0, 12.5]
renderView1.CameraViewUp = [0.0, 0.0, 1.0]

# To save a specific target resolution, rather than using the
# the current view (or layout) size, and override the color palette.
directory = script_dir+'/../../'+project_folder+'/_fig'
if not os.path.exists(directory): 
	os.mkdir(directory)
	
SaveScreenshot(directory+'/'+sim_name+'.png', renderView1, FontScaling=True, OverrideColorPalette='WhiteBackground', TransparentBackground=False)



if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')
