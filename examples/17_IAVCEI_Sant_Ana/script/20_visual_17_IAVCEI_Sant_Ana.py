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
project_name='17_IAVCEI_Sant_Ana'

var_name='CO2_flux'
sub=''
vario='DIR2D'
geom_name='domain'

compute_sim=var_name+'_'+vario+'_'+geom_name

project_folder=work_folder +'/'+project_name
geom_folder=project_folder+'/out/geometry/surf'
compute_folder=project_folder+'/out/compute'

space='/_varspace/_stats'
stats_name='mean'
sim_name=var_name+'_'+stats_name

csv_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+space+'/'+sim_name+'.csv'
obj_file=script_dir+'/../../'+geom_folder+'/'+geom_name+'.obj'
samplescsv_file=script_dir+'/../data/samples.csv'

print(csv_file)
print(obj_file)

# create a new 'CSV Reader'
CO2_fluxcsv = CSVReader(registrationName='CO2_flux.csv', FileName=[csv_file])
CO2_fluxcsv.UseStringDelimiter = 0
CO2_fluxcsv.HaveHeaders = 0
CO2_fluxcsv.FieldDelimiterCharacters = ''
CO2_fluxcsv.AddTabFieldDelimiter = 1

# create a new 'CSV Reader'
samplescsv = CSVReader(registrationName='samples.csv', FileName=[samplescsv_file])
samplescsv.UseStringDelimiter = 0
samplescsv.HaveHeaders = 1
samplescsv.FieldDelimiterCharacters = ';'

# create a new 'Table To Points'
tableToPoints1 = TableToPoints(registrationName='TableToPoints1', Input=samplescsv)
tableToPoints1.XColumn = 'x'
tableToPoints1.YColumn = 'y'
tableToPoints1.ZColumn = 'CO2_flux'
tableToPoints1.a2DPoints = 1
tableToPoints1.KeepAllDataArrays = 1

# create a new 'Table To Points'
tableToPoints2 = TableToPoints(registrationName='TableToPoints2', Input=CO2_fluxcsv)
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

# create a new 'Wavefront OBJ Reader'
domainobj = WavefrontOBJReader(registrationName='domain.obj', FileName=obj_file)

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[domainobj, pointDatatoCellData1])

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

appendAttributes1Display.Representation = 'Surface'
appendAttributes1Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes1Display.LookupTable = field0LUT

# get color legend/bar for field0LUT in view renderView1
field0LUTColorBar = GetScalarBar(field0LUT, renderView1)
#field0LUTColorBar.WindowLocation = 'Any Location'
#field0LUTColorBar.Position = [0.15356589147286817, 0.01825174825174817]
field0LUTColorBar.Title = 'CO2_flux'
field0LUTColorBar.ComponentTitle = '[g/m2 d]'
field0LUTColorBar.HorizontalTitle = 1
#field0LUTColorBar.AutomaticLabelFormat = 1
#field0LUTColorBar.UseCustomLabels = 1
field0LUTColorBar.RangeLabelFormat = '%-#7.3g'
#field0LUTColorBar.ScalarBarLength = 0.9174125874125877

appendAttributes1Display.SetScalarBarVisibility(renderView1, True)

# set scalar coloring
ColorBy(appendAttributes1Display, ('CELLS', 'Field 0'))

# reset view to fit data bounds
renderView1.ResetCamera()
#renderView1.CameraPosition = [23.673108484339377, 26.10432893807473, 136.60254037844388]
#renderView1.CameraFocalPoint = [23.673108484339377, 26.10432893807473, 0.0]
#renderView1.CameraFocalDisk = 1.0

# To save a specific target resolution, rather than using the
# the current view (or layout) size, and override the color palette.
directory = script_dir+'/../../'+project_folder+'/_fig'
if not os.path.exists(directory): 
	os.mkdir(directory)
	
SaveScreenshot(directory+'/'+sim_name+'.png', renderView1, FontScaling=True, OverrideColorPalette='WhiteBackground', TransparentBackground=False)


if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')
