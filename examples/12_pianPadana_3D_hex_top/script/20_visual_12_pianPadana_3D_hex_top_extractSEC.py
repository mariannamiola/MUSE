import os
import sys
import csv

# state file generated using paraview version 5.13.0
import paraview
paraview.compatibility.major = 5
paraview.compatibility.minor = 13

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

# ----------------------------------------------------------------
# setup views used in the visualization
# ----------------------------------------------------------------

# get the material library
materialLibrary1 = GetMaterialLibrary()

# Create a new 'Render View'
renderView1 = CreateView('RenderView')
renderView1.ViewSize = [2152, 1140]
renderView1.AxesGrid = 'Grid Axes 3D Actor'
renderView1.StereoType = 'Crystal Eyes'
renderView1.CameraFocalDisk = 1.0
renderView1.LegendGrid = 'Legend Grid Actor'
renderView1.PolarGrid = 'Polar Grid Actor'
renderView1.BackEnd = 'OSPRay raycaster'
renderView1.OSPRayMaterialLibrary = materialLibrary1

# Create a new 'Render View'
renderView2 = CreateView('RenderView')
renderView2.ViewSize = [2152, 1140]
renderView2.InteractionMode = '2D'
renderView2.AxesGrid = 'Grid Axes 3D Actor'
renderView2.CenterOfRotation = [508514.35905256, 4991352.5, -3092.0]
renderView2.StereoType = 'Crystal Eyes'
renderView2.CameraPosition = [508514.35905256, 4991352.5, 559643.2389617587]
renderView2.CameraFocalPoint = [508514.35905256, 4991352.5, -3092.0]
renderView2.CameraFocalDisk = 1.0
renderView2.CameraParallelScale = 86309.09463325702
renderView2.LegendGrid = 'Legend Grid Actor'
renderView2.PolarGrid = 'Polar Grid Actor'
renderView2.BackEnd = 'OSPRay raycaster'
renderView2.OSPRayMaterialLibrary = materialLibrary1

SetActiveView(None)

# ----------------------------------------------------------------
# setup view layouts
# ----------------------------------------------------------------

# create new layout object 'Layout #1'
layout1 = CreateLayout(name='Layout #1')
layout1.AssignView(0, renderView1)
layout1.SetSize(2152, 1140)

# create new layout object 'Layout #1'
layout1_1 = CreateLayout(name='Layout #1')
layout1_1.AssignView(0, renderView2)
layout1_1.SetSize(2152, 1140)

# ----------------------------------------------------------------
# restore active view
SetActiveView(renderView2)
# ----------------------------------------------------------------

##### SET VARIABLES
discretized_scale = True
fix_depth = True
all_profiles = True
#####

script_dir=os.path.dirname(os.path.realpath(__file__))

script_dir=os.path.dirname(os.path.realpath(__file__))

work_folder='MUSE_test'
project_name='12_pianPadana_3D_hex_top'
project_folder=work_folder +'/'+project_name

var_name='T'
sub=''
vario='DIR3Dxy'
geom_name='top-bottom'

geom_folder=project_folder+'/out/geometry/volume'
vtk_file=script_dir+'/../../'+geom_folder+'/'+geom_name+'.vtk'
print(vtk_file)

compute_folder=project_folder+'/out/compute'
compute_sim=var_name+'_'+vario+'_'+geom_name
space='/_varspace/_stats'

stats_name_list = ["mean", "mean_p_stdev", "mean_m_stdev"]  # Modify with actual filenames

for stats_name in stats_name_list:
    sim_name=var_name+'_'+stats_name

    csv_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+space+'/'+sim_name+'.csv'
    print(csv_file)


    samplescsv_file=script_dir+'/../data/samples.csv'

    # ----------------------------------------------------------------
    # setup the data processing pipelines
    # ----------------------------------------------------------------

    # create a new 'Text'
    malpaga1 = Text(registrationName='Malpaga 1')
    malpaga1.Text = 'Malpaga 1'

    # create a new 'Text'
    baiso1 = Text(registrationName='Baiso 1')
    baiso1.Text = 'Baiso 1'

    # create a new 'CSV Reader'
    t_CORR_meancsv = CSVReader(registrationName='T_CORR_mean.csv', FileName=[csv_file])
    t_CORR_meancsv.UseStringDelimiter = 0
    t_CORR_meancsv.HaveHeaders = 0
    t_CORR_meancsv.FieldDelimiterCharacters = ' '

    # create a new 'Text'
    boscorosso1 = Text(registrationName='Boscorosso 1')
    boscorosso1.Text = 'Bosco Rosso 1'

    # create a new 'Text'
    mortara1 = Text(registrationName='Mortara 1')
    mortara1.Text = 'MT'

    # create a new 'Table To Points'
    tableToPoints1 = TableToPoints(registrationName='TableToPoints1', Input=t_CORR_meancsv)
    tableToPoints1.XColumn = 'Field 0'
    tableToPoints1.YColumn = 'Field 0'
    tableToPoints1.ZColumn = 'Field 0'
    tableToPoints1.KeepAllDataArrays = 1

    # create a new 'Text'
    piadena1 = Text(registrationName='Piadena 1')
    piadena1.Text = 'Piadena 1'

    # create a new 'CSV Reader'
    samplescsv = CSVReader(registrationName='samples.csv', FileName=[samplescsv_file])
    samplescsv.UseStringDelimiter = 0
    samplescsv.FieldDelimiterCharacters = ';'

    # create a new 'Text'
    borgoAle1 = Text(registrationName='Borgo Ale 1')
    borgoAle1.Text = 'Borgo Ale 1'

    # create a new 'Legacy VTK Reader'
    topbottomvtk = LegacyVTKReader(registrationName='top-bottom.vtk', FileNames=[vtk_file])

    # create a new 'Text'
    casanovaStaffora1 = Text(registrationName='Casanova Staffora 1')
    casanovaStaffora1.Text = 'Casanova Staffora 1'

    # create a new 'Text'
    lamadeiCerri1 = Text(registrationName='Lama dei Cerri 1')
    lamadeiCerri1.Text = 'Lama dei Cerri 1'

    # create a new 'Text'
    quara1 = Text(registrationName='Quara1')
    quara1.Text = 'Quara 1'

    # create a new 'Text'
    castelnovo1 = Text(registrationName='Castelnovo 1')
    castelnovo1.Text = 'Castelnovo 1'

    # create a new 'Text'
    rodigo1 = Text(registrationName='Rodigo 1')
    rodigo1.Text = 'Rodigo 1'

    # create a new 'Text'
    battuda1 = Text(registrationName='Battuda 1')
    battuda1.Text = 'BT'
    
    # create a new 'Text'
    malossa3 = Text(registrationName='Malossa 3')
    malossa3.Text = 'MA'
    
    # create a new 'Text'
    valleSalimbene1 = Text(registrationName='Valle Salimbene 1')
    valleSalimbene1.Text = 'VS'

    # create a new 'Text'
    brignano1 = Text(registrationName='Brignano 1')
    brignano1.Text = 'BR'

    # create a new 'Text'
    saliVercellese1 = Text(registrationName='Sali Vercellese 1')
    saliVercellese1.Text = 'SV'

    # create a new 'Tetrahedralize'
    tetrahedralize1 = Tetrahedralize(registrationName='Tetrahedralize1', Input=tableToPoints1)

    # create a new 'Point Data to Cell Data'
    pointDatatoCellData1 = PointDatatoCellData(registrationName='PointDatatoCellData1', Input=tetrahedralize1)
    pointDatatoCellData1.PointDataArraytoprocess = ['Field 0']
    pointDatatoCellData1.PassPointData = 1

    # create a new 'Append Attributes'
    appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[topbottomvtk, pointDatatoCellData1])

    # create a new 'Slice'
    slice1 = Slice(registrationName='Slice1', Input=appendAttributes1)
    slice1.SliceType = 'Plane'
    slice1.HyperTreeGridSlicer = 'Plane'
    slice1.SliceOffsetValues = [0.0]
    slice1.PointMergeMethod = 'Uniform Binning'

    # init the 'Plane' selected for 'SliceType'
    if fix_depth:
        z_value = -4000.0
    else:
        z_value = -abs(float(sys.argv[1]))  # per renderlo negativo
    slice1.SliceType.Origin = [508456.0, 4991050.0, z_value]
    slice1.SliceType.Normal = [0.0, 0.0, 1.0]

    # init the 'Plane' selected for 'HyperTreeGridSlicer'
    slice1.HyperTreeGridSlicer.Origin = [508456.0, 4991050.0, -3114.0]

    # create a new 'Cell Data to Point Data'
    cellDatatoPointData1 = CellDatatoPointData(registrationName='CellDatatoPointData1', Input=slice1)
    cellDatatoPointData1.CellDataArraytoprocess = ['Field 0', 'hex_selector']
    cellDatatoPointData1.PassCellData = 1

    # create a new 'Contour'
    # Clear previous contour if exists
    try:
        Delete(contour1)
        del contour1
    except NameError:
        pass                            
    contour1 = Contour(registrationName='Contour1', Input=cellDatatoPointData1)
    contour1.ContourBy = ['POINTS', 'Field 0']
    contour1.GenerateTriangles = 0
    contour1.Isosurfaces = [25.0, 50.0, 75.0, 100.0, 125.0, 150.0]
    contour1.PointMergeMethod = 'Uniform Binning'

    # create a new 'Table To Points'
    tableToPoints2 = TableToPoints(registrationName='TableToPoints2', Input=samplescsv)
    tableToPoints2.XColumn = 'X'
    tableToPoints2.YColumn = 'Y'
    tableToPoints2.ZColumn = 'Z'
    tableToPoints2.KeepAllDataArrays = 1

    ### VERTICAL PROFILES: create a new 'Plot Over Line'
    # 2. Create a vertical line (e.g., X=100, Y=50, Z from 0 to 100)
    z_max=863
    z_min=-7164


    if all_profiles:
        # 2. Read (label, x, y) from CSV
        points = []
        csvWells=script_dir+"/ALLwells.csv"
        with open(csvWells, "r") as csvfile:
            reader = csv.DictReader(csvfile, delimiter=';')
            for row in reader:
                label = row["name"]
                WELL_X = float(row["x"])
                WELL_Y = float(row["y"])
                points.append((label, WELL_X, WELL_Y))
            
            for label, WELL_X, WELL_Y in points:
                plotOverLine1 = PlotOverLine(registrationName='PlotOverLine1', Input=appendAttributes1)
                plotOverLine1.Resolution = 1000

                plotOverLine1.Point1 = [WELL_X, WELL_Y, z_max]
                plotOverLine1.Point2 = [WELL_X, WELL_Y, z_min]
                
                # Step 5: Export as CSV using ParaView's built-in functionality
                #directory_profile = script_dir+'/../../'+project_folder+'/_1Dprofile'
                directory_profile = script_dir+'/_1Dprofile'
                if not os.path.exists(directory_profile): 
	                os.mkdir(directory_profile)
	    
                profile_output_file=directory_profile+"/"+label+"profile_"+sim_name+".csv"
                SaveData(profile_output_file, proxy=plotOverLine1, Precision=10, FieldAssociation="Point Data", ChooseArraysToWrite=True, PointDataArrays=["Field 0"])
                print("CSV file -- 1D profiles saved successfully!")
    else:
        WELL_X=477550.0
        WELL_Y=5014200.0
        
        plotOverLine1.Point1 = [WELL_X, WELL_Y, z_max]
        plotOverLine1.Point2 = [WELL_X, WELL_Y, z_min]

        # Step 5: Export as CSV using ParaView's built-in functionality
        output_file=script_dir+"/example_profile.csv"
        SaveData(output_file, proxy=plotOverLine1, Precision=10, FieldAssociation="Point Data", ChooseArraysToWrite=True, PointDataArrays=["Field 0"])
        print("CSV file -- 1D profile saved successfully!")


    # ----------------------------------------------------------------
    # setup the visualization in view 'renderView2'
    # ----------------------------------------------------------------

    # show data from topbottomvtk
    topbottomvtkDisplay = Show(topbottomvtk, renderView2, 'UnstructuredGridRepresentation')

    # trace defaults for the display properties.
    topbottomvtkDisplay.Representation = 'Surface'
    topbottomvtkDisplay.ColorArrayName = ['POINTS', '']
    topbottomvtkDisplay.SelectNormalArray = 'None'
    topbottomvtkDisplay.SelectTangentArray = 'None'
    topbottomvtkDisplay.SelectTCoordArray = 'None'
    topbottomvtkDisplay.TextureTransform = 'Transform2'
    topbottomvtkDisplay.OSPRayScaleFunction = 'Piecewise Function'
    topbottomvtkDisplay.Assembly = ''
    topbottomvtkDisplay.SelectedBlockSelectors = ['']
    topbottomvtkDisplay.SelectOrientationVectors = 'None'
    topbottomvtkDisplay.ScaleFactor = 24600.0
    topbottomvtkDisplay.SelectScaleArray = 'None'
    topbottomvtkDisplay.GlyphType = 'Arrow'
    topbottomvtkDisplay.GlyphTableIndexArray = 'None'
    topbottomvtkDisplay.GaussianRadius = 1230.0
    topbottomvtkDisplay.SetScaleArray = ['POINTS', '']
    topbottomvtkDisplay.ScaleTransferFunction = 'Piecewise Function'
    topbottomvtkDisplay.OpacityArray = ['POINTS', '']
    topbottomvtkDisplay.OpacityTransferFunction = 'Piecewise Function'
    topbottomvtkDisplay.DataAxesGrid = 'Grid Axes Representation'
    topbottomvtkDisplay.PolarAxes = 'Polar Axes Representation'
    topbottomvtkDisplay.ScalarOpacityUnitDistance = 2304.41000039915
    topbottomvtkDisplay.OpacityArrayName = ['CELLS', 'hex_selector']
    topbottomvtkDisplay.SelectInputVectors = ['POINTS', '']
    topbottomvtkDisplay.WriteLog = ''

    # show data from tableToPoints1
    tableToPoints1Display = Show(tableToPoints1, renderView2, 'GeometryRepresentation')

    # trace defaults for the display properties.
    tableToPoints1Display.Representation = 'Surface'
    tableToPoints1Display.ColorArrayName = ['POINTS', '']
    tableToPoints1Display.SelectNormalArray = 'None'
    tableToPoints1Display.SelectTangentArray = 'None'
    tableToPoints1Display.SelectTCoordArray = 'None'
    tableToPoints1Display.TextureTransform = 'Transform2'
    tableToPoints1Display.OSPRayScaleArray = 'Field 0'
    tableToPoints1Display.OSPRayScaleFunction = 'Piecewise Function'
    tableToPoints1Display.Assembly = ''
    tableToPoints1Display.SelectedBlockSelectors = ['']
    tableToPoints1Display.SelectOrientationVectors = 'None'
    tableToPoints1Display.ScaleFactor = 18.653305617
    tableToPoints1Display.SelectScaleArray = 'Field 0'
    tableToPoints1Display.GlyphType = 'Arrow'
    tableToPoints1Display.GlyphTableIndexArray = 'Field 0'
    tableToPoints1Display.GaussianRadius = 0.9326652808500001
    tableToPoints1Display.SetScaleArray = ['POINTS', 'Field 0']
    tableToPoints1Display.ScaleTransferFunction = 'Piecewise Function'
    tableToPoints1Display.OpacityArray = ['POINTS', 'Field 0']
    tableToPoints1Display.OpacityTransferFunction = 'Piecewise Function'
    tableToPoints1Display.DataAxesGrid = 'Grid Axes Representation'
    tableToPoints1Display.PolarAxes = 'Polar Axes Representation'
    tableToPoints1Display.SelectInputVectors = ['POINTS', '']
    tableToPoints1Display.WriteLog = ''

    # init the 'Piecewise Function' selected for 'ScaleTransferFunction'
    tableToPoints1Display.ScaleTransferFunction.Points = [7.13233583, 0.0, 0.5, 0.0, 193.665392, 1.0, 0.5, 0.0]

    # init the 'Piecewise Function' selected for 'OpacityTransferFunction'
    tableToPoints1Display.OpacityTransferFunction.Points = [7.13233583, 0.0, 0.5, 0.0, 193.665392, 1.0, 0.5, 0.0]

    # show data from tetrahedralize1
    tetrahedralize1Display = Show(tetrahedralize1, renderView2, 'UnstructuredGridRepresentation')

    # trace defaults for the display properties.
    tetrahedralize1Display.Representation = 'Surface'
    tetrahedralize1Display.ColorArrayName = ['POINTS', '']
    tetrahedralize1Display.SelectNormalArray = 'None'
    tetrahedralize1Display.SelectTangentArray = 'None'
    tetrahedralize1Display.SelectTCoordArray = 'None'
    tetrahedralize1Display.TextureTransform = 'Transform2'
    tetrahedralize1Display.OSPRayScaleArray = 'Field 0'
    tetrahedralize1Display.OSPRayScaleFunction = 'Piecewise Function'
    tetrahedralize1Display.Assembly = ''
    tetrahedralize1Display.SelectedBlockSelectors = ['']
    tetrahedralize1Display.SelectOrientationVectors = 'None'
    tetrahedralize1Display.ScaleFactor = 18.653305617
    tetrahedralize1Display.SelectScaleArray = 'Field 0'
    tetrahedralize1Display.GlyphType = 'Arrow'
    tetrahedralize1Display.GlyphTableIndexArray = 'Field 0'
    tetrahedralize1Display.GaussianRadius = 0.9326652808500001
    tetrahedralize1Display.SetScaleArray = ['POINTS', 'Field 0']
    tetrahedralize1Display.ScaleTransferFunction = 'Piecewise Function'
    tetrahedralize1Display.OpacityArray = ['POINTS', 'Field 0']
    tetrahedralize1Display.OpacityTransferFunction = 'Piecewise Function'
    tetrahedralize1Display.DataAxesGrid = 'Grid Axes Representation'
    tetrahedralize1Display.PolarAxes = 'Polar Axes Representation'
    tetrahedralize1Display.ScalarOpacityUnitDistance = 2.559611409205531
    tetrahedralize1Display.OpacityArrayName = ['POINTS', 'Field 0']
    tetrahedralize1Display.SelectInputVectors = ['POINTS', '']
    tetrahedralize1Display.WriteLog = ''

    # init the 'Piecewise Function' selected for 'ScaleTransferFunction'
    tetrahedralize1Display.ScaleTransferFunction.Points = [7.13233583, 0.0, 0.5, 0.0, 193.665392, 1.0, 0.5, 0.0]

    # init the 'Piecewise Function' selected for 'OpacityTransferFunction'
    tetrahedralize1Display.OpacityTransferFunction.Points = [7.13233583, 0.0, 0.5, 0.0, 193.665392, 1.0, 0.5, 0.0]

    # show data from pointDatatoCellData1
    pointDatatoCellData1Display = Show(pointDatatoCellData1, renderView2, 'UnstructuredGridRepresentation')

    # trace defaults for the display properties.
    pointDatatoCellData1Display.Representation = 'Surface'
    pointDatatoCellData1Display.ColorArrayName = ['POINTS', '']
    pointDatatoCellData1Display.SelectNormalArray = 'None'
    pointDatatoCellData1Display.SelectTangentArray = 'None'
    pointDatatoCellData1Display.SelectTCoordArray = 'None'
    pointDatatoCellData1Display.TextureTransform = 'Transform2'
    pointDatatoCellData1Display.OSPRayScaleArray = 'Field 0'
    pointDatatoCellData1Display.OSPRayScaleFunction = 'Piecewise Function'
    pointDatatoCellData1Display.Assembly = ''
    pointDatatoCellData1Display.SelectedBlockSelectors = ['']
    pointDatatoCellData1Display.SelectOrientationVectors = 'None'
    pointDatatoCellData1Display.ScaleFactor = 18.653305617
    pointDatatoCellData1Display.SelectScaleArray = 'Field 0'
    pointDatatoCellData1Display.GlyphType = 'Arrow'
    pointDatatoCellData1Display.GlyphTableIndexArray = 'Field 0'
    pointDatatoCellData1Display.GaussianRadius = 0.9326652808500001
    pointDatatoCellData1Display.SetScaleArray = ['POINTS', 'Field 0']
    pointDatatoCellData1Display.ScaleTransferFunction = 'Piecewise Function'
    pointDatatoCellData1Display.OpacityArray = ['POINTS', 'Field 0']
    pointDatatoCellData1Display.OpacityTransferFunction = 'Piecewise Function'
    pointDatatoCellData1Display.DataAxesGrid = 'Grid Axes Representation'
    pointDatatoCellData1Display.PolarAxes = 'Polar Axes Representation'
    pointDatatoCellData1Display.ScalarOpacityUnitDistance = 2.559611409205531
    pointDatatoCellData1Display.OpacityArrayName = ['POINTS', 'Field 0']
    pointDatatoCellData1Display.SelectInputVectors = ['POINTS', '']
    pointDatatoCellData1Display.WriteLog = ''

    # init the 'Piecewise Function' selected for 'ScaleTransferFunction'
    pointDatatoCellData1Display.ScaleTransferFunction.Points = [7.13233583, 0.0, 0.5, 0.0, 193.665392, 1.0, 0.5, 0.0]

    # init the 'Piecewise Function' selected for 'OpacityTransferFunction'
    pointDatatoCellData1Display.OpacityTransferFunction.Points = [7.13233583, 0.0, 0.5, 0.0, 193.665392, 1.0, 0.5, 0.0]

    # show data from appendAttributes1
    appendAttributes1Display = Show(appendAttributes1, renderView2, 'UnstructuredGridRepresentation')

    # get 2D transfer function for 'Field0'
    field0TF2D = GetTransferFunction2D('Field0')
    field0TF2D.ScalarRangeInitialized = 1
    #field0TF2D.Range = [0.0, 134.481445, 0.0, 1.0]
    field0TF2D.Range = [25.0, 150.0, 0.0, 1.0]

    # get color transfer function/color map for 'Field0'
    field0LUT = GetColorTransferFunction('Field0')
    field0LUT.TransferFunction2D = field0TF2D

    if discretized_scale:
        field0LUT.RGBPoints = [25.0, 0.23137254902, 0.298039215686, 0.752941176471, 50.0, 0.5529411764705883, 0.6901960784313725, 0.996078431372549, 75.0, 0.8666666666666667, 0.8666666666666667, 0.8666666666666667, 100.0, 0.865, 0.865, 0.865, 125.0, 0.9568627450980393, 0.6039215686274509, 0.4823529411764706, 150.0, 0.705882352941, 0.0156862745098, 0.149019607843]
        field0LUT.UseBelowRangeColor = 1
        field0LUT.BelowRangeColor = [0.0, 0.0, 0.4980392156862745]
        field0LUT.UseAboveRangeColor = 1
        field0LUT.AboveRangeColor = [1.0, 0.0, 0.0]
        field0LUT.NumberOfTableValues = 5
    else:
        field0LUT.RGBPoints = [7.13233583, 0.231373, 0.298039, 0.752941, 100.39886391499999, 0.865003, 0.865003, 0.865003, 193.665392, 0.705882, 0.0156863, 0.14902]
    field0LUT.ScalarRangeInitialized = 1.0

    # get opacity transfer function/opacity map for 'Field0'
    field0PWF = GetOpacityTransferFunction('Field0')
    if discretized_scale:
        field0PWF.Points = [25.0, 0.0, 0.5, 0.0, 150.0, 1.0, 0.5, 0.0]
    else:
        field0PWF.Points = [7.13233583, 0.0, 0.5, 0.0, 193.665392, 1.0, 0.5, 0.0]
    field0PWF.ScalarRangeInitialized = 1

    # trace defaults for the display properties.
    appendAttributes1Display.Representation = 'Surface'
    appendAttributes1Display.ColorArrayName = ['CELLS', 'Field 0']
    appendAttributes1Display.LookupTable = field0LUT
    appendAttributes1Display.SelectNormalArray = 'None'
    appendAttributes1Display.SelectTangentArray = 'None'
    appendAttributes1Display.SelectTCoordArray = 'None'
    appendAttributes1Display.TextureTransform = 'Transform2'
    appendAttributes1Display.OSPRayScaleFunction = 'Piecewise Function'
    appendAttributes1Display.Assembly = ''
    appendAttributes1Display.SelectedBlockSelectors = ['']
    appendAttributes1Display.SelectOrientationVectors = 'None'
    appendAttributes1Display.ScaleFactor = 24600.0
    appendAttributes1Display.SelectScaleArray = 'None'
    appendAttributes1Display.GlyphType = 'Arrow'
    appendAttributes1Display.GlyphTableIndexArray = 'None'
    appendAttributes1Display.GaussianRadius = 1230.0
    appendAttributes1Display.SetScaleArray = ['POINTS', '']
    appendAttributes1Display.ScaleTransferFunction = 'Piecewise Function'
    appendAttributes1Display.OpacityArray = ['POINTS', '']
    appendAttributes1Display.OpacityTransferFunction = 'Piecewise Function'
    appendAttributes1Display.DataAxesGrid = 'Grid Axes Representation'
    appendAttributes1Display.PolarAxes = 'Polar Axes Representation'
    appendAttributes1Display.ScalarOpacityFunction = field0PWF
    appendAttributes1Display.ScalarOpacityUnitDistance = 2304.41000039915
    appendAttributes1Display.OpacityArrayName = ['CELLS', 'Field 0']
    appendAttributes1Display.SelectInputVectors = ['POINTS', '']
    appendAttributes1Display.WriteLog = ''

    # show data from tableToPoints2
    tableToPoints2Display = Show(tableToPoints2, renderView2, 'GeometryRepresentation')

    # trace defaults for the display properties.
    tableToPoints2Display.Representation = 'Points'
    tableToPoints2Display.AmbientColor = [1.0, 0.0, 0.0]
    tableToPoints2Display.ColorArrayName = ['POINTS', '']
    tableToPoints2Display.DiffuseColor = [1.0, 0.0, 0.0]
    tableToPoints2Display.PointSize = 20.0
    tableToPoints2Display.RenderPointsAsSpheres = 1
    tableToPoints2Display.SelectNormalArray = 'None'
    tableToPoints2Display.SelectTangentArray = 'None'
    tableToPoints2Display.SelectTCoordArray = 'None'
    tableToPoints2Display.TextureTransform = 'Transform2'
    tableToPoints2Display.OSPRayScaleArray = 'T'
    tableToPoints2Display.OSPRayScaleFunction = 'Piecewise Function'
    tableToPoints2Display.Assembly = ''
    tableToPoints2Display.SelectedBlockSelectors = ['']
    tableToPoints2Display.SelectOrientationVectors = 'None'
    tableToPoints2Display.ScaleFactor = 24611.728189488
    tableToPoints2Display.SelectScaleArray = 'None'
    tableToPoints2Display.GlyphType = 'Arrow'
    tableToPoints2Display.GlyphTableIndexArray = 'None'
    tableToPoints2Display.GaussianRadius = 1230.5864094744
    tableToPoints2Display.SetScaleArray = ['POINTS', 'T']
    tableToPoints2Display.ScaleTransferFunction = 'Piecewise Function'
    tableToPoints2Display.OpacityArray = ['POINTS', 'T']
    tableToPoints2Display.OpacityTransferFunction = 'Piecewise Function'
    tableToPoints2Display.DataAxesGrid = 'Grid Axes Representation'
    tableToPoints2Display.PolarAxes = 'Polar Axes Representation'
    tableToPoints2Display.SelectInputVectors = ['POINTS', '']
    tableToPoints2Display.WriteLog = ''

    # init the 'Piecewise Function' selected for 'ScaleTransferFunction'
    tableToPoints2Display.ScaleTransferFunction.Points = [6.85, 0.0, 0.5, 0.0, 194.0, 1.0, 0.5, 0.0]

    # init the 'Piecewise Function' selected for 'OpacityTransferFunction'
    tableToPoints2Display.OpacityTransferFunction.Points = [6.85, 0.0, 0.5, 0.0, 194.0, 1.0, 0.5, 0.0]

    # show data from saliVercellese1
    saliVercellese1Display = Show(saliVercellese1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    saliVercellese1Display.WindowLocation = 'Any Location'
    saliVercellese1Display.Position = [0.316753099434673, 0.6628070175438595]
    saliVercellese1Display.Color = [0.0, 0.0, 0.0]
    saliVercellese1Display.FontSize = 16

    # show data from mortara1
    mortara1Display = Show(mortara1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    mortara1Display.WindowLocation = 'Any Location'
    mortara1Display.Position = [0.3957493819625541, 0.5873684210526315]
    mortara1Display.Color = [0.0, 0.0, 0.0]
    mortara1Display.FontSize = 16

    # show data from battuda1
    battuda1Display = Show(battuda1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    battuda1Display.WindowLocation = 'Any Location'
    battuda1Display.Position = [0.47939250463913397, 0.5785964912280702]
    battuda1Display.Color = [0.0, 0.0, 0.0]
    battuda1Display.FontSize = 16

    # show data from valleSalimbene1
    valleSalimbene1Display = Show(valleSalimbene1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    valleSalimbene1Display.WindowLocation = 'Any Location'
    valleSalimbene1Display.Position = [0.5230728020369035, 0.5119298245614035]
    valleSalimbene1Display.Color = [0.0, 0.0, 0.0]
    valleSalimbene1Display.FontSize = 16

    # show data from brignano1
    brignano1Display = Show(brignano1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    brignano1Display.WindowLocation = 'Any Location'
    brignano1Display.Position = [0.6355263336354167, 0.8207017543859649]
    brignano1Display.Color = [0.0, 0.0, 0.0]
    brignano1Display.FontSize = 16

    # show data from malossa3
    malossa3Display = Show(malossa3, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    malossa3Display.WindowLocation = 'Any Location'
    malossa3Display.Position = [0.5927754042673868, 0.7908771929824561]
    malossa3Display.Color = [0.0, 0.0, 0.0]
    malossa3Display.FontSize = 16

    # show data from rodigo1
    rodigo1Display = Show(rodigo1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    rodigo1Display.WindowLocation = 'Any Location'
    rodigo1Display.Position = [0.8529984525945243, 0.6259649122807017]
    rodigo1Display.Color = [0.0, 0.0, 0.0]
    rodigo1Display.FontSize = 16

    # show data from castelnovo1
    castelnovo1Display = Show(castelnovo1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    castelnovo1Display.WindowLocation = 'Any Location'
    castelnovo1Display.Position = [0.8780913893974983, 0.33122807017543854]
    castelnovo1Display.Color = [0.0, 0.0, 0.0]
    castelnovo1Display.FontSize = 16

    # show data from quara1
    quara1Display = Show(quara1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    quara1Display.WindowLocation = 'Any Location'
    quara1Display.Position = [0.8195412035238924, 0.029473684210526284]
    quara1Display.Color = [0.0, 0.0, 0.0]
    quara1Display.FontSize = 16

    # show data from lamadeiCerri1
    lamadeiCerri1Display = Show(lamadeiCerri1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    lamadeiCerri1Display.WindowLocation = 'Any Location'
    lamadeiCerri1Display.Position = [0.6364557016651563, 0.05403508771929824]
    lamadeiCerri1Display.Color = [0.0, 0.0, 0.0]
    lamadeiCerri1Display.FontSize = 16

    # show data from casanovaStaffora1
    casanovaStaffora1Display = Show(casanovaStaffora1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    casanovaStaffora1Display.WindowLocation = 'Any Location'
    casanovaStaffora1Display.Position = [0.45708767192537936, 0.24701754385964908]
    casanovaStaffora1Display.Color = [0.0, 0.0, 0.0]
    casanovaStaffora1Display.FontSize = 16

    # show data from borgoAle1
    borgoAle1Display = Show(borgoAle1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    borgoAle1Display.WindowLocation = 'Any Location'
    borgoAle1Display.Position = [0.16898358270604846, 0.7259649122807017]
    borgoAle1Display.Color = [0.0, 0.0, 0.0]
    borgoAle1Display.FontSize = 16

    # show data from boscorosso1
    boscorosso1Display = Show(boscorosso1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    boscorosso1Display.WindowLocation = 'Any Location'
    boscorosso1Display.Position = [0.8437047722971266, 0.39789473684210525]
    boscorosso1Display.Color = [0.0, 0.0, 0.0]
    boscorosso1Display.FontSize = 16

    # show data from baiso1
    baiso1Display = Show(baiso1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    baiso1Display.WindowLocation = 'Any Location'
    baiso1Display.Position = [0.8808794934867177, 0.11719298245614032]
    baiso1Display.Color = [0.0, 0.0, 0.0]
    baiso1Display.FontSize = 16

    # show data from malpaga1
    malpaga1Display = Show(malpaga1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    malpaga1Display.WindowLocation = 'Any Location'
    malpaga1Display.Position = [0.7972363708101378, 0.6926315789473684]
    malpaga1Display.Color = [0.0, 0.0, 0.0]
    malpaga1Display.FontSize = 16

    # show data from piadena1
    piadena1Display = Show(piadena1, renderView2, 'TextSourceRepresentation')

    # trace defaults for the display properties.
    piadena1Display.WindowLocation = 'Any Location'
    piadena1Display.Position = [0.7823664823343013, 0.503157894736842]
    piadena1Display.Color = [0.0, 0.0, 0.0]
    piadena1Display.FontSize = 16

    # show data from slice1
    slice1Display = Show(slice1, renderView2, 'GeometryRepresentation')

    # trace defaults for the display properties.
    slice1Display.Representation = 'Surface'
    slice1Display.ColorArrayName = ['CELLS', 'Field 0']
    slice1Display.LookupTable = field0LUT
    slice1Display.SelectNormalArray = 'None'
    slice1Display.SelectTangentArray = 'None'
    slice1Display.SelectTCoordArray = 'None'
    slice1Display.TextureTransform = 'Transform2'
    slice1Display.OSPRayScaleFunction = 'Piecewise Function'
    slice1Display.Assembly = ''
    slice1Display.SelectedBlockSelectors = ['']
    slice1Display.SelectOrientationVectors = 'None'
    slice1Display.ScaleFactor = 13600.0
    slice1Display.SelectScaleArray = 'None'
    slice1Display.GlyphType = 'Arrow'
    slice1Display.GlyphTableIndexArray = 'None'
    slice1Display.GaussianRadius = 680.0
    slice1Display.SetScaleArray = [None, '']
    slice1Display.ScaleTransferFunction = 'Piecewise Function'
    slice1Display.OpacityArray = [None, '']
    slice1Display.OpacityTransferFunction = 'Piecewise Function'
    slice1Display.DataAxesGrid = 'Grid Axes Representation'
    slice1Display.PolarAxes = 'Polar Axes Representation'
    slice1Display.SelectInputVectors = [None, '']
    slice1Display.WriteLog = ''

    # show data from cellDatatoPointData1
    cellDatatoPointData1Display = Show(cellDatatoPointData1, renderView2, 'GeometryRepresentation')

    # trace defaults for the display properties.
    cellDatatoPointData1Display.Representation = 'Surface'
    cellDatatoPointData1Display.ColorArrayName = ['CELLS', 'Field 0']
    cellDatatoPointData1Display.LookupTable = field0LUT
    cellDatatoPointData1Display.SelectNormalArray = 'None'
    cellDatatoPointData1Display.SelectTangentArray = 'None'
    cellDatatoPointData1Display.SelectTCoordArray = 'None'
    cellDatatoPointData1Display.TextureTransform = 'Transform2'
    cellDatatoPointData1Display.OSPRayScaleArray = 'Field 0'
    cellDatatoPointData1Display.OSPRayScaleFunction = 'Piecewise Function'
    cellDatatoPointData1Display.Assembly = ''
    cellDatatoPointData1Display.SelectedBlockSelectors = ['']
    cellDatatoPointData1Display.SelectOrientationVectors = 'None'
    cellDatatoPointData1Display.ScaleFactor = 24600.0
    cellDatatoPointData1Display.SelectScaleArray = 'Field 0'
    cellDatatoPointData1Display.GlyphType = 'Arrow'
    cellDatatoPointData1Display.GlyphTableIndexArray = 'Field 0'
    cellDatatoPointData1Display.GaussianRadius = 1230.0
    cellDatatoPointData1Display.SetScaleArray = ['POINTS', 'Field 0']
    cellDatatoPointData1Display.ScaleTransferFunction = 'Piecewise Function'
    cellDatatoPointData1Display.OpacityArray = ['POINTS', 'Field 0']
    cellDatatoPointData1Display.OpacityTransferFunction = 'Piecewise Function'
    cellDatatoPointData1Display.DataAxesGrid = 'Grid Axes Representation'
    cellDatatoPointData1Display.PolarAxes = 'Polar Axes Representation'
    cellDatatoPointData1Display.SelectInputVectors = [None, '']
    cellDatatoPointData1Display.WriteLog = ''

    # init the 'Piecewise Function' selected for 'ScaleTransferFunction'
    cellDatatoPointData1Display.ScaleTransferFunction.Points = [58.04287276666667, 0.0, 0.5, 0.0, 132.8021875, 1.0, 0.5, 0.0]

    # init the 'Piecewise Function' selected for 'OpacityTransferFunction'
    cellDatatoPointData1Display.OpacityTransferFunction.Points = [58.04287276666667, 0.0, 0.5, 0.0, 132.8021875, 1.0, 0.5, 0.0]

    # show data from contour1
    contour1Display = Show(contour1, renderView2, 'GeometryRepresentation')

    # trace defaults for the display properties.
    contour1Display.Representation = 'Wireframe'
    contour1Display.AmbientColor = [0.0, 0.0, 0.0]
    contour1Display.ColorArrayName = ['POINTS', '']
    contour1Display.DiffuseColor = [0.0, 0.0, 0.0]
    contour1Display.LineWidth = 2.0
    contour1Display.SelectNormalArray = 'None'
    contour1Display.SelectTangentArray = 'None'
    contour1Display.SelectTCoordArray = 'None'
    contour1Display.TextureTransform = 'Transform2'
    contour1Display.OSPRayScaleArray = 'Field 0'
    contour1Display.OSPRayScaleFunction = 'Piecewise Function'
    contour1Display.Assembly = ''
    contour1Display.SelectedBlockSelectors = ['']
    contour1Display.SelectOrientationVectors = 'None'
    contour1Display.ScaleFactor = 19850.624403493377
    contour1Display.SelectScaleArray = 'Field 0'
    contour1Display.GlyphType = 'Arrow'
    contour1Display.GlyphTableIndexArray = 'Field 0'
    contour1Display.GaussianRadius = 992.5312201746688
    contour1Display.SetScaleArray = ['POINTS', 'Field 0']
    contour1Display.ScaleTransferFunction = 'Piecewise Function'
    contour1Display.OpacityArray = ['POINTS', 'Field 0']
    contour1Display.OpacityTransferFunction = 'Piecewise Function'
    contour1Display.DataAxesGrid = 'Grid Axes Representation'
    contour1Display.PolarAxes = 'Polar Axes Representation'
    contour1Display.SelectInputVectors = [None, '']
    contour1Display.WriteLog = ''

    # init the 'Piecewise Function' selected for 'ScaleTransferFunction'
    contour1Display.ScaleTransferFunction.Points = [95.42253013333334, 0.0, 0.5, 0.0, 95.43815612792969, 1.0, 0.5, 0.0]

    # init the 'Piecewise Function' selected for 'OpacityTransferFunction'
    contour1Display.OpacityTransferFunction.Points = [95.42253013333334, 0.0, 0.5, 0.0, 95.43815612792969, 1.0, 0.5, 0.0]

    # setup the color legend parameters for each legend in this view

    # get color legend/bar for field0LUT in view renderView2
    field0LUTColorBar = GetScalarBar(field0LUT, renderView2)
    field0LUTColorBar.WindowLocation = 'Any Location'
    field0LUTColorBar.Position = [0.0214335501858737, 0.6336842105263157]
    field0LUTColorBar.Title = 'Field 0'
    field0LUTColorBar.ComponentTitle = ''
    field0LUTColorBar.ScalarBarLength = 0.33000000000000007

    # set color bar visibility
    field0LUTColorBar.Visibility = 1

    # get separate 2D transfer function for 'Field0'
    separate_contour1Display_Field0TF2D = GetTransferFunction2D('Field0', contour1Display, separate=True)

    # get separate color transfer function/color map for 'Field0'
    separate_contour1Display_Field0LUT = GetColorTransferFunction('Field0', contour1Display, separate=True)
    separate_contour1Display_Field0LUT.TransferFunction2D = separate_contour1Display_Field0TF2D
    separate_contour1Display_Field0LUT.RGBPoints = [75.0, 0.231373, 0.298039, 0.752941, 100.0, 0.865003, 0.865003, 0.865003, 125.0, 0.705882, 0.0156863, 0.14902]
    separate_contour1Display_Field0LUT.NumberOfTableValues = 3
    separate_contour1Display_Field0LUT.ScalarRangeInitialized = 1.0

    # get color legend/bar for separate_contour1Display_Field0LUT in view renderView2
    separate_contour1Display_Field0LUTColorBar = GetScalarBar(separate_contour1Display_Field0LUT, renderView2)
    separate_contour1Display_Field0LUTColorBar.WindowLocation = 'Any Location'
    separate_contour1Display_Field0LUTColorBar.Position = [0.020910780669144986, 0.21140350877192982]
    separate_contour1Display_Field0LUTColorBar.Title = 'Field 0'
    separate_contour1Display_Field0LUTColorBar.ComponentTitle = ''
    separate_contour1Display_Field0LUTColorBar.ScalarBarLength = 0.3300000000000002

    # set color bar visibility
    separate_contour1Display_Field0LUTColorBar.Visibility = 0

    # hide data in view
    Hide(topbottomvtk, renderView2)

    # hide data in view
    Hide(tableToPoints1, renderView2)

    # hide data in view
    Hide(tetrahedralize1, renderView2)

    # hide data in view
    Hide(pointDatatoCellData1, renderView2)

    # show color legend
    appendAttributes1Display.SetScalarBarVisibility(renderView2, True)

    # hide data in view
    Hide(appendAttributes1, renderView2)

    # show color legend
    slice1Display.SetScalarBarVisibility(renderView2, True)

    # hide data in view
    Hide(slice1, renderView2)

    # show color legend
    cellDatatoPointData1Display.SetScalarBarVisibility(renderView2, True)

    # ----------------------------------------------------------------
    # setup color maps and opacity maps used in the visualization
    # note: the Get..() functions create a new object, if needed
    # ----------------------------------------------------------------

    # get separate opacity transfer function/opacity map for 'Field0'
    separate_contour1Display_Field0PWF = GetOpacityTransferFunction('Field0', contour1Display, separate=True)
    separate_contour1Display_Field0PWF.Points = [75.0, 0.0, 0.5, 0.0, 125.0, 1.0, 0.5, 0.0]
    separate_contour1Display_Field0PWF.ScalarRangeInitialized = 1

    # ----------------------------------------------------------------
    # setup animation scene, tracks and keyframes
    # note: the Get..() functions create a new object, if needed
    # ----------------------------------------------------------------

    # get time animation track
    timeAnimationCue1 = GetTimeTrack()

    # initialize the animation scene

    # get the time-keeper
    timeKeeper1 = GetTimeKeeper()

    # initialize the timekeeper

    # initialize the animation track

    # get animation scene
    animationScene1 = GetAnimationScene()

    # initialize the animation scene
    animationScene1.ViewModules = renderView2
    animationScene1.Cues = timeAnimationCue1
    animationScene1.AnimationTime = 0.0

    # ----------------------------------------------------------------
    # restore active source
    SetActiveSource(contour1)
    # ----------------------------------------------------------------

    # To save a specific target resolution, rather than using the
    # the current view (or layout) size, and override the color palette.
    directory = script_dir+'/../../'+project_folder+'/_fig'
    if not os.path.exists(directory): 
	    os.mkdir(directory)

    if fix_depth:
        SaveScreenshot(directory+'/'+sim_name+'_sec'+str(z_value)+'.png', renderView2, FontScaling=True, OverrideColorPalette='WhiteBackground', TransparentBackground=False)
    else:
        SaveScreenshot(directory+'/'+sim_name+'_sec'+sys.argv[1]+'.png', renderView2, FontScaling=True, OverrideColorPalette='WhiteBackground', TransparentBackground=False)

    ##--------------------------------------------
    ## You may need to add some code at the end of this python script depending on your usage, eg:
    #
    ## Render all views to see them appears
    # RenderAllViews()
    #
    ## Interact with the view, usefull when running from pvpython
    # Interact()
    #
    ## Save a screenshot of the active view
    # SaveScreenshot("path/to/screenshot.png")
    #
    ## Save a screenshot of a layout (multiple splitted view)
    # SaveScreenshot("path/to/screenshot.png", GetLayout())
    #
    ## Save all "Extractors" from the pipeline browser
    # SaveExtracts()
    #
    ## Save a animation of the current active view
    # SaveAnimation()
    #
    ## Please refer to the documentation of paraview.simple
    ## https://www.paraview.org/paraview-docs/latest/python/paraview.simple.html
    ##--------------------------------------------
