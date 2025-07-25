import os

# state file generated using paraview version 5.13.2
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
renderView1.ViewSize = [1612, 811]
renderView1.AxesGrid = 'Grid Axes 3D Actor'
renderView1.StereoType = 'Crystal Eyes'
renderView1.CameraFocalDisk = 1.0
renderView1.LegendGrid = 'Legend Grid Actor'
renderView1.PolarGrid = 'Polar Grid Actor'
renderView1.BackEnd = 'OSPRay raycaster'
renderView1.OSPRayMaterialLibrary = materialLibrary1

# Create a new 'Render View'
renderView2 = CreateView('RenderView')
renderView2.ViewSize = [1404, 811]
renderView2.AxesGrid = 'Grid Axes 3D Actor'
renderView2.CenterOfRotation = [508456.0, 4991050.0, -15570.0]
renderView2.StereoType = 'Crystal Eyes'
renderView2.CameraPosition = [509129.0307915306, 4427284.315901938, 46079.03991803636]
renderView2.CameraFocalPoint = [508455.99999999953, 4991050.0, -15569.999999994225]
renderView2.CameraViewUp = [-0.08308642518167866, 0.1082303529681174, 0.9906476854295507]
renderView2.CameraFocalDisk = 1.0
renderView2.CameraParallelScale = 146783.2160023754
renderView2.LegendGrid = 'Legend Grid Actor'
renderView2.PolarGrid = 'Polar Grid Actor'
renderView2.BackEnd = 'OSPRay raycaster'
renderView2.OSPRayMaterialLibrary = materialLibrary1

# init the 'Grid Axes 3D Actor' selected for 'AxesGrid'
renderView2.AxesGrid.Visibility = 1
renderView2.AxesGrid.DataScale = [1.0, 1.0, 5.0]

SetActiveView(None)

# ----------------------------------------------------------------
# setup view layouts
# ----------------------------------------------------------------

# create new layout object 'Layout #1'
layout1 = CreateLayout(name='Layout #1')
layout1.AssignView(0, renderView2)
layout1.SetSize(1404, 811)

# create new layout object 'Layout #1'
layout1_1 = CreateLayout(name='Layout #1')
layout1_1.AssignView(0, renderView1)
layout1_1.SetSize(1612, 811)

# ----------------------------------------------------------------
# restore active view
SetActiveView(renderView2)
# ----------------------------------------------------------------

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
#stats_name='mean'
for stats_name in stats_name_list:
    sim_name=var_name+'_'+stats_name

    csv_file=script_dir+'/../../'+compute_folder+'/'+compute_sim+space+'/'+sim_name+'.csv'
    print(csv_file)


    samplescsv_file=script_dir+'/../data/samples.csv'


    # ----------------------------------------------------------------
    # setup the data processing pipelines
    # ----------------------------------------------------------------

    # create a new 'CSV Reader'
    t_meancsv = CSVReader(registrationName='T_mean.csv', FileName=[csv_file])
    t_meancsv.UseStringDelimiter = 0
    t_meancsv.HaveHeaders = 0
    t_meancsv.FieldDelimiterCharacters = ''
    t_meancsv.AddTabFieldDelimiter = 1

    # create a new 'Table To Points'
    tableToPoints2 = TableToPoints(registrationName='TableToPoints2', Input=t_meancsv)
    tableToPoints2.XColumn = 'Field 0'
    tableToPoints2.YColumn = 'Field 0'
    tableToPoints2.ZColumn = 'Field 0'
    tableToPoints2.KeepAllDataArrays = 1

    # create a new 'Tetrahedralize'
    tetrahedralize1 = Tetrahedralize(registrationName='Tetrahedralize1', Input=tableToPoints2)

    # create a new 'Point Data to Cell Data'
    pointDatatoCellData1 = PointDatatoCellData(registrationName='PointDatatoCellData1', Input=tetrahedralize1)
    pointDatatoCellData1.PointDataArraytoprocess = ['Field 0']
    pointDatatoCellData1.PassPointData = 1

    # create a new 'Legacy VTK Reader'
    topbottomvtk = LegacyVTKReader(registrationName='top-bottom.vtk', FileNames=[vtk_file])

    # create a new 'Transform'
    transform1 = Transform(registrationName='Transform1', Input=topbottomvtk)
    transform1.Transform = 'Transform'

    # init the 'Transform' selected for 'Transform'
    transform1.Transform.Scale = [1.0, 1.0, 5.0]

    # create a new 'Append Attributes'
    appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[transform1, pointDatatoCellData1])

    # create a new 'Slice'
    slice7000 = Slice(registrationName='Slice7000', Input=appendAttributes1)
    slice7000.SliceType = 'Plane'
    slice7000.HyperTreeGridSlicer = 'Plane'
    slice7000.SliceOffsetValues = [0.0]
    slice7000.PointMergeMethod = 'Uniform Binning'

    # init the 'Plane' selected for 'SliceType'
    slice7000.SliceType.Origin = [508514.5, 4991355.0, -35000.0]
    slice7000.SliceType.Normal = [0.0, 0.0, 1.0]

    # init the 'Plane' selected for 'HyperTreeGridSlicer'
    slice7000.HyperTreeGridSlicer.Origin = [508514.5, 4991355.0, -18412.5]

    # create a new 'Slice'
    slice3000 = Slice(registrationName='Slice3000', Input=appendAttributes1)
    slice3000.SliceType = 'Plane'
    slice3000.HyperTreeGridSlicer = 'Plane'
    slice3000.SliceOffsetValues = [0.0]
    slice3000.PointMergeMethod = 'Uniform Binning'

    # init the 'Plane' selected for 'SliceType'
    slice3000.SliceType.Origin = [508514.5, 4991355.0, -15000.0]
    slice3000.SliceType.Normal = [0.0, 0.0, 1.0]

    # init the 'Plane' selected for 'HyperTreeGridSlicer'
    slice3000.HyperTreeGridSlicer.Origin = [508514.5, 4991355.0, -18412.5]

    # create a new 'Slice'
    slice6000 = Slice(registrationName='Slice6000', Input=appendAttributes1)
    slice6000.SliceType = 'Plane'
    slice6000.HyperTreeGridSlicer = 'Plane'
    slice6000.SliceOffsetValues = [0.0]
    slice6000.PointMergeMethod = 'Uniform Binning'

    # init the 'Plane' selected for 'SliceType'
    slice6000.SliceType.Origin = [508514.5, 4991355.0, -30000.0]
    slice6000.SliceType.Normal = [0.0, 0.0, 1.0]

    # init the 'Plane' selected for 'HyperTreeGridSlicer'
    slice6000.HyperTreeGridSlicer.Origin = [508514.5, 4991355.0, -18412.5]

    # create a new 'Slice'
    mALBRI = Slice(registrationName='MAL-BRI', Input=appendAttributes1)
    mALBRI.SliceType = 'Plane'
    mALBRI.HyperTreeGridSlicer = 'Plane'
    mALBRI.SliceOffsetValues = [0.0]
    mALBRI.PointMergeMethod = 'Uniform Binning'

    # init the 'Plane' selected for 'SliceType'
    mALBRI.SliceType.Origin = [544087.0, 5038574.0, 112.0]
    mALBRI.SliceType.Normal = [38980102.0, -44711916.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridSlicer'
    mALBRI.HyperTreeGridSlicer.Origin = [508514.5, 4991355.0, -18412.5]

    # create a new 'Clip'
    mALBRIcut1 = Clip(registrationName='MAL-BRI cut1', Input=mALBRI)
    mALBRIcut1.ClipType = 'Plane'
    mALBRIcut1.HyperTreeGridClipper = 'Plane'
    mALBRIcut1.Scalars = ['CELLS', 'Field 0']
    mALBRIcut1.Value = 93.2547868

    # init the 'Plane' selected for 'ClipType'
    mALBRIcut1.ClipType.Origin = [551209.0, 5044783.0, 138.0]
    mALBRIcut1.ClipType.Normal = [44711916.0, 38980102.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridClipper'
    mALBRIcut1.HyperTreeGridClipper.Origin = [514651.421875, 5012911.75, -15645.0]

    # create a new 'Clip'
    mALBRIfinale = Clip(registrationName='MAL-BRI finale', Input=mALBRIcut1)
    mALBRIfinale.ClipType = 'Plane'
    mALBRIfinale.HyperTreeGridClipper = 'Plane'
    mALBRIfinale.Scalars = ['CELLS', 'Field 0']
    mALBRIfinale.Value = 90.1514238
    mALBRIfinale.Invert = 0

    # init the 'Plane' selected for 'ClipType'
    mALBRIfinale.ClipType.Origin = [544087.0, 5038574.0, 112.0]
    mALBRIfinale.ClipType.Normal = [59666112.0, -48164816.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridClipper'
    mALBRIfinale.HyperTreeGridClipper.Origin = [506995.734375, 5006237.5, -15645.0]

    # create a new 'Calculator'
    calculator7 = Calculator(registrationName='Calculator7', Input=mALBRIfinale)
    calculator7.ResultTCoords = 1
    calculator7.Function = ''

    # create a new 'Cell Data to Point Data'
    cellDatatoPointData7 = CellDatatoPointData(registrationName='CellDatatoPointData7', Input=calculator7)

    # create a new 'Slice'
    mORBAT = Slice(registrationName='MOR-BAT', Input=appendAttributes1)
    mORBAT.SliceType = 'Plane'
    mORBAT.HyperTreeGridSlicer = 'Plane'
    mORBAT.SliceOffsetValues = [0.0]
    mORBAT.PointMergeMethod = 'Uniform Binning'

    # init the 'Plane' selected for 'SliceType'
    mORBAT.SliceType.Origin = [477546.0, 5014218.0, 113.0]
    mORBAT.SliceType.Normal = [9421002.0, 165130708.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridSlicer'
    mORBAT.HyperTreeGridSlicer.Origin = [508514.5, 4991355.0, -18412.5]

    # create a new 'Clip'
    mORBATcut1 = Clip(registrationName='MOR-BAT cut1', Input=mORBAT)
    mORBATcut1.ClipType = 'Plane'
    mORBATcut1.HyperTreeGridClipper = 'Plane'
    mORBATcut1.Scalars = ['CELLS', 'Field 0']
    mORBATcut1.Value = 93.1215196
    mORBATcut1.Invert = 0

    # init the 'Plane' selected for 'ClipType'
    mORBATcut1.ClipType.Origin = [477546.0, 5014218.0, 113.0]
    mORBATcut1.ClipType.Normal = [13957040.0, 172812994.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridClipper'
    mORBATcut1.HyperTreeGridClipper.Origin = [521274.875, 5011723.0, -15645.0]

    # create a new 'Clip'
    mORBATfinale = Clip(registrationName='MOR-BAT finale', Input=mORBATcut1)
    mORBATfinale.ClipType = 'Plane'
    mORBATfinale.HyperTreeGridClipper = 'Plane'
    mORBATfinale.Scalars = ['CELLS', 'Field 0']
    mORBATfinale.Value = 93.62090485

    # init the 'Plane' selected for 'ClipType'
    mORBATfinale.ClipType.Origin = [505468.0, 5012625.0, 86.0]
    mORBATfinale.ClipType.Normal = [66526410.0, 67061268.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridClipper'
    mORBATfinale.HyperTreeGridClipper.Origin = [552633.625, 5009934.25, -15645.0]

    # create a new 'Calculator'
    calculator2 = Calculator(registrationName='Calculator2', Input=mORBATfinale)
    calculator2.AttributeType = 'Cell Data'
    calculator2.Function = ''

    # create a new 'Cell Data to Point Data'
    cellDatatoPointData2 = CellDatatoPointData(registrationName='CellDatatoPointData2', Input=calculator2)
    cellDatatoPointData2.CellDataArraytoprocess = ['Field 0', 'hex_selector']
    
    # create a new 'Plot Over Line'
    #plotOverLine1 = PlotOverLine(registrationName='PlotOverLine1', Input=mORBATfinale)
    #plotOverLine1.Point1 = [491506.99999999255, 5013421.5, -35820.0]
    #plotOverLine1.Point2 = [491506.99999999255, 5013421.5, 180.0]

    # create a new 'Slice'
    slice4000 = Slice(registrationName='Slice4000', Input=appendAttributes1)
    slice4000.SliceType = 'Plane'
    slice4000.HyperTreeGridSlicer = 'Plane'
    slice4000.SliceOffsetValues = [0.0]
    slice4000.PointMergeMethod = 'Uniform Binning'

    # init the 'Plane' selected for 'SliceType'
    slice4000.SliceType.Origin = [508514.5, 4991355.0, -20000.0]
    slice4000.SliceType.Normal = [0.0, 0.0, 1.0]

    # init the 'Plane' selected for 'HyperTreeGridSlicer'
    slice4000.HyperTreeGridSlicer.Origin = [508514.5, 4991355.0, -18412.5]

    # create a new 'Slice'
    slice5000 = Slice(registrationName='Slice5000', Input=appendAttributes1)
    slice5000.SliceType = 'Plane'
    slice5000.HyperTreeGridSlicer = 'Plane'
    slice5000.SliceOffsetValues = [0.0]
    slice5000.PointMergeMethod = 'Uniform Binning'

    # init the 'Plane' selected for 'SliceType'
    slice5000.SliceType.Origin = [508514.5, 4991355.0, -25000.0]
    slice5000.SliceType.Normal = [0.0, 0.0, 1.0]

    # init the 'Plane' selected for 'HyperTreeGridSlicer'
    slice5000.HyperTreeGridSlicer.Origin = [508514.5, 4991355.0, -18412.5]

    # create a new 'Slice'
    pANMAL = Slice(registrationName='PAN-MAL', Input=appendAttributes1)
    pANMAL.SliceType = 'Plane'
    pANMAL.HyperTreeGridSlicer = 'Plane'
    pANMAL.SliceOffsetValues = [0.0]
    pANMAL.PointMergeMethod = 'Uniform Binning'

    # init the 'Plane' selected for 'SliceType'
    pANMAL.SliceType.Origin = [544087.0, 5038574.0, 112.0]
    pANMAL.SliceType.Normal = [59666112.0, -48164816.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridSlicer'
    pANMAL.HyperTreeGridSlicer.Origin = [508514.5, 4991355.0, -18412.5]

    # create a new 'Clip'
    pANMALcut1 = Clip(registrationName='PAN-MAL cut1', Input=pANMAL)
    pANMALcut1.ClipType = 'Plane'
    pANMALcut1.HyperTreeGridClipper = 'Plane'
    pANMALcut1.Scalars = ['CELLS', 'Field 0']
    pANMALcut1.Value = 91.30088310500001
    pANMALcut1.Invert = 0

    # init the 'Plane' selected for 'ClipType'
    pANMALcut1.ClipType.Origin = [536415.0, 5029070.0, 100.0]
    pANMALcut1.ClipType.Normal = [39803860.0, 2341237.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridClipper'
    pANMALcut1.HyperTreeGridClipper.Origin = [520706.640625, 5009610.75, -15645.0]

    # create a new 'Clip'
    pANMALfinale = Clip(registrationName='PAN-MAL finale', Input=pANMALcut1)
    pANMALfinale.ClipType = 'Plane'
    pANMALfinale.HyperTreeGridClipper = 'Plane'
    pANMALfinale.Scalars = ['CELLS', 'Field 0']
    pANMALfinale.Value = 91.8668688
    pANMALfinale.Invert = 0

    # init the 'Plane' selected for 'ClipType'
    pANMALfinale.ClipType.Origin = [544087.0, 5038574.0, 112.0]
    pANMALfinale.ClipType.Normal = [38980102.0, -44711916.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridClipper'
    pANMALfinale.HyperTreeGridClipper.Origin = [548863.625, 5044491.0, -15645.0]

    # create a new 'Calculator'
    calculator6 = Calculator(registrationName='Calculator6', Input=pANMALfinale)
    calculator6.ResultTCoords = 1
    calculator6.Function = ''

    # create a new 'Cell Data to Point Data'
    cellDatatoPointData6 = CellDatatoPointData(registrationName='CellDatatoPointData6', Input=calculator6)

    # create a new 'Slice'
    cORPAN = Slice(registrationName='COR-PAN', Input=appendAttributes1)
    cORPAN.SliceType = 'Plane'
    cORPAN.HyperTreeGridSlicer = 'Plane'
    cORPAN.SliceOffsetValues = [0.0]
    cORPAN.PointMergeMethod = 'Uniform Binning'

    # init the 'Plane' selected for 'SliceType'
    cORPAN.SliceType.Origin = [537242.0, 5015010.0, 78.0]
    cORPAN.SliceType.Normal = [39803860.0, 2341237.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridSlicer'
    cORPAN.HyperTreeGridSlicer.Origin = [508514.5, 4991355.0, -18412.5]

    # create a new 'Clip'
    cORPANcut1 = Clip(registrationName='COR-PAN cut1', Input=cORPAN)
    cORPANcut1.ClipType = 'Plane'
    cORPANcut1.HyperTreeGridClipper = 'Plane'
    cORPANcut1.Scalars = ['POINTS', '']
    cORPANcut1.Invert = 0

    # init the 'Plane' selected for 'ClipType'
    cORPANcut1.ClipType.Origin = [537242.0, 5015010.0, 78.0]
    cORPANcut1.ClipType.Normal = [40907950.0, 55521572.0, 0.0]

    # create a new 'Clip'
    cORPANfinale = Clip(registrationName='COR-PAN finale', Input=cORPANcut1)
    cORPANfinale.ClipType = 'Plane'
    cORPANfinale.HyperTreeGridClipper = 'Plane'
    cORPANfinale.Scalars = ['CELLS', 'Field 0']
    cORPANfinale.Value = 95.3351437
    cORPANfinale.Invert = 0

    # init the 'Plane' selected for 'ClipType'
    cORPANfinale.ClipType.Origin = [536415.0, 5029070.0, 100.0]
    cORPANfinale.ClipType.Normal = [59666112.0, -48164816.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridClipper'
    cORPANfinale.HyperTreeGridClipper.Origin = [535649.875, 5042077.75, -15645.0]

    # create a new 'Calculator'
    calculator5 = Calculator(registrationName='Calculator5', Input=cORPANfinale)
    calculator5.ResultTCoords = 1
    calculator5.Function = ''

    # create a new 'Cell Data to Point Data'
    cellDatatoPointData5 = CellDatatoPointData(registrationName='CellDatatoPointData5', Input=calculator5)

    # create a new 'Slice'
    vSCOR = Slice(registrationName='VS-COR', Input=appendAttributes1)
    vSCOR.SliceType = 'Plane'
    vSCOR.HyperTreeGridSlicer = 'Plane'
    vSCOR.SliceOffsetValues = [0.0]
    vSCOR.PointMergeMethod = 'Uniform Binning'

    # init the 'Plane' selected for 'SliceType'
    vSCOR.SliceType.Origin = [537242.0, 5015010.0, 78.0]
    vSCOR.SliceType.Normal = [40907950.0, -55521572.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridSlicer'
    vSCOR.HyperTreeGridSlicer.Origin = [508514.5, 4991355.0, -18412.5]

    # create a new 'Clip'
    vSCORcut1 = Clip(registrationName='VS-COR cut1', Input=vSCOR)
    vSCORcut1.ClipType = 'Plane'
    vSCORcut1.HyperTreeGridClipper = 'Plane'
    vSCORcut1.Scalars = ['POINTS', '']
    vSCORcut1.Invert = 0

    # init the 'Plane' selected for 'ClipType'
    vSCORcut1.ClipType.Origin = [517630.0, 5000560.0, 80.0]
    vSCORcut1.ClipType.Normal = [0.7042700240428484, 0.7099322032664005, 0.0]

    # create a new 'Clip'
    vSCORfinale = Clip(registrationName='VS-COR finale', Input=vSCORcut1)
    vSCORfinale.ClipType = 'Plane'
    vSCORfinale.HyperTreeGridClipper = 'Plane'
    vSCORfinale.Scalars = ['CELLS', 'Field 0']
    vSCORfinale.Value = 91.96136095

    # init the 'Plane' selected for 'ClipType'
    vSCORfinale.ClipType.Origin = [537242.0, 5015010.0, 78.0]
    vSCORfinale.ClipType.Normal = [39803860.0, 2341237.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridClipper'
    vSCORfinale.HyperTreeGridClipper.Origin = [545863.015625, 5021362.0, -15645.0]

    # create a new 'Calculator'
    calculator4 = Calculator(registrationName='Calculator4', Input=vSCORfinale)
    calculator4.AttributeType = 'Cell Data'
    calculator4.ResultTCoords = 1
    calculator4.Function = ''

    # create a new 'Cell Data to Point Data'
    cellDatatoPointData4 = CellDatatoPointData(registrationName='CellDatatoPointData4', Input=calculator4)

    # create a new 'Slice'
    sVMOR = Slice(registrationName='SV-MOR', Input=appendAttributes1)
    sVMOR.SliceType = 'Plane'
    sVMOR.HyperTreeGridSlicer = 'Plane'
    sVMOR.SliceOffsetValues = [0.0]
    sVMOR.PointMergeMethod = 'Uniform Binning'

    # init the 'Plane' selected for 'SliceType'
    sVMOR.SliceType.Origin = [448325.0, 5016578.0, 137.0]
    sVMOR.SliceType.Normal = [13957040.0, 172812994.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridSlicer'
    sVMOR.HyperTreeGridSlicer.Origin = [508514.5, 4991355.0, -18412.5]

    # create a new 'Clip'
    sVMORcut1 = Clip(registrationName='SV-MOR cut1', Input=sVMOR)
    sVMORcut1.ClipType = 'Plane'
    sVMORcut1.HyperTreeGridClipper = 'Plane'
    sVMORcut1.Scalars = ['CELLS', 'Field 0']
    sVMORcut1.Value = 92.88098978500001

    # init the 'Plane' selected for 'ClipType'
    sVMORcut1.ClipType.Origin = [448325.0, 5016578.0, 137.0]
    sVMORcut1.ClipType.Normal = [-172812994.0, 13957040.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridClipper'
    sVMORcut1.HyperTreeGridClipper.Origin = [521421.765625, 5010674.25, -15645.0]

    # create a new 'Clip'
    sVMORfinale = Clip(registrationName='SV-MOR finale', Input=sVMORcut1)
    sVMORfinale.ClipType = 'Plane'
    sVMORfinale.HyperTreeGridClipper = 'Plane'
    sVMORfinale.Scalars = ['CELLS', 'Field 0']
    sVMORfinale.Value = 93.4523412
    sVMORfinale.Invert = 0

    # init the 'Plane' selected for 'ClipType'
    sVMORfinale.ClipType.Origin = [477546.0, 5014218.0, 113.0]
    sVMORfinale.ClipType.Normal = [9421002.0, 165130708.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridClipper'
    sVMORfinale.HyperTreeGridClipper.Origin = [537385.703125, 5009385.0, -15645.0]

    # create a new 'Calculator'
    calculator1 = Calculator(registrationName='Calculator1', Input=sVMORfinale)
    calculator1.AttributeType = 'Cell Data'
    calculator1.ResultTCoords = 1
    calculator1.Function = ''

    # create a new 'Cell Data to Point Data'
    cellDatatoPointData1 = CellDatatoPointData(registrationName='CellDatatoPointData1', Input=calculator1)

    # create a new 'Slice'
    bATVS = Slice(registrationName='BAT-VS', Input=appendAttributes1)
    bATVS.SliceType = 'Plane'
    bATVS.HyperTreeGridSlicer = 'Plane'
    bATVS.SliceOffsetValues = [0.0]
    bATVS.PointMergeMethod = 'Uniform Binning'

    # init the 'Plane' selected for 'SliceType'
    bATVS.SliceType.Origin = [505468.0, 5012625.0, 86.0]
    bATVS.SliceType.Normal = [66526410.0, 67061268.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridSlicer'
    bATVS.HyperTreeGridSlicer.Origin = [508514.5, 4991355.0, -18412.5]

    # create a new 'Clip'
    bATVScut1 = Clip(registrationName='BAT-VS cut1', Input=bATVS)
    bATVScut1.ClipType = 'Plane'
    bATVScut1.HyperTreeGridClipper = 'Plane'
    bATVScut1.Scalars = ['CELLS', 'Field 0']
    bATVScut1.Value = 99.886144765

    # init the 'Plane' selected for 'ClipType'
    bATVScut1.ClipType.Origin = [505468.0, 5012625.0, 86.0]
    bATVScut1.ClipType.Normal = [9421002.0, 165130708.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridClipper'
    bATVScut1.HyperTreeGridClipper.Origin = [531100.265625, 4987197.0, -15645.0]

    # create a new 'Clip'
    bATVSfinale = Clip(registrationName='BAT-VS finale', Input=bATVScut1)
    bATVSfinale.ClipType = 'Plane'
    bATVSfinale.HyperTreeGridClipper = 'Plane'
    bATVSfinale.Scalars = ['CELLS', 'Field 0']
    bATVSfinale.Value = 83.732929265

    # init the 'Plane' selected for 'ClipType'
    bATVSfinale.ClipType.Origin = [517630.0, 5000560.0, 78.0]
    bATVSfinale.ClipType.Normal = [40907950.0, -55521572.0, 0.0]

    # init the 'Plane' selected for 'HyperTreeGridClipper'
    bATVSfinale.HyperTreeGridClipper.Origin = [553678.703125, 4964798.5, -15645.0]

    # create a new 'Calculator'
    calculator3 = Calculator(registrationName='Calculator3', Input=bATVSfinale)
    calculator3.ResultTCoords = 1
    calculator3.Function = ''

    # create a new 'Cell Data to Point Data'
    cellDatatoPointData3 = CellDatatoPointData(registrationName='CellDatatoPointData3', Input=calculator3)

    # ----------------------------------------------------------------
    # setup the visualization in view 'renderView2'
    # ----------------------------------------------------------------

    # show data from appendAttributes1
    appendAttributes1Display = Show(appendAttributes1, renderView2, 'UnstructuredGridRepresentation')

    # get 2D transfer function for 'Field0'
    field0TF2D = GetTransferFunction2D('Field0')

    # get color transfer function/color map for 'Field0'
    field0LUT = GetColorTransferFunction('Field0')
    field0LUT.TransferFunction2D = field0TF2D
    field0LUT.RGBPoints = [6.63, 0.231373, 0.298039, 0.752941, 98.91, 0.865003, 0.865003, 0.865003, 191.19, 0.705882, 0.0156863, 0.14902]
    field0LUT.ScalarRangeInitialized = 1.0

    # get opacity transfer function/opacity map for 'Field0'
    field0PWF = GetOpacityTransferFunction('Field0')
    field0PWF.Points = [6.63, 0.0, 0.5, 0.0, 191.19, 1.0, 0.5, 0.0]
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
    appendAttributes1Display.SetScaleArray = [None, '']
    appendAttributes1Display.ScaleTransferFunction = 'Piecewise Function'
    appendAttributes1Display.OpacityArray = [None, '']
    appendAttributes1Display.OpacityTransferFunction = 'Piecewise Function'
    appendAttributes1Display.DataAxesGrid = 'Grid Axes Representation'
    appendAttributes1Display.PolarAxes = 'Polar Axes Representation'
    appendAttributes1Display.ScalarOpacityFunction = field0PWF
    appendAttributes1Display.ScalarOpacityUnitDistance = 3356.364569222269
    appendAttributes1Display.OpacityArrayName = ['CELLS', 'Field 0']
    appendAttributes1Display.SelectInputVectors = [None, '']
    appendAttributes1Display.WriteLog = ''

    # show data from mORBAT
    mORBATDisplay = Show(mORBAT, renderView2, 'GeometryRepresentation')

    # trace defaults for the display properties.
    mORBATDisplay.Representation = 'Surface'
    mORBATDisplay.ColorArrayName = ['CELLS', 'Field 0']
    mORBATDisplay.LookupTable = field0LUT
    mORBATDisplay.SelectNormalArray = 'None'
    mORBATDisplay.SelectTangentArray = 'None'
    mORBATDisplay.SelectTCoordArray = 'None'
    mORBATDisplay.TextureTransform = 'Transform2'
    mORBATDisplay.OSPRayScaleFunction = 'Piecewise Function'
    mORBATDisplay.Assembly = ''
    mORBATDisplay.SelectedBlockSelectors = ['']
    mORBATDisplay.SelectOrientationVectors = 'None'
    mORBATDisplay.ScaleFactor = 21500.000000029384
    mORBATDisplay.SelectScaleArray = 'None'
    mORBATDisplay.GlyphType = 'Arrow'
    mORBATDisplay.GlyphTableIndexArray = 'None'
    mORBATDisplay.GaussianRadius = 1075.0000000014693
    mORBATDisplay.SetScaleArray = [None, '']
    mORBATDisplay.ScaleTransferFunction = 'Piecewise Function'
    mORBATDisplay.OpacityArray = [None, '']
    mORBATDisplay.OpacityTransferFunction = 'Piecewise Function'
    mORBATDisplay.DataAxesGrid = 'Grid Axes Representation'
    mORBATDisplay.PolarAxes = 'Polar Axes Representation'
    mORBATDisplay.SelectInputVectors = [None, '']
    mORBATDisplay.WriteLog = ''

    # show data from mORBATcut1
    mORBATcut1Display = Show(mORBATcut1, renderView2, 'UnstructuredGridRepresentation')

    # trace defaults for the display properties.
    mORBATcut1Display.Representation = 'Surface'
    mORBATcut1Display.ColorArrayName = ['CELLS', 'Field 0']
    mORBATcut1Display.LookupTable = field0LUT
    mORBATcut1Display.SelectNormalArray = 'None'
    mORBATcut1Display.SelectTangentArray = 'None'
    mORBATcut1Display.SelectTCoordArray = 'None'
    mORBATcut1Display.TextureTransform = 'Transform2'
    mORBATcut1Display.OSPRayScaleFunction = 'Piecewise Function'
    mORBATcut1Display.Assembly = ''
    mORBATcut1Display.SelectedBlockSelectors = ['']
    mORBATcut1Display.SelectOrientationVectors = 'None'
    mORBATcut1Display.ScaleFactor = 15191.000000001095
    mORBATcut1Display.SelectScaleArray = 'None'
    mORBATcut1Display.GlyphType = 'Arrow'
    mORBATcut1Display.GlyphTableIndexArray = 'None'
    mORBATcut1Display.GaussianRadius = 759.5500000000548
    mORBATcut1Display.SetScaleArray = [None, '']
    mORBATcut1Display.ScaleTransferFunction = 'Piecewise Function'
    mORBATcut1Display.OpacityArray = [None, '']
    mORBATcut1Display.OpacityTransferFunction = 'Piecewise Function'
    mORBATcut1Display.DataAxesGrid = 'Grid Axes Representation'
    mORBATcut1Display.PolarAxes = 'Polar Axes Representation'
    mORBATcut1Display.ScalarOpacityFunction = field0PWF
    mORBATcut1Display.ScalarOpacityUnitDistance = 7924.997500169576
    mORBATcut1Display.OpacityArrayName = ['CELLS', 'Field 0']
    mORBATcut1Display.SelectInputVectors = [None, '']
    mORBATcut1Display.WriteLog = ''

    # show data from cellDatatoPointData2
    cellDatatoPointData2Display = Show(cellDatatoPointData2, renderView2, 'UnstructuredGridRepresentation')

    # trace defaults for the display properties.
    cellDatatoPointData2Display.Representation = 'Surface'
    cellDatatoPointData2Display.ColorArrayName = [None, '']
    cellDatatoPointData2Display.SelectNormalArray = 'None'
    cellDatatoPointData2Display.SelectTangentArray = 'None'
    cellDatatoPointData2Display.SelectTCoordArray = 'None'
    cellDatatoPointData2Display.TextureTransform = 'Transform2'
    cellDatatoPointData2Display.OSPRayScaleArray = 'Field 0'
    cellDatatoPointData2Display.OSPRayScaleFunction = 'Piecewise Function'
    cellDatatoPointData2Display.Assembly = ''
    cellDatatoPointData2Display.SelectedBlockSelectors = ['']
    cellDatatoPointData2Display.SelectOrientationVectors = 'None'
    cellDatatoPointData2Display.ScaleFactor = 3600.0
    cellDatatoPointData2Display.SelectScaleArray = 'Field 0'
    cellDatatoPointData2Display.GlyphType = 'Arrow'
    cellDatatoPointData2Display.GlyphTableIndexArray = 'Field 0'
    cellDatatoPointData2Display.GaussianRadius = 180.0
    cellDatatoPointData2Display.SetScaleArray = ['POINTS', 'Field 0']
    cellDatatoPointData2Display.ScaleTransferFunction = 'Piecewise Function'
    cellDatatoPointData2Display.OpacityArray = ['POINTS', 'Field 0']
    cellDatatoPointData2Display.OpacityTransferFunction = 'Piecewise Function'
    cellDatatoPointData2Display.DataAxesGrid = 'Grid Axes Representation'
    cellDatatoPointData2Display.PolarAxes = 'Polar Axes Representation'
    cellDatatoPointData2Display.ScalarOpacityUnitDistance = 4036.952480006786
    cellDatatoPointData2Display.OpacityArrayName = ['POINTS', 'Field 0']
    cellDatatoPointData2Display.SelectInputVectors = [None, '']
    cellDatatoPointData2Display.WriteLog = ''

    # init the 'Piecewise Function' selected for 'ScaleTransferFunction'
    cellDatatoPointData2Display.ScaleTransferFunction.Points = [15.276666666666666, 0.0, 0.5, 0.0, 168.17, 1.0, 0.5, 0.0]

    # init the 'Piecewise Function' selected for 'OpacityTransferFunction'
    cellDatatoPointData2Display.OpacityTransferFunction.Points = [15.276666666666666, 0.0, 0.5, 0.0, 168.17, 1.0, 0.5, 0.0]

    # setup the color legend parameters for each legend in this view

    # get color legend/bar for field0LUT in view renderView2
    field0LUTColorBar = GetScalarBar(field0LUT, renderView2)
    field0LUTColorBar.Position = [0.9173789173789174, 0.014796547472256474]
    field0LUTColorBar.Title = 'Field 0'
    field0LUTColorBar.ComponentTitle = ''

    # set color bar visibility
    field0LUTColorBar.Visibility = 1

    # show color legend
    appendAttributes1Display.SetScalarBarVisibility(renderView2, True)

    # show color legend
    mORBATDisplay.SetScalarBarVisibility(renderView2, True)

    # show color legend
    mORBATcut1Display.SetScalarBarVisibility(renderView2, True)

    # ----------------------------------------------------------------
    # setup color maps and opacity maps used in the visualization
    # note: the Get..() functions create a new object, if needed
    # ----------------------------------------------------------------

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
    animationScene1.ViewModules = [renderView1, renderView2]
    animationScene1.Cues = timeAnimationCue1
    animationScene1.AnimationTime = 0.0

    # ----------------------------------------------------------------
    # restore active source
    SetActiveSource(cellDatatoPointData2)
    # ----------------------------------------------------------------


    # Step 5: Export as CSV using ParaView's built-in functionality
    output_file=script_dir+"/MOR_BATprofile_"+var_name+stats_name+".csv"
    #SaveData(output_file, proxy=plotOverLine1, Precision=10, FieldAssociation="Point Data", ChooseArraysToWrite=True, PointDataArrays=["Field 0"])
    SaveData(output_file, proxy=cellDatatoPointData2, Precision=10, FieldAssociation="Point Data", ChooseArraysToWrite=True, PointDataArrays=["Field 0"])
    print("CSV file saved successfully!")
    
print("End loop on file!")




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
