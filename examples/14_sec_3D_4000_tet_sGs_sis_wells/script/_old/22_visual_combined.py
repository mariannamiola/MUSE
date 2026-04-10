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

script_dir = os.path.dirname(os.path.realpath(__file__))

work_folder = 'MUSE_test'
project_name = '14_sec_3D_4000_hex_sGs_sis'

# Variables for first visualizer (ind)
var_name_ind = 'phi'
sub = ''
vario = 'DIR2D'
geom_name = 'SEC'

compute_sim_ind = var_name_ind + '_' + vario + '_' + geom_name

# Variables for second visualizer (sGs)
var_name_sgs = 'phi_sgs'
compute_sim_sgs = var_name_sgs + '_' + vario + '_' + geom_name
space = '/_varspace/_stats'
stats_name = 'mean'
sim_name_sgs = var_name_sgs + '_' + stats_name

project_folder = work_folder + '/' + project_name
geom_folder = project_folder + '/out/geometry/volume'
compute_folder = project_folder + '/out/compute'

# File paths
vtk_file = script_dir + '/../../' + geom_folder + '/' + geom_name + '_rot.vtk'
samplescsv_file = script_dir + '/../data/samples.csv'

# First visualizer files
csv_file_ind = script_dir + '/../../' + compute_folder + '/' + compute_sim_ind + '/sisim.csv'
pdf0_file = script_dir + '/../../' + compute_folder + '/' + compute_sim_ind + '/pdf_cat_0.csv'
pdf1_file = script_dir + '/../../' + compute_folder + '/' + compute_sim_ind + '/pdf_cat_1.csv'
pdf2_file = script_dir + '/../../' + compute_folder + '/' + compute_sim_ind + '/pdf_cat_2.csv'
pdf3_file = script_dir + '/../../' + compute_folder + '/' + compute_sim_ind + '/pdf_cat_3.csv'
pdf4_file = script_dir + '/../../' + compute_folder + '/' + compute_sim_ind + '/pdf_cat_4.csv'
pdf5_file = script_dir + '/../../' + compute_folder + '/' + compute_sim_ind + '/pdf_cat_5.csv'
pdf6_file = script_dir + '/../../' + compute_folder + '/' + compute_sim_ind + '/pdf_cat_6.csv'
pdf7_file = script_dir + '/../../' + compute_folder + '/' + compute_sim_ind + '/pdf_cat_7.csv'

# Second visualizer files
csv_file_sgs = script_dir + '/../../' + compute_folder + '/' + compute_sim_sgs + space + '/' + sim_name_sgs + '.csv'

print('VTK file:', vtk_file)
print('Samples file:', samplescsv_file)
print('IND CSV:', csv_file_ind)
print('SGS CSV:', csv_file_sgs)

# Load geometry once
gridvtk = LegacyVTKReader(registrationName=geom_name + '.vtk', FileNames=[vtk_file])

# Load samples
samplescsv = CSVReader(registrationName='samples.csv', FileName=[samplescsv_file])
samplescsv.UseStringDelimiter = 0
samplescsv.HaveHeaders = 1
samplescsv.FieldDelimiterCharacters = ';'

tableToPoints_samples = TableToPoints(registrationName='TableToPoints_samples', Input=samplescsv)
tableToPoints_samples.XColumn = 'X'
tableToPoints_samples.YColumn = 'Y'
tableToPoints_samples.ZColumn = 'Z'
tableToPoints_samples.KeepAllDataArrays = 1

# First visualizer: BEST (sisim)
sisimcsv = CSVReader(registrationName='sisim.csv', FileName=[csv_file_ind])
sisimcsv.HaveHeaders = 0
sisimcsv.FieldDelimiterCharacters = ''
sisimcsv.AddTabFieldDelimiter = 1

tableToPoints1 = TableToPoints(registrationName='TableToPoints1', Input=sisimcsv)
tableToPoints1.XColumn = 'Field 0'
tableToPoints1.YColumn = 'Field 0'
tableToPoints1.ZColumn = 'Field 0'
tableToPoints1.KeepAllDataArrays = 1

tetrahedralize1 = Tetrahedralize(registrationName='Tetrahedralize1', Input=tableToPoints1)

pointDatatoCellData1 = PointDatatoCellData(registrationName='PointDatatoCellData1', Input=tetrahedralize1)
pointDatatoCellData1.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData1.PassPointData = 1

best = AppendAttributes(registrationName='best', Input=[gridvtk, pointDatatoCellData1])

# PDF categories for first visualizer
pdf_files = [pdf0_file, pdf1_file, pdf2_file, pdf3_file, pdf4_file, pdf5_file, pdf6_file, pdf7_file]
pdf_objects = []

for i, pdf_file in enumerate(pdf_files):
    pdf_csv = CSVReader(registrationName=f'pdf_cat_{i}.csv', FileName=[pdf_file])
    pdf_csv.UseStringDelimiter = 0
    pdf_csv.HaveHeaders = 0
    pdf_csv.FieldDelimiterCharacters = ' '
    pdf_csv.AddTabFieldDelimiter = 1

    tableToPoints_pdf = TableToPoints(registrationName=f'TableToPoints_pdf_{i}', Input=pdf_csv)
    tableToPoints_pdf.XColumn = 'Field 0'
    tableToPoints_pdf.YColumn = 'Field 0'
    tableToPoints_pdf.ZColumn = 'Field 0'
    tableToPoints_pdf.KeepAllDataArrays = 1

    tetrahedralize_pdf = Tetrahedralize(registrationName=f'Tetrahedralize_pdf_{i}', Input=tableToPoints_pdf)

    pointDatatoCellData_pdf = PointDatatoCellData(registrationName=f'PointDatatoCellData_pdf_{i}', Input=tetrahedralize_pdf)
    pointDatatoCellData_pdf.PointDataArraytoprocess = ['Field 0']
    pointDatatoCellData_pdf.PassPointData = 1

    pdf_cat = AppendAttributes(registrationName=f'pdf_cat{i}', Input=[gridvtk, pointDatatoCellData_pdf])
    pdf_objects.append(pdf_cat)

# Second visualizer: phi_mean
phi_meancsv = CSVReader(registrationName='phi_mean.csv', FileName=[csv_file_sgs])
phi_meancsv.UseStringDelimiter = 0
phi_meancsv.HaveHeaders = 0
phi_meancsv.FieldDelimiterCharacters = ''
phi_meancsv.AddTabFieldDelimiter = 1

tableToPoints_sgs = TableToPoints(registrationName='TableToPoints_sgs', Input=phi_meancsv)
tableToPoints_sgs.XColumn = 'Field 0'
tableToPoints_sgs.YColumn = 'Field 0'
tableToPoints_sgs.ZColumn = 'Field 0'
tableToPoints_sgs.KeepAllDataArrays = 1

tetrahedralize_sgs = Tetrahedralize(registrationName='Tetrahedralize_sgs', Input=tableToPoints_sgs)

pointDatatoCellData_sgs = PointDatatoCellData(registrationName='PointDatatoCellData_sgs', Input=tetrahedralize_sgs)
pointDatatoCellData_sgs.PointDataArraytoprocess = ['Field 0']
pointDatatoCellData_sgs.PassPointData = 1

phi_mean = AppendAttributes(registrationName='phi_mean', Input=[gridvtk, pointDatatoCellData_sgs])

# ----------------------------------------------------------------
# setup the visualization in view 'renderView1'
# ----------------------------------------------------------------

# Show geometry
gridvtkDisplay = Show(gridvtk, renderView1, 'GeometryRepresentation')
gridvtkDisplay.Representation = 'Surface'
gridvtkDisplay.ColorArrayName = [None, '']
gridvtkDisplay.ScaleFactor = 10.0

# Show best (first visualizer)
bestDisplay = Show(best, renderView1, 'GeometryRepresentation')
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

bestDisplay.Representation = 'Surface'
bestDisplay.ColorArrayName = ['CELLS', 'Field 0']
bestDisplay.LookupTable = field0LUT
bestDisplay.ScaleFactor = 10.0

# Show phi_mean (second visualizer)
phi_meanDisplay = Show(phi_mean, renderView1, 'GeometryRepresentation')
field0LUT_sgs = GetColorTransferFunction('Field0', phi_meanDisplay, separate=True)
phi_meanDisplay.Representation = 'Surface'
phi_meanDisplay.ColorArrayName = ['CELLS', 'Field 0']
phi_meanDisplay.LookupTable = field0LUT_sgs
phi_meanDisplay.ScaleFactor = 10.0
phi_meanDisplay.SetScalarBarVisibility(renderView1, True)

# Show samples
samplesDisplay = Show(tableToPoints_samples, renderView1, 'GeometryRepresentation')
samplesDisplay.Representation = 'Points'
samplesDisplay.PointSize = 5.0
samplesDisplay.ColorArrayName = [None, '']

# Optionally show PDF categories (commented out to avoid clutter, can be enabled if needed)
# for pdf_cat in pdf_objects:
#     pdfDisplay = Show(pdf_cat, renderView1, 'GeometryRepresentation')
#     pdfDisplay.Representation = 'Surface'
#     pdfDisplay.ColorArrayName = ['CELLS', 'Field 0']
#     pdfDisplay.ScaleFactor = 10.0

# Reset camera
renderView1.ResetCamera()

# Save screenshot for phi_mean
directory = script_dir + '/../../' + project_folder + '/_fig'
if not os.path.exists(directory):
    os.mkdir(directory)

SaveScreenshot(directory + '/' + sim_name_sgs + '.png', renderView1, FontScaling=True, OverrideColorPalette='WhiteBackground', TransparentBackground=False)

if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')