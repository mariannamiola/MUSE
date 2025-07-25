#!/bin/bash

help()
{
    echo "
    	Usage: 10_test_MUSE 
    			[ -d | --data 	    ] data filename
    			[ -D | --datasource ] data folder
    			[ -p | --proj       ] project name
    			[ -r | --repl       ] replace project
        	        [ -s | --sim        ] number of simulations
               		[ -w | --work       ] working directory
               		[ -h | --help       ] 
               		
               		"
    exit 2
}

(
set -e	#exit if an error occours

SHORT=d:,D:,p:,r:,s:,w:,h
LONG=data:,data_source:,proj:,repl:,sim:,work:,help
OPTS=$(getopt -a --options $SHORT --longoptions $LONG -- "$@")
echo $OPTS

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
    -r | --repl )
      OPTREPL="$2"
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
echo $PATH

#######################################################################

export MUSE=${SCRIPT_DIR}/../../..

if [ "$OPTDATASOURCE" ]	#if a input data folder is provided
then
	export DATA_SOURCE=${OPTDATASOURCE}
else
	export DATA_SOURCE=${MUSE}/examples/13_RISL_SUpoly_mf/data
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
	export PROJ=13_RISL_SUpoly_mf
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	#export DATA=080220141900_format_UTM.csv
	export DATA=190520202200_UTM.csv
fi


# 1. Export variables
#######################################################################
##export GEOM0=italy_3
##export GEOM1=_t2000
##export GEOM=points
export SU=su_liguria

export VAR0=forecast
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
#export OPT=a500000

#For vario
export DIR=DIR
export DIM=2D

#For compute
if [ "$OPTSIM" ]	#if a number of simulations is provided
then
	export NSIM=$OPTSIM
else
	export NSIM=10
fi
#######################################################################

# Project settings
#######################################################################
if [ "$OPTREPL" ]	#if a boolean for replace data is provided
then
	export RMPROJ=${OPTREPL}
else
	export RMPROJ=true
fi

########## REPORT  ###########

echo "
    	Running: 10_test_MUSE 
    			[ -d | --data 	    ] $DATA
    			[ -D | --datasource ] ${DATA_SOURCE}
    			[ -p | --proj       ] $PROJ
    			[ -r | --repl       ] $RMPROJ
        	        [ -s | --sim        ] $NSIM
               		[ -w | --work       ] $WORK
               		
               		"

####################################################################### MUSE START ...
export DATAFOLDER=Liguria_2018_format

# Export folders
export WP=${WORK}/${PROJ}

export INWP=${WP}/in

export INDATA=${INWP}/data
export INGEOM=${INWP}/geometry

export OUTWP=${WP}/out

export OUTDATA=${OUTWP}/data
export OUTSURF=${OUTWP}/geometry/surf
export OUTVOL=${OUTWP}/geometry/volume

export OUTMAN=${OUTWP}/manipulate
export OUTVARIO=${OUTWP}/vario
export OUTCOMP=${OUTWP}/compute

export OUTSGS=VECSIM


mkdir -p ${WORK}

if [[ $OSTYPE == 'darwin'* ]]; then
  SED=gsed
else
  SED=sed
fi


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
# Check if the current user is root
if [[ -d ${WP} ]]; then
    echo "DIRECTORY: " ${WP} " EXISTS."
    if $RMPROJ; then
        echo "REMOVING EXISTING PROJECT " ${WP} 
        rm -r ${WP}
    
        ##########  PROJECT  ###########
        muse_project -N -p ${WORK} --name ${PROJ}
   
        ##########  GEOMETRY  ###########
        muse_geometry -N -p ${WP}
        cp -R ${DATA_SOURCE}/${SU}.gpkg ${INGEOM}
        muse_geometry -V -p ${WP} --poly --obj --attribute
        #cp -R ${DATA_SOURCE}/../${GEOM}.xyz ${INGEOM}
        #muse_geometry -P -p ${WP} --tri --concave --points ${INGEOM}/${GEOM}.xyz --opt ${OPT} --obj
        
    else
        if [[ -d ${OUTMAN} ]]; then
           rm -r ${OUTMAN}
        fi
        if [[ -d ${OUTVARIO} ]]; then
           rm -r ${OUTVARIO}
        fi
        if [[ -d ${OUTCOMP} ]]; then
           rm -r ${OUTCOMP}
        fi
    fi
else
    echo "DIRECTORY: " ${WP} " NOT EXISTS."
    ##########  PROJECT  ###########
    muse_project -N -p ${WORK} --name ${PROJ}
    
    ##########  GEOMETRY  ###########
    muse_geometry -N -p ${WP}
    cp -R ${DATA_SOURCE}/${SU}.gpkg ${INGEOM}
    muse_geometry -V -p ${WP} --poly --obj --attribute
    #cp -R ${DATA_SOURCE}/../${GEOM}.xyz ${INGEOM}
    #muse_geometry -P -p ${WP} --tri --concave --points ${INGEOM}/${GEOM}.xyz --opt ${OPT} --obj
    
fi

#testdata:    
##########  DATA  ###########
muse_data -N -p ${WP}
cp -R ${DATA_SOURCE}/${DATAFOLDER}/*.csv ${INDATA}
muse_data -S -p ${WP} --setX 3 --setY 4
muse_data -C -p ${WP}
        
#manip:
#########  MANIPULATE  ###########


#vario:
#########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --eps 5.0 --dim ${DIM} --dir ${DIR} --dirs 0,22.5,35,90,110 --vclean 2 ##--maxdist 200000 ##--decl 


#compute:
#########  COMPUTE  ###########
muse_compute -C -p ${WP} -v ${VAR0} -m ${OUTSURF}/${SU}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS} --file ${DATA_SOURCE}/centroids.csv
muse_compute -S -p ${WP} -v ${VAR0} -m ${OUTSURF}/${SU}.obj --dir ${DIR} --dim ${DIM} --csv

muse_compute -B -p ${WP} -v ${VAR0} -m ${OUTSURF}/${SU}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 50 --csv

muse_compute -S -p ${WP} -v ${VAR0} -m ${OUTSURF}/${SU}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv

#export:
#########  EXPORT  ###########
cd ${OUTCOMP}
for frame in * 
do
   echo ${frame}
   muse_export -p ${WP} -m ${SU}.obj --dir ${DIR} --dim ${DIM} -v ${VAR0} -N ${NSIM} --mf ${frame} --append_gpkg ${SU}.gpkg --geopkg -o ${frame}/${SU}_${frame}_out.gpkg
done

#if [[ $OUTSGS == 'MEAN'* ]]; then
#	muse_compute -C -p ${WP} -v ${VAR0} -m ${OUTSURF}/${SU}.obj --nsim ${NSIM} --bnscore --extr Extr --minextr 0 --maxextr 50 --dir ${DIR} --dim ${DIM}
#else
#	muse_compute -C -p ${WP} -v ${VAR0} -m ${OUTSURF}/${SU}.obj --nsim ${NSIM} --dir ${DIR} --dim ${DIM} --out ${OUTSGS}
#	muse_compute -S -p ${WP} -v ${VAR0} -m ${OUTSURF}/${SU}.obj --dir ${DIR} --dim ${DIM} --csv
#
#	muse_compute -B -p ${WP} -v ${VAR0} -m ${OUTSURF}/${SU}.obj --dir ${DIR} --dim ${DIM} --extr Extr --minextr 0 --maxextr 50 --csv
#
#	muse_compute -S -p ${WP} -v ${VAR0} -m ${OUTSURF}/${SU}.obj --dir ${DIR} --dim ${DIM} --space VAR --csv
#fi


##slope:
###########  GEOMETRY  ###########
#### SLOPE UNIT EXTRACTION
#cp -R ${DATA_SOURCE}/../${SU} ${INGEOM}
#
#for frame in ${OUTCOMP}/* 
#do
#	if [[ -d "$frame" && ! -f "${OUTCOMP}/*.csv" ]]; then
#		echo ${frame}
#		muse_geometry -D -p ${WP} --refmod ${OUTSURF}/${GEOM}.obj --polygon ${INGEOM}/${SU}/su_LIGURIA_susc.shp -f ${frame}/${VAR0}_DIR2D_${GEOM}/_varspace/_stats/${VAR0}_mean.csv
#	fi
#done


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
