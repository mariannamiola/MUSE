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
project_name='11_matrac_3D_poly_mv_tet'

var_name='DO_mgl'
sub=''
vario='OMNI3D'
geom_name='porto'

compute_sim=var_name+'_'+vario+'_'+geom_name

project_folder=work_folder +'/'+project_name
geom_folder=project_folder+'/out/geometry/volume'
compute_folder=project_folder+'/out/compute'

space='/_varspace/_stats'
stats_name='mean'
sim_name=var_name+'_'+stats_name

csv_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+space+'/'+sim_name+'.csv'
vtk_file=script_dir+'/../../'+geom_folder+'/'+geom_name+'.vtk'

#samplescsv_file=script_dir+'/../data/samples.csv'

##csv_file=script_dir+'/../../MUSE_test/11_matrac_3D_poly_mv_tet/out/compute/DO_mgl_OMNI3D_porto/DO_mgl.csv'
##vtk_file=script_dir+'/../../MUSE_test/11_matrac_3D_poly_mv_tet/out/geometry/volume/porto.vtk'

print(csv_file)
print(vtk_file)


# create a new 'CSV Reader'
DO_mglcsv = CSVReader(registrationName='DO_mgl.csv', FileName=[csv_file])
DO_mglcsv.UseStringDelimiter = 0
DO_mglcsv.HaveHeaders = 0
DO_mglcsv.FieldDelimiterCharacters = ''
DO_mglcsv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tableToPoints1 = TableToPoints(registrationName='TableToPoints1', Input=DO_mglcsv)
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

# create a new 'Legacy VTK Reader'
portovtk = LegacyVTKReader(registrationName='porto.vtk', FileNames=[vtk_file])

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[portovtk, pointDatatoCellData1])

# create a new 'Transform'
transform1 = Transform(registrationName='Transform1', Input=appendAttributes1)
transform1.Transform = 'Transform'
transform1.TransformAllInputVectors = 0

# init the 'Transform' selected for 'Transform'
transform1.Transform.Scale = [1.0, 1.0, 20.0]

# ----------------------------------------------------------------
# restore active source
SetActiveSource(appendAttributes1)
#SetActiveSource(transform1)
# ----------------------------------------------------------------

# get active source.
#appendAttributes1 = GetActiveSource()
#transform1 = GetActiveSource()

# get active view
renderView1 = GetActiveViewOrCreate('RenderView')

# show data from appendAttributes1
appendAttributes1Display = Show(appendAttributes1, renderView1, 'UnstructuredGridRepresentation')

# get color transfer function/color map for 'Field0'
field0LUT = GetColorTransferFunction('Field0')

# trace defaults for the display properties
appendAttributes1Display.Representation = 'Surface'
appendAttributes1Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes1Display.LookupTable = field0LUT

# show data from transform1
transform1Display = Show(transform1, renderView1, 'UnstructuredGridRepresentation')

# trace defaults for the display properties.
transform1Display.Representation = 'Surface'
transform1Display.ColorArrayName = ['CELLS', 'Field 0']
transform1Display.LookupTable = field0LUT

# show color legend
appendAttributes1Display.SetScalarBarVisibility(renderView1, True)

# hide data in view
#Hide(appendAttributes1, renderView1)

# show color legend
transform1Display.SetScalarBarVisibility(renderView1, True)

# set scalar coloring
#ColorBy(appendAttributes1Display, ('CELLS', 'Field 0'))
ColorBy(transform1Display, ('CELLS', 'Field 0'))

# reset view to fit data bounds
renderView1.ResetCamera()
renderView1.CameraPosition = [1494977.9395742328, 4912153.631749143, 2759.704946000928]
renderView1.CameraFocalPoint = [1494510.0000000007, 4916330.000000001, -2.5000000001114273]
renderView1.CameraViewUp = [-0.05902798335304039, 0.5460776906435952, 0.835652351736433]


if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')
