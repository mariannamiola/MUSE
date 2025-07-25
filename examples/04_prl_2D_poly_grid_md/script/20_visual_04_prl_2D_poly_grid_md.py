# state file generated using paraview version 5.10.0-RC1

# uncomment the following three lines to ensure this script works in future versions
#import paraview
#paraview.compatibility.major = 5
#paraview.compatibility.minor = 10

import sys
import os

###frame=sys.argv[1]

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

# ----------------------------------------------------------------
# setup views used in the visualization
# ----------------------------------------------------------------

# Create a new 'Render View'
renderView1 = CreateView('RenderView')
renderView1.ViewSize = [1365, 784]
renderView1.InteractionMode = '2D'
renderView1.AxesGrid = 'GridAxes3DActor'
renderView1.CenterOfRotation = [812736.71875, 4719403.25, 0.0]
renderView1.StereoType = 'Crystal Eyes'
renderView1.CameraPosition = [787620.5889933391, 4643905.715066378, 10000.0]
renderView1.CameraFocalPoint = [787620.5889933391, 4643905.715066378, 0.0]
renderView1.CameraFocalDisk = 1.0
renderView1.CameraParallelScale = 584651.0014614653
renderView1.Background = [0.32, 0.34, 0.43]

SetActiveView(None)

# ----------------------------------------------------------------
# setup view layouts
# ----------------------------------------------------------------

# create new layout object 'Layout #1'
layout1 = CreateLayout(name='Layout #1')
layout1.AssignView(0, renderView1)
layout1.SetSize(1365, 784)

# ----------------------------------------------------------------
# restore active view
SetActiveView(renderView1)
# ----------------------------------------------------------------

# ----------------------------------------------------------------
# setup the data processing pipelines
# ----------------------------------------------------------------

script_dir=os.path.dirname(os.path.realpath(__file__))
#print(script_dir)

work_folder='MUSE_test'
project_name='04_prl_2D_poly_grid_md'

var_name='cr'
sub='F'
vario='OMNI3D'
geom_name='F'


compute_sim1=var_name+'_'+sub+'5TERRE_tri'+'_'+vario+'_'+geom_name+'5TERRE_grid'
compute_sim2=var_name+'_'+sub+'MAGRA_tri'+'_'+vario+'_'+geom_name+'MAGRA_grid'
compute_sim3=var_name+'_'+sub+'PETRONIO_tri'+'_'+vario+'_'+geom_name+'PETRONIO_grid'
compute_sim4=var_name+'_'+sub+'PORTOFINO_tri'+'_'+vario+'_'+geom_name+'PORTOFINO_grid'
compute_sim5=var_name+'_'+sub+'ENTELLA_tri'+'_'+vario+'_'+geom_name+'ENTELLA_grid'
compute_sim6=var_name+'_'+sub+'AVETO_tri'+'_'+vario+'_'+geom_name+'AVETO_grid'
compute_sim7=var_name+'_'+sub+'PADANO_tri'+'_'+vario+'_'+geom_name+'PADANO_grid'
compute_sim8=var_name+'_'+sub+'BISAGNO_tri'+'_'+vario+'_'+geom_name+'BISAGNO_grid'
compute_sim9=var_name+'_'+sub+'POLCEVERA_tri'+'_'+vario+'_'+geom_name+'POLCEVERA_grid'
compute_sim10=var_name+'_'+sub+'ARENZANO_tri'+'_'+vario+'_'+geom_name+'ARENZANO_grid'
compute_sim11=var_name+'_'+sub+'SASSELLO_tri'+'_'+vario+'_'+geom_name+'SASSELLO_grid'
compute_sim12=var_name+'_'+sub+'BORMIDE_tri'+'_'+vario+'_'+geom_name+'BORMIDE_grid'
compute_sim13=var_name+'_'+sub+'SAVONESE_tri'+'_'+vario+'_'+geom_name+'SAVONESE_grid'
compute_sim14=var_name+'_'+sub+'IMPERIESE_tri'+'_'+vario+'_'+geom_name+'IMPERIESE_grid'

project_folder=work_folder +'/'+project_name
geom_folder=project_folder+'/out/geometry/surf'

compute_folder=project_folder+'/out/compute'
##project_append=sys.argv[1]
##compute_folder=compute_folder+'/'+project_append

space='/_varspace'
stats_name='mean'
sim_name=var_name+'_'+stats_name


csv_file1=script_dir+'/../../'+compute_folder+'/'+compute_sim1+space+'/_stats/'+sim_name+'.csv'
obj_file1=script_dir+'/../../'+geom_folder+'/'+geom_name+'5TERRE_grid.obj'

csv_file2=script_dir+'/../../'+compute_folder+'/'+compute_sim2+space+'/_stats/'+sim_name+'.csv'
obj_file2=script_dir+'/../../'+geom_folder+'/'+geom_name+'MAGRA_grid.obj'

csv_file3=script_dir+'/../../'+compute_folder+'/'+compute_sim3+space+'/_stats/'+sim_name+'.csv'
obj_file3=script_dir+'/../../'+geom_folder+'/'+geom_name+'PETRONIO_grid.obj'

csv_file4=script_dir+'/../../'+compute_folder+'/'+compute_sim4+space+'/_stats/'+sim_name+'.csv'
obj_file4=script_dir+'/../../'+geom_folder+'/'+geom_name+'PORTOFINO_grid.obj'

csv_file5=script_dir+'/../../'+compute_folder+'/'+compute_sim5+space+'/_stats/'+sim_name+'.csv'
obj_file5=script_dir+'/../../'+geom_folder+'/'+geom_name+'ENTELLA_grid.obj'

csv_file6=script_dir+'/../../'+compute_folder+'/'+compute_sim6+space+'/_stats/'+sim_name+'.csv'
obj_file6=script_dir+'/../../'+geom_folder+'/'+geom_name+'AVETO_grid.obj'

csv_file7=script_dir+'/../../'+compute_folder+'/'+compute_sim7+space+'/_stats/'+sim_name+'.csv'
obj_file7=script_dir+'/../../'+geom_folder+'/'+geom_name+'PADANO_grid.obj'

csv_file8=script_dir+'/../../'+compute_folder+'/'+compute_sim8+space+'/_stats/'+sim_name+'.csv'
obj_file8=script_dir+'/../../'+geom_folder+'/'+geom_name+'BISAGNO_grid.obj'

csv_file9=script_dir+'/../../'+compute_folder+'/'+compute_sim9+space+'/_stats/'+sim_name+'.csv'
obj_file9=script_dir+'/../../'+geom_folder+'/'+geom_name+'POLCEVERA_grid.obj'

csv_file10=script_dir+'/../../'+compute_folder+'/'+compute_sim10+space+'/_stats/'+sim_name+'.csv'
obj_file10=script_dir+'/../../'+geom_folder+'/'+geom_name+'ARENZANO_grid.obj'

csv_file11=script_dir+'/../../'+compute_folder+'/'+compute_sim11+space+'/_stats/'+sim_name+'.csv'
obj_file11=script_dir+'/../../'+geom_folder+'/'+geom_name+'SASSELLO_grid.obj'

csv_file12=script_dir+'/../../'+compute_folder+'/'+compute_sim12+space+'/_stats/'+sim_name+'.csv'
obj_file12=script_dir+'/../../'+geom_folder+'/'+geom_name+'BORMIDE_grid.obj'

csv_file13=script_dir+'/../../'+compute_folder+'/'+compute_sim13+space+'/_stats/'+sim_name+'.csv'
obj_file13=script_dir+'/../../'+geom_folder+'/'+geom_name+'SAVONESE_grid.obj'

csv_file14=script_dir+'/../../'+compute_folder+'/'+compute_sim14+space+'/_stats/'+sim_name+'.csv'
obj_file14=script_dir+'/../../'+geom_folder+'/'+geom_name+'IMPERIESE_grid.obj'



########################
###1-5TERRE
# create a new 'CSV Reader'
cr1_csv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file1])
cr1_csv.UseStringDelimiter = 0
cr1_csv.HaveHeaders = 0
cr1_csv.FieldDelimiterCharacters = ' '
cr1_csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tTP1 = TableToPoints(registrationName='TableToPoints1', Input=cr1_csv)
tTP1.XColumn = 'Field 0'
tTP1.YColumn = 'Field 0'
tTP1.ZColumn = 'Field 0'
tTP1.a2DPoints = 1
tTP1.KeepAllDataArrays = 1

# create a new 'Triangulate'
tri1 = Triangulate(registrationName='Triangulate1', Input=tTP1)

# create a new 'Point Data to Cell Data'
pDtCD1 = PointDatatoCellData(registrationName='PointDatatoCellData1', Input=tri1)
pDtCD1.PointDataArraytoprocess = ['Field 0']
pDtCD1.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
region1 = WavefrontOBJReader(registrationName=geom_name+'5TERRE_grid.obj', FileName=obj_file1)

# create a new 'Append Attributes'
appendAttributes1 = AppendAttributes(registrationName='AppendAttributes1', Input=[region1, pDtCD1])



########################
###2-MAGRA
# create a new 'CSV Reader'
cr2_csv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file2])
cr2_csv.UseStringDelimiter = 0
cr2_csv.HaveHeaders = 0
cr2_csv.FieldDelimiterCharacters = ' '
cr2_csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tTP2 = TableToPoints(registrationName='TableToPoints2', Input=cr2_csv)
tTP2.XColumn = 'Field 0'
tTP2.YColumn = 'Field 0'
tTP2.ZColumn = 'Field 0'
tTP2.a2DPoints = 1
tTP2.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate2 = Triangulate(registrationName='Triangulate2', Input=tTP2)

# create a new 'Point Data to Cell Data'
pDtCD2 = PointDatatoCellData(registrationName='PointDatatoCellData2', Input=triangulate2)
pDtCD2.PointDataArraytoprocess = ['Field 0']
pDtCD2.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
region2 = WavefrontOBJReader(registrationName=geom_name+'MAGRA_grid.obj', FileName=obj_file2)

# create a new 'Append Attributes'
appendAttributes2 = AppendAttributes(registrationName='AppendAttributes2', Input=[region2, pDtCD2])


########################
###3-PETRONIO
# create a new 'CSV Reader'
cr3_csv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file3])
cr3_csv.UseStringDelimiter = 0
cr3_csv.HaveHeaders = 0
cr3_csv.FieldDelimiterCharacters = ' '
cr3_csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tTP3 = TableToPoints(registrationName='TableToPoints3', Input=cr3_csv)
tTP3.XColumn = 'Field 0'
tTP3.YColumn = 'Field 0'
tTP3.ZColumn = 'Field 0'
tTP3.a2DPoints = 1
tTP3.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate3 = Triangulate(registrationName='Triangulate3', Input=tTP3)

# create a new 'Point Data to Cell Data'
pDtCD3 = PointDatatoCellData(registrationName='PointDatatoCellData3', Input=triangulate3)
pDtCD3.PointDataArraytoprocess = ['Field 0']
pDtCD3.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
region3 = WavefrontOBJReader(registrationName=geom_name+'PETRONIO_grid.obj', FileName=obj_file3)

# create a new 'Append Attributes'
appendAttributes3 = AppendAttributes(registrationName='AppendAttributes3', Input=[region3, pDtCD3])


########################
###4-PORTOFINO
# create a new 'CSV Reader'
cr4_csv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file4])
cr4_csv.UseStringDelimiter = 0
cr4_csv.HaveHeaders = 0
cr4_csv.FieldDelimiterCharacters = ' '
cr4_csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tTP4 = TableToPoints(registrationName='TableToPoints4', Input=cr4_csv)
tTP4.XColumn = 'Field 0'
tTP4.YColumn = 'Field 0'
tTP4.ZColumn = 'Field 0'
tTP4.a2DPoints = 1
tTP4.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate4 = Triangulate(registrationName='Triangulate4', Input=tTP4)

# create a new 'Point Data to Cell Data'
pDtCD4 = PointDatatoCellData(registrationName='PointDatatoCellData4', Input=triangulate4)
pDtCD4.PointDataArraytoprocess = ['Field 0']
pDtCD4.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
region4 = WavefrontOBJReader(registrationName=geom_name+'PORTOFINO_grid.obj', FileName=obj_file4)

# create a new 'Append Attributes'
appendAttributes4 = AppendAttributes(registrationName='AppendAttributes4', Input=[region4, pDtCD4])


########################
###5-ENTELLA
# create a new 'CSV Reader'
cr5_csv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file5])
cr5_csv.UseStringDelimiter = 0
cr5_csv.HaveHeaders = 0
cr5_csv.FieldDelimiterCharacters = ' '
cr5_csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tTP5 = TableToPoints(registrationName='TableToPoints5', Input=cr5_csv)
tTP5.XColumn = 'Field 0'
tTP5.YColumn = 'Field 0'
tTP5.ZColumn = 'Field 0'
tTP5.a2DPoints = 1
tTP5.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate5 = Triangulate(registrationName='Triangulate5', Input=tTP5)

# create a new 'Point Data to Cell Data'
pDtCD5 = PointDatatoCellData(registrationName='PointDatatoCellData5', Input=triangulate5)
pDtCD5.PointDataArraytoprocess = ['Field 0']
pDtCD5.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
region5 = WavefrontOBJReader(registrationName=geom_name+'ENTELLA_grid.obj', FileName=obj_file5)

# create a new 'Append Attributes'
appendAttributes5 = AppendAttributes(registrationName='AppendAttributes5', Input=[region5, pDtCD5])


########################
###6-AVETO
# create a new 'CSV Reader'
cr6_csv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file6])
cr6_csv.UseStringDelimiter = 0
cr6_csv.HaveHeaders = 0
cr6_csv.FieldDelimiterCharacters = ' '
cr6_csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tTP6 = TableToPoints(registrationName='TableToPoints6', Input=cr6_csv)
tTP6.XColumn = 'Field 0'
tTP6.YColumn = 'Field 0'
tTP6.ZColumn = 'Field 0'
tTP6.a2DPoints = 1
tTP6.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate6 = Triangulate(registrationName='Triangulate6', Input=tTP6)

# create a new 'Point Data to Cell Data'
pDtCD6 = PointDatatoCellData(registrationName='PointDatatoCellData6', Input=triangulate6)
pDtCD6.PointDataArraytoprocess = ['Field 0']
pDtCD6.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
region6 = WavefrontOBJReader(registrationName=geom_name+'AVETO_grid.obj', FileName=obj_file6)

# create a new 'Append Attributes'
appendAttributes6 = AppendAttributes(registrationName='AppendAttributes6', Input=[region6, pDtCD6])


########################
###7-PADANO
# create a new 'CSV Reader'
cr7_csv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file7])
cr7_csv.UseStringDelimiter = 0
cr7_csv.HaveHeaders = 0
cr7_csv.FieldDelimiterCharacters = ' '
cr7_csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tTP7 = TableToPoints(registrationName='TableToPoints7', Input=cr7_csv)
tTP7.XColumn = 'Field 0'
tTP7.YColumn = 'Field 0'
tTP7.ZColumn = 'Field 0'
tTP7.a2DPoints = 1
tTP7.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate7 = Triangulate(registrationName='Triangulate7', Input=tTP7)

# create a new 'Point Data to Cell Data'
pDtCD7 = PointDatatoCellData(registrationName='PointDatatoCellData7', Input=triangulate7)
pDtCD7.PointDataArraytoprocess = ['Field 0']
pDtCD7.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
region7 = WavefrontOBJReader(registrationName=geom_name+'PADANO_grid.obj', FileName=obj_file7)

# create a new 'Append Attributes'
appendAttributes7 = AppendAttributes(registrationName='AppendAttributes7', Input=[region7, pDtCD7])



########################
###8-BISAGNO
# create a new 'CSV Reader'
cr8_csv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file8])
cr8_csv.UseStringDelimiter = 0
cr8_csv.HaveHeaders = 0
cr8_csv.FieldDelimiterCharacters = ' '
cr8_csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tTP8 = TableToPoints(registrationName='TableToPoints8', Input=cr8_csv)
tTP8.XColumn = 'Field 0'
tTP8.YColumn = 'Field 0'
tTP8.ZColumn = 'Field 0'
tTP8.a2DPoints = 1
tTP8.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate8 = Triangulate(registrationName='Triangulate8', Input=tTP8)

# create a new 'Point Data to Cell Data'
pDtCD8 = PointDatatoCellData(registrationName='PointDatatoCellData8', Input=triangulate8)
pDtCD8.PointDataArraytoprocess = ['Field 0']
pDtCD8.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
region8 = WavefrontOBJReader(registrationName=geom_name+'BISAGNO_grid.obj', FileName=obj_file8)

# create a new 'Append Attributes'
appendAttributes8 = AppendAttributes(registrationName='AppendAttributes8', Input=[region8, pDtCD8])


########################
###9-POLCEVERA
# create a new 'CSV Reader'
cr9_csv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file9])
cr9_csv.UseStringDelimiter = 0
cr9_csv.HaveHeaders = 0
cr9_csv.FieldDelimiterCharacters = ' '
cr9_csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tTP9 = TableToPoints(registrationName='TableToPoints9', Input=cr9_csv)
tTP9.XColumn = 'Field 0'
tTP9.YColumn = 'Field 0'
tTP9.ZColumn = 'Field 0'
tTP9.a2DPoints = 1
tTP9.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate9 = Triangulate(registrationName='Triangulate9', Input=tTP9)

# create a new 'Point Data to Cell Data'
pDtCD9 = PointDatatoCellData(registrationName='PointDatatoCellData9', Input=triangulate9)
pDtCD9.PointDataArraytoprocess = ['Field 0']
pDtCD9.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
region9 = WavefrontOBJReader(registrationName=geom_name+'PORTOFINO_grid.obj', FileName=obj_file9)

# create a new 'Append Attributes'
appendAttributes9 = AppendAttributes(registrationName='AppendAttributes9', Input=[region9, pDtCD9])


########################
###10-ARENZANO
# create a new 'CSV Reader'
cr10_csv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file10])
cr10_csv.UseStringDelimiter = 0
cr10_csv.HaveHeaders = 0
cr10_csv.FieldDelimiterCharacters = ' '
cr10_csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tTP10 = TableToPoints(registrationName='TableToPoints10', Input=cr10_csv)
tTP10.XColumn = 'Field 0'
tTP10.YColumn = 'Field 0'
tTP10.ZColumn = 'Field 0'
tTP10.a2DPoints = 1
tTP10.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate10 = Triangulate(registrationName='Triangulate10', Input=tTP10)

# create a new 'Point Data to Cell Data'
pDtCD10 = PointDatatoCellData(registrationName='PointDatatoCellData10', Input=triangulate10)
pDtCD10.PointDataArraytoprocess = ['Field 0']
pDtCD10.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
region10 = WavefrontOBJReader(registrationName=geom_name+'ARENZANO_grid.obj', FileName=obj_file10)

# create a new 'Append Attributes'
appendAttributes10 = AppendAttributes(registrationName='AppendAttributes10', Input=[region10, pDtCD10])


########################
###11-SASSELLO
# create a new 'CSV Reader'
cr11_csv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file11])
cr11_csv.UseStringDelimiter = 0
cr11_csv.HaveHeaders = 0
cr11_csv.FieldDelimiterCharacters = ' '
cr11_csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tTP11 = TableToPoints(registrationName='TableToPoints11', Input=cr11_csv)
tTP11.XColumn = 'Field 0'
tTP11.YColumn = 'Field 0'
tTP11.ZColumn = 'Field 0'
tTP11.a2DPoints = 1
tTP11.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate11 = Triangulate(registrationName='Triangulate11', Input=tTP11)

# create a new 'Point Data to Cell Data'
pDtCD11 = PointDatatoCellData(registrationName='PointDatatoCellData11', Input=triangulate11)
pDtCD11.PointDataArraytoprocess = ['Field 0']
pDtCD11.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
region11 = WavefrontOBJReader(registrationName=geom_name+'PORTOFINO_grid.obj', FileName=obj_file11)

# create a new 'Append Attributes'
appendAttributes11 = AppendAttributes(registrationName='AppendAttributes11', Input=[region11, pDtCD11])

########################
###12-BORMIDE
# create a new 'CSV Reader'
cr12_csv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file12])
cr12_csv.UseStringDelimiter = 0
cr12_csv.HaveHeaders = 0
cr12_csv.FieldDelimiterCharacters = ' '
cr12_csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tTP12 = TableToPoints(registrationName='TableToPoints12', Input=cr12_csv)
tTP12.XColumn = 'Field 0'
tTP12.YColumn = 'Field 0'
tTP12.ZColumn = 'Field 0'
tTP12.a2DPoints = 1
tTP12.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate12 = Triangulate(registrationName='Triangulate12', Input=tTP12)

# create a new 'Point Data to Cell Data'
pDtCD12 = PointDatatoCellData(registrationName='PointDatatoCellData12', Input=triangulate12)
pDtCD12.PointDataArraytoprocess = ['Field 0']
pDtCD12.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
region12 = WavefrontOBJReader(registrationName=geom_name+'BORMIDE_grid.obj', FileName=obj_file12)

# create a new 'Append Attributes'
appendAttributes12 = AppendAttributes(registrationName='AppendAttributes12', Input=[region12, pDtCD12])


########################
###13-SAVONESE
# create a new 'CSV Reader'
cr13_csv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file13])
cr13_csv.UseStringDelimiter = 0
cr13_csv.HaveHeaders = 0
cr13_csv.FieldDelimiterCharacters = ' '
cr13_csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tTP13 = TableToPoints(registrationName='TableToPoints13', Input=cr13_csv)
tTP13.XColumn = 'Field 0'
tTP13.YColumn = 'Field 0'
tTP13.ZColumn = 'Field 0'
tTP13.a2DPoints = 1
tTP13.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate13 = Triangulate(registrationName='Triangulate13', Input=tTP13)

# create a new 'Point Data to Cell Data'
pDtCD13 = PointDatatoCellData(registrationName='PointDatatoCellData13', Input=triangulate13)
pDtCD13.PointDataArraytoprocess = ['Field 0']
pDtCD13.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
region13 = WavefrontOBJReader(registrationName=geom_name+'SAVONESE_grid.obj', FileName=obj_file13)

# create a new 'Append Attributes'
appendAttributes13 = AppendAttributes(registrationName='AppendAttributes13', Input=[region13, pDtCD13])


########################
###14-IMPERIESE
# create a new 'CSV Reader'
cr14_csv = CSVReader(registrationName=sim_name+'.csv', FileName=[csv_file14])
cr14_csv.UseStringDelimiter = 0
cr14_csv.HaveHeaders = 0
cr14_csv.FieldDelimiterCharacters = ' '
cr14_csv.AddTabFieldDelimiter = 1

# create a new 'Table To Points'
tTP14 = TableToPoints(registrationName='TableToPoints14', Input=cr14_csv)
tTP14.XColumn = 'Field 0'
tTP14.YColumn = 'Field 0'
tTP14.ZColumn = 'Field 0'
tTP14.a2DPoints = 1
tTP14.KeepAllDataArrays = 1

# create a new 'Triangulate'
triangulate14 = Triangulate(registrationName='Triangulate14', Input=tTP14)

# create a new 'Point Data to Cell Data'
pDtCD14 = PointDatatoCellData(registrationName='PointDatatoCellData14', Input=triangulate14)
pDtCD14.PointDataArraytoprocess = ['Field 0']
pDtCD14.PassPointData = 1

# create a new 'Wavefront OBJ Reader'
region14 = WavefrontOBJReader(registrationName=geom_name+'IMPERIESE_grid.obj', FileName=obj_file14)

# create a new 'Append Attributes'
appendAttributes14 = AppendAttributes(registrationName='AppendAttributes14', Input=[region14, pDtCD14])



# ----------------------------------------------------------------
# setup the visualization in view 'renderView1'
# ----------------------------------------------------------------

# show data from italy_3_1_resobj
region1Display = Show(region1, renderView1, 'GeometryRepresentation')

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

# trace defaults for the display properties.
appendAttributes1Display.Representation = 'Surface'
appendAttributes1Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes1Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes1Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
#ColorBy(appendAttributes1Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes1Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RGBPoints = [1.813, 1.0, 1.0, 0.7137254901960784, 214.41916224541305, 1.0, 1.0, 0.7137254901960784, 602.1127762421611, 1.0, 0.8862745098039215, 0.5333333333333333, 1008.5657594232223, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1390.006187108945, 1.0, 0.6235294117647059, 0.3058823529411765, 1815.2184920371287, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2234.1778162154474, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 2628.1245848984267, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3015.817993334015, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392, 3209.66484, 0.705882, 0.0156863, 0.14902]
#field0LUT.NumberOfTableValues = 8
#field0LUT.RescaleTransferFunction(0.0, 3800.0)

################################################

# show data from italy_3_2_resobj
region2Display = Show(region2, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes2Display = Show(appendAttributes2, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes2Display.Representation = 'Surface'
appendAttributes2Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes2Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes2Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
#ColorBy(appendAttributes2Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes2Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RGBPoints = [1.813, 1.0, 1.0, 0.7137254901960784, 214.41916224541305, 1.0, 1.0, 0.7137254901960784, 602.1127762421611, 1.0, 0.8862745098039215, 0.5333333333333333, 1008.5657594232223, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1390.006187108945, 1.0, 0.6235294117647059, 0.3058823529411765, 1815.2184920371287, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2234.1778162154474, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 2628.1245848984267, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3015.817993334015, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392, 3209.66484, 0.705882, 0.0156863, 0.14902]
#field0LUT.NumberOfTableValues = 8
#field0LUT.RescaleTransferFunction(0.0, 3800.0)

##################################################

# show data from italy_3_3_resobj
region3Display = Show(region3, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes3Display = Show(appendAttributes3, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes3Display.Representation = 'Surface'
appendAttributes3Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes3Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes3Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
#ColorBy(appendAttributes3Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes3Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RGBPoints = [1.813, 1.0, 1.0, 0.7137254901960784, 214.41916224541305, 1.0, 1.0, 0.7137254901960784, 602.1127762421611, 1.0, 0.8862745098039215, 0.5333333333333333, 1008.5657594232223, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1390.006187108945, 1.0, 0.6235294117647059, 0.3058823529411765, 1815.2184920371287, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2234.1778162154474, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 2628.1245848984267, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3015.817993334015, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392, 3209.66484, 0.705882, 0.0156863, 0.14902]
#field0LUT.NumberOfTableValues = 8
#field0LUT.RescaleTransferFunction(0.0, 3800.0)


##################################################
###4-PORTOFINO
# show data from italy_3_3_resobj
region4Display = Show(region4, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes4Display = Show(appendAttributes4, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes4Display.Representation = 'Surface'
appendAttributes4Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes4Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes4Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
#ColorBy(appendAttributes4Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes4Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RGBPoints = [1.813, 1.0, 1.0, 0.7137254901960784, 214.41916224541305, 1.0, 1.0, 0.7137254901960784, 602.1127762421611, 1.0, 0.8862745098039215, 0.5333333333333333, 1008.5657594232223, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1390.006187108945, 1.0, 0.6235294117647059, 0.3058823529411765, 1815.2184920371287, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2234.1778162154474, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 2628.1245848984267, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3015.817993334015, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392, 3209.66484, 0.705882, 0.0156863, 0.14902]
#field0LUT.NumberOfTableValues = 8
#field0LUT.RescaleTransferFunction(0.0, 3800.0)


##################################################
###5-ENTELLA
region5Display = Show(region5, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes5Display = Show(appendAttributes5, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes5Display.Representation = 'Surface'
appendAttributes5Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes5Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes5Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
#ColorBy(appendAttributes5Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes5Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RGBPoints = [1.813, 1.0, 1.0, 0.7137254901960784, 214.41916224541305, 1.0, 1.0, 0.7137254901960784, 602.1127762421611, 1.0, 0.8862745098039215, 0.5333333333333333, 1008.5657594232223, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1390.006187108945, 1.0, 0.6235294117647059, 0.3058823529411765, 1815.2184920371287, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2234.1778162154474, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 2628.1245848984267, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3015.817993334015, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392, 3209.66484, 0.705882, 0.0156863, 0.14902]
#field0LUT.NumberOfTableValues = 8
#field0LUT.RescaleTransferFunction(0.0, 3800.0)


##################################################
###6-AVETO
region6Display = Show(region6, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes6Display = Show(appendAttributes6, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes6Display.Representation = 'Surface'
appendAttributes6Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes6Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes6Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
#ColorBy(appendAttributes6Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes6Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RGBPoints = [1.813, 1.0, 1.0, 0.7137254901960784, 214.41916224541305, 1.0, 1.0, 0.7137254901960784, 602.1127762421611, 1.0, 0.8862745098039215, 0.5333333333333333, 1008.5657594232223, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1390.006187108945, 1.0, 0.6235294117647059, 0.3058823529411765, 1815.2184920371287, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2234.1778162154474, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 2628.1245848984267, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3015.817993334015, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392, 3209.66484, 0.705882, 0.0156863, 0.14902]
#field0LUT.NumberOfTableValues = 8
#field0LUT.RescaleTransferFunction(0.0, 3800.0)


##################################################
###7-PADANO
region7Display = Show(region7, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes7Display = Show(appendAttributes7, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes7Display.Representation = 'Surface'
appendAttributes7Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes7Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes7Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
#ColorBy(appendAttributes7Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes7Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RGBPoints = [1.813, 1.0, 1.0, 0.7137254901960784, 214.41916224541305, 1.0, 1.0, 0.7137254901960784, 602.1127762421611, 1.0, 0.8862745098039215, 0.5333333333333333, 1008.5657594232223, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1390.006187108945, 1.0, 0.6235294117647059, 0.3058823529411765, 1815.2184920371287, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2234.1778162154474, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 2628.1245848984267, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3015.817993334015, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392, 3209.66484, 0.705882, 0.0156863, 0.14902]
#field0LUT.NumberOfTableValues = 8
#field0LUT.RescaleTransferFunction(0.0, 3800.0)

##################################################
###8-BISAGNO
region8Display = Show(region8, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes8Display = Show(appendAttributes8, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes8Display.Representation = 'Surface'
appendAttributes8Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes8Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes8Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
#ColorBy(appendAttributes8Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes8Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RGBPoints = [1.813, 1.0, 1.0, 0.7137254901960784, 214.41916224541305, 1.0, 1.0, 0.7137254901960784, 602.1127762421611, 1.0, 0.8862745098039215, 0.5333333333333333, 1008.5657594232223, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1390.006187108945, 1.0, 0.6235294117647059, 0.3058823529411765, 1815.2184920371287, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2234.1778162154474, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 2628.1245848984267, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3015.817993334015, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392, 3209.66484, 0.705882, 0.0156863, 0.14902]
#field0LUT.NumberOfTableValues = 8
#field0LUT.RescaleTransferFunction(0.0, 3800.0)


##################################################
###9-POLCEVERA
region9Display = Show(region9, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes9Display = Show(appendAttributes9, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes9Display.Representation = 'Surface'
appendAttributes9Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes9Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes9Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
#ColorBy(appendAttributes9Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes9Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RGBPoints = [1.813, 1.0, 1.0, 0.7137254901960784, 214.41916224541305, 1.0, 1.0, 0.7137254901960784, 602.1127762421611, 1.0, 0.8862745098039215, 0.5333333333333333, 1008.5657594232223, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1390.006187108945, 1.0, 0.6235294117647059, 0.3058823529411765, 1815.2184920371287, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2234.1778162154474, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 2628.1245848984267, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3015.817993334015, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392, 3209.66484, 0.705882, 0.0156863, 0.14902]
#field0LUT.NumberOfTableValues = 8
#field0LUT.RescaleTransferFunction(0.0, 3800.0)


##################################################
###10-ARENZANO
region10Display = Show(region10, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes10Display = Show(appendAttributes10, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes10Display.Representation = 'Surface'
appendAttributes10Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes10Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes10Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
#ColorBy(appendAttributes10Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes10Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RGBPoints = [1.813, 1.0, 1.0, 0.7137254901960784, 214.41916224541305, 1.0, 1.0, 0.7137254901960784, 602.1127762421611, 1.0, 0.8862745098039215, 0.5333333333333333, 1008.5657594232223, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1390.006187108945, 1.0, 0.6235294117647059, 0.3058823529411765, 1815.2184920371287, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2234.1778162154474, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 2628.1245848984267, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3015.817993334015, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392, 3209.66484, 0.705882, 0.0156863, 0.14902]
#field0LUT.NumberOfTableValues = 8
#field0LUT.RescaleTransferFunction(0.0, 3800.0)

##################################################
###11-SASSELLO
region11Display = Show(region11, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes11Display = Show(appendAttributes11, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes11Display.Representation = 'Surface'
appendAttributes11Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes11Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes11Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
#ColorBy(appendAttributes11Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes11Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RGBPoints = [1.813, 1.0, 1.0, 0.7137254901960784, 214.41916224541305, 1.0, 1.0, 0.7137254901960784, 602.1127762421611, 1.0, 0.8862745098039215, 0.5333333333333333, 1008.5657594232223, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1390.006187108945, 1.0, 0.6235294117647059, 0.3058823529411765, 1815.2184920371287, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2234.1778162154474, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 2628.1245848984267, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3015.817993334015, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392, 3209.66484, 0.705882, 0.0156863, 0.14902]
#field0LUT.NumberOfTableValues = 8
#field0LUT.RescaleTransferFunction(0.0, 3800.0)


##################################################
###12-BORMIDE
region12Display = Show(region12, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes12Display = Show(appendAttributes12, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes12Display.Representation = 'Surface'
appendAttributes12Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes12Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes12Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
#ColorBy(appendAttributes12Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes12Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RGBPoints = [1.813, 1.0, 1.0, 0.7137254901960784, 214.41916224541305, 1.0, 1.0, 0.7137254901960784, 602.1127762421611, 1.0, 0.8862745098039215, 0.5333333333333333, 1008.5657594232223, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1390.006187108945, 1.0, 0.6235294117647059, 0.3058823529411765, 1815.2184920371287, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2234.1778162154474, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 2628.1245848984267, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3015.817993334015, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392, 3209.66484, 0.705882, 0.0156863, 0.14902]
#field0LUT.NumberOfTableValues = 8
#field0LUT.RescaleTransferFunction(0.0, 3800.0)


##################################################
###13-SAVONESE
region13Display = Show(region13, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes13Display = Show(appendAttributes13, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes13Display.Representation = 'Surface'
appendAttributes13Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes13Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes13Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
#ColorBy(appendAttributes13Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes13Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RGBPoints = [1.813, 1.0, 1.0, 0.7137254901960784, 214.41916224541305, 1.0, 1.0, 0.7137254901960784, 602.1127762421611, 1.0, 0.8862745098039215, 0.5333333333333333, 1008.5657594232223, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1390.006187108945, 1.0, 0.6235294117647059, 0.3058823529411765, 1815.2184920371287, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2234.1778162154474, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 2628.1245848984267, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3015.817993334015, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392, 3209.66484, 0.705882, 0.0156863, 0.14902]
#field0LUT.NumberOfTableValues = 8
#field0LUT.RescaleTransferFunction(0.0, 3800.0)

##################################################
###14-IMPERIESE
region14Display = Show(region14, renderView1, 'GeometryRepresentation')

# show data from appendAttributes2
appendAttributes14Display = Show(appendAttributes14, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
appendAttributes14Display.Representation = 'Surface'
appendAttributes14Display.ColorArrayName = ['CELLS', 'Field 0']
appendAttributes14Display.LookupTable = field0LUT

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
appendAttributes14Display.OSPRayScaleFunction.Points = [0.0892922587, 0.0, 0.5, 0.0, 12.7415053, 1.0, 0.5, 0.0]

# set scalar coloring
#ColorBy(appendAttributes14Display, ('CELLS', 'Field 0'))
# rescale color and/or opacity maps used to include current data range
#appendAttributes14Display.RescaleTransferFunctionToDataRange(True, False)
# Rescale transfer function
#field0LUT = GetColorTransferFunction('Field 0')
#field0LUT.RGBPoints = [1.813, 1.0, 1.0, 0.7137254901960784, 214.41916224541305, 1.0, 1.0, 0.7137254901960784, 602.1127762421611, 1.0, 0.8862745098039215, 0.5333333333333333, 1008.5657594232223, 0.996078431372549, 0.7647058823529411, 0.3764705882352941, 1390.006187108945, 1.0, 0.6235294117647059, 0.3058823529411765, 1815.2184920371287, 0.984313725490196, 0.4627450980392157, 0.23529411764705882, 2234.1778162154474, 0.9529411764705882, 0.2784313725490196, 0.16862745098039217, 2628.1245848984267, 0.8627450980392157, 0.13333333333333333, 0.14901960784313725, 3015.817993334015, 0.7411764705882353, 0.011764705882352941, 0.1568627450980392, 3209.66484, 0.705882, 0.0156863, 0.14902]
#field0LUT.NumberOfTableValues = 8
#field0LUT.RescaleTransferFunction(0.0, 3800.0)

#################################################


# setup the color legend parameters for each legend in this view

# get color legend/bar for field0LUT in view renderView1
field0LUTColorBar = GetScalarBar(field0LUT, renderView1)
field0LUTColorBar.Title = 'Field 0'
field0LUTColorBar.ComponentTitle = ''

# set color bar visibility
field0LUTColorBar.Visibility = 1

# show color legend
appendAttributes1Display.SetScalarBarVisibility(renderView1, True)
appendAttributes2Display.SetScalarBarVisibility(renderView1, True)
appendAttributes3Display.SetScalarBarVisibility(renderView1, True)
appendAttributes4Display.SetScalarBarVisibility(renderView1, True)
appendAttributes5Display.SetScalarBarVisibility(renderView1, True)
appendAttributes6Display.SetScalarBarVisibility(renderView1, True)
appendAttributes7Display.SetScalarBarVisibility(renderView1, True)
appendAttributes8Display.SetScalarBarVisibility(renderView1, True)
appendAttributes9Display.SetScalarBarVisibility(renderView1, True)
appendAttributes10Display.SetScalarBarVisibility(renderView1, True)
appendAttributes11Display.SetScalarBarVisibility(renderView1, True)
appendAttributes12Display.SetScalarBarVisibility(renderView1, True)
appendAttributes13Display.SetScalarBarVisibility(renderView1, True)
appendAttributes14Display.SetScalarBarVisibility(renderView1, True)

# ----------------------------------------------------------------
# setup color maps and opacity mapes used in the visualization
# note: the Get..() functions create a new object, if needed
# ----------------------------------------------------------------

# get opacity transfer function/opacity map for 'Field0'
field0PWF = GetOpacityTransferFunction('Field0')
field0PWF.Points = [0.0836820019, 0.0, 0.5, 0.0, 14.4433432, 1.0, 0.5, 0.0]
field0PWF.ScalarRangeInitialized = 1

# reset view to fit data bounds
renderView1.ResetCamera()


# To save a specific target resolution, rather than using the
# the current view (or layout) size, and override the color palette.
directory = script_dir+'/../../'+project_folder+'/_fig'
if not os.path.exists(directory): 
	os.mkdir(directory)
	
SaveScreenshot(directory+'/'+sim_name+'.png', renderView1, FontScaling=True, OverrideColorPalette='WhiteBackground', TransparentBackground=False)



# ----------------------------------------------------------------
# restore active source
SetActiveSource(appendAttributes14)
# ----------------------------------------------------------------

if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')
    
    
    
