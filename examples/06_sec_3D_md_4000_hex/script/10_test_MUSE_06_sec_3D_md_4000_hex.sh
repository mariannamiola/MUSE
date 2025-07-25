#!/bin/bash

help()
{
    echo "
    	Usage: 10_test_MUSE 
    			[ -d | --data 	    ] data filename
    			[ -D | --datasource ] data folder
    			[ -p | --proj       ] project name
        	        [ -s | --sim        ] number of simulations
               		[ -w | --work       ] working directory
               		[ -h | --help       ] 
               		
               		"
    exit 2
}

(
set -e	#exit if an error occours

SHORT=d:,D:,p:,s:,w:,h
LONG=data:,data_source:,proj:,sim:,work:,help
OPTS=$(getopt -a --options $SHORT --longoptions $LONG -- "$@")
#echo $OPTS

eval set -- "$OPTS"

while :
do
  case "$1" in
    -d | --data )
      OPTDATA="$2"
      shift 2
      ;;
    -D | --datasource )
      OPTDATASOURCE="$2"
      shift 2
      ;;
    -p | --proj )
      OPTPROJ="$2"
      shift 2
      ;;
    -s | --sim )
      OPTSIM="$2"
      shift 2
      ;;
    -w | --work )
      OPTWORK="$2"
      shift 2
      ;;
    -h | --help)
      help
      exit 2
      ;;
    --)
      shift;
      break
      ;;
    *)
      echo "Unexpected option: $1"
      ;;
  esac
done


# 0. Export paths
#######################################################################

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BIN_DIR=${SCRIPT_DIR}/../../../bin
export PATH=${BIN_DIR}:$PATH
###echo $PATH

#######################################################################

export MUSE=${SCRIPT_DIR}/../../..

if [ "$OPTDATASOURCE" ]	#if a input data folder is provided
then
	export DATA_SOURCE=${OPTDATASOURCE}
else
	export DATA_SOURCE=${MUSE}/examples/06_sec_3D_md_4000_hex/data
fi

if [ "$OPTWORK" ]	#if a working directory is provided
then
	export WORK=${OPTWORK}
else
	export WORK=${MUSE}/examples/MUSE_test
fi


# 1. Export variables
#######################################################################

if [ "$OPTPROJ" ]	#if a project name is provided
then
	export PROJ=${OPTPROJ}
else
	export PROJ=06_sec_3D_md_4000_hex
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=amga.csv
fi


# 1. Export variables
#######################################################################
export GEOM1=SEC.gpkg

export VAR=phi
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=a0.5
#export OPT=a0.1

#For vario
export DIR=DIR
export DIM=2D

#For compute
if [ "$OPTSIM" ]	#if a number of simulations is provided
then
	export NSIM=$OPTSIM
else
	export NSIM=100
fi
#######################################################################

########## REPORT  ###########

echo "
    	Running: 10_test_MUSE 
    			[ -d | --data 	    ] $DATA
    			[ -D | --datasource ] ${DATA_SOURCE}
    			[ -p | --proj       ] $PROJ
        	        [ -s | --sim        ] $NSIM
               		[ -w | --work       ] $WORK
               		
               		"

####################################################################### MUSE START ...
# Export folders
export WP=${WORK}/${PROJ}

export INWP=${WP}/in

export INDATA=${INWP}/data
export INGEOM=${INWP}/geometry

export OUTWP=${WP}/out

export OUTSURF=${OUTWP}/geometry/surf
export OUTVOL=${OUTWP}/geometry/volume

export OUTMAN=${OUTWP}/manipulate



mkdir -p ${WORK}

if [[ $OSTYPE == 'darwin'* ]]; then
  SED=gsed
else
  SED=sed
fi

export RESX=0.5
export RESY=0.1
export RESZ=1.0

export SURF1=SEC_1_0.5-SEC_1_absz
export SURF2=SEC_2_0.5-SEC_2_absz
export SURF3=SEC_3_0.5-SEC_3_absz

export OUTSGS=VECSIM


# https://bobcopeland.com/blog/2012/10/goto-in-bash/
function jumpto
{
    label=$1
    cmd=$($SED	 -n "/$label:/{:a;n;p;ba};" $0 | grep -v ':$')
    eval "$cmd"
    exit
}

start=${1:-"start"}

jumpto $start

#project:
##########  PROJECT  ###########
muse_project -N -p ${WORK} --name ${PROJ}

##########  DATA  ###########

#set +e	#do not exit if an error occours
muse_data -N -p ${WP}
cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
muse_data -S -p ${WP}  --setX 1 --setY 2 --setZ 3
muse_data -C -p ${WP}
#set -e	#exit if an error occours

#geometry:
##########  GEOMETRY  ###########
#set +e	#do not exit if an error occours
muse_geometry -N -p ${WP}
cp -R ${DATA_SOURCE}/${GEOM1} ${INGEOM}

muse_geometry -V -p ${WP} --tri --rotaxis X --rotangle 270 --opt ${OPT} --setz 0.0 --obj

muse_geometry -O -p ${WP} -m ${OUTSURF}/SEC_1.obj --abs -z -1 --obj
muse_geometry -O -p ${WP} -m ${OUTSURF}/SEC_2.obj --abs -z -1 --obj
muse_geometry -O -p ${WP} -m ${OUTSURF}/SEC_3.obj --abs -z -1 --obj

mv -v ${OUTSURF}/SEC_1_absz.obj ${OUTSURF}/SEC_1.obj
mv -v ${OUTSURF}/SEC_2_absz.obj ${OUTSURF}/SEC_2.obj
mv -v ${OUTSURF}/SEC_3_absz.obj ${OUTSURF}/SEC_3.obj

muse_geometry -O -p ${WP} -m ${OUTSURF}/SEC_1.obj --abs -z -0.5 --obj
muse_geometry -O -p ${WP} -m ${OUTSURF}/SEC_2.obj --abs -z -0.5 --obj
muse_geometry -O -p ${WP} -m ${OUTSURF}/SEC_3.obj --abs -z -0.5 --obj

mv -v ${OUTSURF}/SEC_1_absz.obj ${OUTSURF}/SEC_1_0.5.obj
mv -v ${OUTSURF}/SEC_2_absz.obj ${OUTSURF}/SEC_2_0.5.obj
mv -v ${OUTSURF}/SEC_3_absz.obj ${OUTSURF}/SEC_3_0.5.obj

muse_geometry -O -p ${WP} -m ${OUTSURF}/SEC_1.obj --abs -z -1.5 --obj
muse_geometry -O -p ${WP} -m ${OUTSURF}/SEC_2.obj --abs -z -1.5 --obj
muse_geometry -O -p ${WP} -m ${OUTSURF}/SEC_3.obj --abs -z -1.5 --obj

muse_geometry -T -p ${WP} -m ${OUTSURF}/SEC_1_0.5.obj -m ${OUTSURF}/SEC_1_absz.obj --obj
muse_geometry -T -p ${WP} -m ${OUTSURF}/SEC_2_0.5.obj -m ${OUTSURF}/SEC_2_absz.obj --obj
muse_geometry -T -p ${WP} -m ${OUTSURF}/SEC_3_0.5.obj -m ${OUTSURF}/SEC_3_absz.obj --obj

muse_geometry -G -p ${WP} --hex --bbp 0,-15,-0.5 --bbp 100,-15,-0.5 --bbp 100,0,-0.5 --bbp 0,0,-0.5 --bbp 0,-15,-1.5 --bbp 100,-15,-1.5 --bbp 100,0,-1.5 --bbp 0,0,-1.5 --resx ${RESX} --resy ${RESY} --resz ${RESZ} --vtk

muse_geometry -S -p ${WP} --hex -m ${OUTVOL}/grid.vtk  --boundary ${OUTSURF}/SEC_1_0.5-SEC_1_absz.obj --vtk 
muse_geometry -S -p ${WP} --hex -m ${OUTVOL}/grid.vtk  --boundary ${OUTSURF}/SEC_2_0.5-SEC_2_absz.obj --vtk
muse_geometry -S -p ${WP} --hex -m ${OUTVOL}/grid.vtk  --boundary ${OUTSURF}/SEC_3_0.5-SEC_3_absz.obj --vtk

mv -v ${OUTVOL}/grid_SEC_1_0.5-SEC_1_absz.vtk ${OUTVOL}/SEC_1.vtk
mv -v ${OUTVOL}/grid_SEC_2_0.5-SEC_2_absz.vtk ${OUTVOL}/SEC_2.vtk
mv -v ${OUTVOL}/grid_SEC_3_0.5-SEC_3_absz.vtk ${OUTVOL}/SEC_3.vtk

muse_geometry -U -p ${WP} -m ${OUTVOL}/SEC_1.vtk -m ${OUTVOL}/SEC_2.vtk --vtk
muse_geometry -U -p ${WP} -m ${OUTVOL}/SEC_1_SEC_2.vtk -m ${OUTVOL}/SEC_3.vtk --vtk

mv -v ${OUTVOL}/SEC_1_SEC_2_SEC_3.vtk ${OUTVOL}/SEC.vtk
muse_geometry -Z -p ${WP} -m ${OUTVOL}/SEC.vtk --rotaxis X --rotangle -270 --vtk

#set -e	#exit if an error occours


#manip:
##########  MANIPULATE  ###########
muse_manipulate -E -p ${WP} --geom ${OUTSURF}/SEC_1_0.5-SEC_1_absz.obj --rotaxis X --rotangle 270
muse_manipulate -E -p ${WP} --geom ${OUTSURF}/SEC_2_0.5-SEC_2_absz.obj --rotaxis X --rotangle 270
muse_manipulate -E -p ${WP} --geom ${OUTSURF}/SEC_3_0.5-SEC_3_absz.obj --rotaxis X --rotangle 270


#vario:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR} --sub ${SURF1} --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --vclean 10 --eps 5.0 --tolfac 2.0 --dirs 0,22.5,67.5,90,112.5 --degtol 22.5 #--weight 
muse_vario -V -p ${WP} -v ${VAR} --sub ${SURF2} --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --vclean 10 --eps 5.0 --tolfac 2.0 --dirs 0,22.5,67.5,90,112.5 --degtol 22.5 #--weight 
muse_vario -V -p ${WP} -v ${VAR} --sub ${SURF3} --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --vclean 10 --eps 5.0 --tolfac 2.0 --dirs 0,22.5,67.5,90,112.5 --degtol 22.5 #--weight 


#compute:
##########  COMPUTE  ###########
export VOL1=SEC_1
export VOL2=SEC_2
export VOL3=SEC_3

export OUTCOM1=${OUTWP}/compute/${VAR}_${SURF1}_${DIR}${DIM}_${VOL1}
export OUTCOM2=${OUTWP}/compute/${VAR}_${SURF2}_${DIR}${DIM}_${VOL2}
export OUTCOM3=${OUTWP}/compute/${VAR}_${SURF3}_${DIR}${DIM}_${VOL3}

export OUTNORMS1=${OUTCOM1}/_normspace
export OUTNORMS2=${OUTCOM2}/_normspace
export OUTNORMS3=${OUTCOM3}/_normspace

export OUTVARS1=${OUTCOM1}/_varspace
export OUTVARS2=${OUTCOM2}/_varspace
export OUTVARS3=${OUTCOM3}/_varspace

if [[ $OUTSGS == 'MEAN'* ]]; then
  muse_compute -C -p ${WP} -v ${VAR} --sub ${SURF1} -m ${OUTVOL}/${VOL1}.vtk --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 0 --maxextr 1
else
  muse_compute -C -p ${WP} -v ${VAR} --sub ${SURF1} -m ${OUTVOL}/${VOL1}.vtk --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
  muse_compute -S -p ${WP} -v ${VAR} --sub ${SURF1} -m ${OUTVOL}/${VOL1}.vtk --dir ${DIR} --dim ${DIM} --csv

  #back:
  muse_compute -B -p ${WP} -v ${VAR} --sub ${SURF1} -m ${OUTVOL}/${VOL1}.vtk --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 1 --csv
  #for (( id=0; id<=$NSIM-1; id++ ))
  #do
  #  muse_compute -B -p ${WP} -v ${VAR} --sub ${SURF1} -m ${OUTVOL}/${VOL1}.vtk --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 1 -f ${OUTNORMS1}/${VAR}_sgs_$id.csv
  #done

  #statsback:
  muse_compute -S -p ${WP} -v ${VAR} --sub ${SURF1} -m ${OUTVOL}/${VOL1}.vtk --dir ${DIR} --dim ${DIM} --space VAR --csv
fi


if [[ $OUTSGS == 'MEAN'* ]]; then
  muse_compute -C -p ${WP} -v ${VAR} --sub ${SURF2} -m ${OUTVOL}/${VOL2}.vtk --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 0 --maxextr 1
else
  muse_compute -C -p ${WP} -v ${VAR} --sub ${SURF2} -m ${OUTVOL}/${VOL2}.vtk --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
  muse_compute -S -p ${WP} -v ${VAR} --sub ${SURF2} -m ${OUTVOL}/${VOL2}.vtk --dir ${DIR} --dim ${DIM} --csv

  #back:
  muse_compute -B -p ${WP} -v ${VAR} --sub ${SURF2} -m ${OUTVOL}/${VOL2}.vtk --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 1 --csv
  #for (( id=0; id<=$NSIM-1; id++ ))
  #do
  #  muse_compute -B -p ${WP} -v ${VAR} --sub ${SURF2} -m ${OUTVOL}/${VOL2}.vtk --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 1 -f ${OUTNORMS2}/${VAR}_sgs_$id.csv
  #done

  #statsback:
  muse_compute -S -p ${WP} -v ${VAR} --sub ${SURF2} -m ${OUTVOL}/${VOL2}.vtk --dir ${DIR} --dim ${DIM} --space VAR --csv
fi


if [[ $OUTSGS == 'MEAN'* ]]; then
  muse_compute -C -p ${WP} -v ${VAR} --sub ${SURF3} -m ${OUTVOL}/${VOL3}.vtk --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 0 --maxextr 1
else
  muse_compute -C -p ${WP} -v ${VAR} --sub ${SURF3} -m ${OUTVOL}/${VOL3}.vtk --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
  muse_compute -S -p ${WP} -v ${VAR} --sub ${SURF3} -m ${OUTVOL}/${VOL3}.vtk --dir ${DIR} --dim ${DIM} --csv

  #back:
  muse_compute -B -p ${WP} -v ${VAR} --sub ${SURF3} -m ${OUTVOL}/${VOL3}.vtk --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 1 --csv
  #for (( id=0; id<=$NSIM-1; id++ ))
  #do
  #  muse_compute -B -p ${WP} -v ${VAR} --sub ${SURF3} -m ${OUTVOL}/${VOL3}.vtk --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 1 -f ${OUTNORMS3}/${VAR}_sgs_$id.csv
  #done

  #statsback:
  muse_compute -S -p ${WP} -v ${VAR} --sub ${SURF3} -m ${OUTVOL}/${VOL3}.vtk --dir ${DIR} --dim ${DIM} --space VAR --csv
fi


#cat:
if [[ $OUTSGS == 'MEAN'* ]]; then
  cat ${OUTCOM1}/${VAR}_${SURF1}.csv ${OUTCOM2}/${VAR}_${SURF2}.csv ${OUTCOM3}/${VAR}_${SURF3}.csv > ${OUTWP}/compute/${VAR}_merge.csv
else
  cat ${OUTVARS1}/_stats/${VAR}_sgs_mean.csv ${OUTVARS2}/_stats/${VAR}_sgs_mean.csv ${OUTVARS3}/_stats/${VAR}_sgs_mean.csv > ${OUTWP}/compute/${VAR}_sgs_merge.csv
fi
#echo ${OUTVARS1}/_stats/${VAR}_mean.csv


#clean:
if [[ $OUTSGS == 'MEAN'* ]]; then
  for (( id=0; id<$NSIM; id++ ))
  do
    rm ${SCRIPT_DIR}/output${id}.dat
  done
  rm ${SCRIPT_DIR}/sgs_output_data
  rm ${SCRIPT_DIR}/back_transf_data
else
    for (( id=0; id<=$NSIM-1; id++ ))
  do
    rm ${SCRIPT_DIR}/sgs_output_${id}.dat
    rm ${SCRIPT_DIR}/output${id}.dat
  done
  rm ${SCRIPT_DIR}/sgs_output_data
fi

####################################################################### MUSE END

)
