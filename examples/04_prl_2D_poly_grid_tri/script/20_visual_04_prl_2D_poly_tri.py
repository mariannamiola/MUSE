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
project_name='04_prl_2D_poly_grid_tri'

var_name='cr'
sub=''
vario=''
geom_name='FSASSELLO_tri_FARENZANO_tri'

project_folder=work_folder +'/'+project_name
geom_folder=project_folder+'/out/geometry/surf'
compute_folder=project_folder+'/out/compute'

space='/_varspace/_stats'
stats_name='mean'

csv_file=script_dir+'/../../'+compute_folder+'/'+var_name+'_merge_tri.csv'
obj_file=script_dir+'/../../'+geom_folder+'/'+geom_name+'.obj'
samplescsv_file=script_dir+'/../data/samples.csv'

shape0_file=script_dir+'/../data/FSASSELLO/FSASSELLO.shp'
shape1_file=script_dir+'/../data/FARENZANO/FARENZANO.shp'

print(csv_file)
print(obj_file)

# create a new 'CSV Reader'
cr_merge_gridcsv = CSVReader(registrationName=var_name+'_merge_tri.csv', FileName=[csv_file])
cr_merge_gridcsv.UseStringDelimiter = 0
cr_merge_gridcsv.HaveHeaders = 0
cr_merge_gridcsv.FieldDelimiterCharacters = ''
cr_merge_gridcsv.AddTabFieldDelimiter = 1

# create a new 'GDAL Vector Reader'
fSASSELLOshp = GDALVectorReader(registrationName='FSASSELLO.shp', FileName=[shape0_file])

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
tableToPoints2 = TableToPoints(registrationName='TableToPoints2', Input=cr_merge_gridcsv)
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

# create a new 'GDAL Vector Reader'
fARENZANOshp = GDALVectorReader(registrationName='FARENZANO.shp', FileName=[shape1_file])

# create a new 'Wavefront OBJ Reader'
FSASSELLO_grid_FARENZANO_gridobj = WavefrontOBJReader(registrationName=geom_name+'.obj', FileName=obj_file)

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[FSASSELLO_grid_FARENZANO_gridobj, pointDatatoCellData1])

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
field0LUT.AutomaticRescaleRangeMode = 'Never'
field0LUT.RGBPoints = [500, 1.0, 0.8862745098039215, 0.5333333333333333, 1000, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1500, 1.0, 0.6235294117647059, 0.3058823529411765, 2000, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2500, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 3000, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3500, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392]
field0LUT.UseBelowRangeColor = 1
field0LUT.BelowRangeColor = [1.0, 1.0, 0.7137254901960784]
field0LUT.UseAboveRangeColor = 1
field0LUT.AboveRangeColor = [0.705882, 0.0156863, 0.14902]
field0LUT.ScalarRangeInitialized = 1.0
field0LUT.NumberOfTableValues = 6

appendAttributes1Display.Representation = 'Surface'
appendAttributes1Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes1Display.LookupTable = field0LUT
appendAttributes1Display.SetScalarBarVisibility(renderView1, True)

# show data from fSASSELLOshp
fSASSELLOshpDisplay = Show(fSASSELLOshp, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
fSASSELLOshpDisplay.Representation = 'Wireframe'
fSASSELLOshpDisplay.AmbientColor = [0.0, 1.0, 0.0]
fSASSELLOshpDisplay.ColorArrayName = [None, '']
fSASSELLOshpDisplay.DiffuseColor = [0.0, 1.0, 0.0]
fSASSELLOshpDisplay.LineWidth = 5.0

# show data from fARENZANOshp
fARENZANOshpDisplay = Show(fARENZANOshp, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
fARENZANOshpDisplay.Representation = 'Wireframe'
fARENZANOshpDisplay.AmbientColor = [0.3333333333333333, 1.0, 0.0]
fARENZANOshpDisplay.ColorArrayName = [None, '']
fARENZANOshpDisplay.DiffuseColor = [0.3333333333333333, 1.0, 0.0]
fARENZANOshpDisplay.LineWidth = 5.0

# set scalar coloring
ColorBy(appendAttributes1Display, ('CELLS', 'Field 0'))

# reset view to fit data bounds
renderView1.ResetCamera()

# To save a specific target resolution, rather than using the
# the current view (or layout) size, and override the color palette.
directory = script_dir+'/../../'+project_folder+'/_fig'
if not os.path.exists(directory): 
	os.mkdir(directory)
	
SaveScreenshot(directory+'/'+var_name+'_'+geom_name+'.png', renderView1, FontScaling=True, OverrideColorPalette='WhiteBackground', TransparentBackground=False)


if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')
