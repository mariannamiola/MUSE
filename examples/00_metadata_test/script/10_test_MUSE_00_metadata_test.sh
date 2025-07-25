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
echo $PATH

#######################################################################

export MUSE=${SCRIPT_DIR}/../../..

if [ "$OPTDATASOURCE" ]	#if a input data folder is provided
then
	export DATA_SOURCE=${OPTDATASOURCE}
else
	export DATA_SOURCE=${MUSE}/examples/00_metadata_test/data
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
	export PROJ=00_metadata_test
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=nisyros.csv
fi


# 1. Export variables
#######################################################################
export GEOM1=nis.gpkg
export GEOM2=nis.xyz

export VAR0=T
export VAR1=CO2
export VAR2=lCO2
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=a100

#For vario
export DIR=DIR

#For compute
if [ "$OPTSIM" ]	#if a number of simulations is provided
then
	export NSIM=$OPTSIM
else
	export NSIM=10
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
##########  PROJECT  ###########
muse_project -N -p ${WORK} --name ${PROJ}

##########  DATA  ###########
muse_data -N -p ${WP}
cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
muse_data -S -p ${WP}  --setX 1 --setY 2 --setZ 3
muse_data -C -p ${WP}


#geometry:
##########  GEOMETRY  ###########
muse_geometry -N -p ${WP}

###routine for triangulation and tets
cp -R ${DATA_SOURCE}/${GEOM1} ${INGEOM}
muse_geometry -V -p ${WP} --tri --opt ${OPT} --obj
mv -v ${OUTSURF}/nis.obj ${OUTSURF}/nis_tri.obj
mv -v ${OUTSURF}/nis.json ${OUTSURF}/nis_tri.json

muse_geometry -V -p ${WP} --grid --resx 10 --resy 10 --obj
mv -v ${OUTSURF}/nis.obj ${OUTSURF}/nis_grid.obj
mv -v ${OUTSURF}/nis.json ${OUTSURF}/nis_grid.json

muse_geometry -V -p ${WP} --tri --opt a10000 --obj
mv -v ${OUTSURF}/nis.obj ${OUTSURF}/nis_tri_coarse.obj
mv -v ${OUTSURF}/nis.json ${OUTSURF}/nis_tri_coarse.json

muse_geometry -L -p ${WP} -m ${OUTSURF}/nis_tri_coarse.obj --splmet EDGE --obj
mv -v ${OUTSURF}/nis_tri_coarse_res.obj ${OUTSURF}/nis_tri_mid.obj
mv -v ${OUTSURF}/nis_tri_coarse_res.json ${OUTSURF}/nis_tri_mid.json

muse_geometry -L -p ${WP} -m ${OUTSURF}/nis_tri_mid.obj --splmet EDGE --obj
mv -v ${OUTSURF}/nis_tri_mid_res.obj ${OUTSURF}/nis_tri_fine.obj
mv -v ${OUTSURF}/nis_tri_mid_res.json ${OUTSURF}/nis_tri_fine.json


#vario:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --dir ${DIR} --dim 2D --vario MODEL --deg 22.5 --degtol 22.5 --vclean 10 --eps 5.0 --type SPHERICAL --nugget 0.25
muse_vario -V -p ${WP} -v ${VAR1} --nscore YES --dir ${DIR} --dim 2D --vario MODEL --deg 22.5 --degtol 22.5 --vclean 10 --eps 5.0 --type SPHERICAL --nugget 0.25
muse_vario -V -p ${WP} -v ${VAR2} --nscore YES --dir ${DIR} --dim 2D --vario MODEL --deg 22.5 --degtol 22.5 --vclean 10 --eps 5.0 --type SPHERICAL --nugget 0.25


#compute:
declare -a arr=("nis_tri" "nis_grid" "nis_tri_fine")

for i in "${arr[@]}"
do
  if [[ $DIR == 'DIR'* ]]; then
    export OUTCOM0=${OUTWP}/compute/${VAR0}_dir_$i
    export OUTCOM1=${OUTWP}/compute/${VAR1}_dir_$i
    export OUTCOM2=${OUTWP}/compute/${VAR2}_dir_$i
  else
    export OUTCOM0=${OUTWP}/compute/${VAR0}_omni_$i
    export OUTCOM1=${OUTWP}/compute/${VAR1}_omni_$i
    export OUTCOM2=${OUTWP}/compute/${VAR2}_omni_$i
  fi

  export OUTNORMS0=${OUTCOM0}/_normspace
  export OUTNORMS1=${OUTCOM1}/_normspace
  export OUTNORMS2=${OUTCOM2}/_normspace

  #export OUTVARS=${OUTCOM}/_varspace

	
  if [[ $OUTSGS == 'MEAN'* ]]; then
    ##########  COMPUTE on tri  ###########
    echo "Computing stochastic simulations on " $i " mesh ... "
    muse_compute -C -p ${WP} -v ${VAR0} -m ${OUTSURF}/$i.obj --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 10 --maxextr 100 --dir ${DIR}
    muse_compute -C -p ${WP} -v ${VAR1} -m ${OUTSURF}/$i.obj --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 0 --maxextr 6500 --dir ${DIR}
    muse_compute -C -p ${WP} -v ${VAR2} -m ${OUTSURF}/$i.obj --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr -1 --maxextr 4 --dir ${DIR}
  
  else
    echo "Computing stochastic simulations on " $i " mesh ... "
    ##########  COMPUTE on tri  ###########
    muse_compute -C -p ${WP} -v ${VAR0} -m ${OUTSURF}/$i.obj --nsim ${NSIM} --dir ${DIR} --out ${OUTSGS}
    muse_compute -C -p ${WP} -v ${VAR1} -m ${OUTSURF}/$i.obj --nsim ${NSIM} --dir ${DIR} --out ${OUTSGS}
    muse_compute -C -p ${WP} -v ${VAR2} -m ${OUTSURF}/$i.obj --nsim ${NSIM} --dir ${DIR} --out ${OUTSGS}
  
    #stats:
    muse_compute -S -p ${WP} -v ${VAR0} -m ${OUTSURF}/$i.obj --dir ${DIR} --csv
    muse_compute -S -p ${WP} -v ${VAR1} -m ${OUTSURF}/$i.obj --dir ${DIR} --csv
    muse_compute -S -p ${WP} -v ${VAR2} -m ${OUTSURF}/$i.obj --dir ${DIR} --csv
  
    #back: 
    for (( id=0; id<=$NSIM-1; id++ ))
    do
      muse_compute -B -p ${WP} -v ${VAR0} -m ${OUTSURF}/$i.obj --dir ${DIR} --extr Extr --minextr 10 --maxextr 100 -f ${OUTNORMS0}/sgs_$id.csv --csv
      muse_compute -B -p ${WP} -v ${VAR1} -m ${OUTSURF}/$i.obj --dir ${DIR} --extr Extr --minextr 0 --maxextr 6500 -f ${OUTNORMS1}/sgs_$id.csv --csv
      muse_compute -B -p ${WP} -v ${VAR2} -m ${OUTSURF}/$i.obj --dir ${DIR} --extr Extr --minextr -1 --maxextr 4 -f ${OUTNORMS2}/sgs_$id.csv --csv
    done
  
    #statsback:
    muse_compute -S -p ${WP} -v ${VAR0} -m ${OUTSURF}/$i.obj --dir ${DIR} --space VAR --csv
    muse_compute -S -p ${WP} -v ${VAR1} -m ${OUTSURF}/$i.obj --dir ${DIR} --space VAR --csv
    muse_compute -S -p ${WP} -v ${VAR2} -m ${OUTSURF}/$i.obj --dir ${DIR} --space VAR --csv
  fi
  
  #db:
  ##########  DATABASE  ###########
  if [[ $OUTSGS == 'MEAN'* ]]; then
    echo "Dataset creation for MEAN SGS method ... NOT IMPLEMENTED!"
  else
    muse_compute -D -p ${WP} -v ${VAR0} -m ${OUTSURF}/$i.obj --dir ${DIR} --space VAR
    muse_compute -D -p ${WP} -v ${VAR1} -m ${OUTSURF}/$i.obj --dir ${DIR} --space VAR
    muse_compute -D -p ${WP} -v ${VAR2} -m ${OUTSURF}/$i.obj --dir ${DIR} --space VAR
  fi
done


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


#multires:
export HIGH=nis_tri_fine
export MID=nis_tri_mid
export COARSE=nis_tri_coarse

if [[ $DIR == 'DIR'* ]]; then
  export COM0=compute/${VAR0}_dir
  export COM1=compute/${VAR1}_dir
  export COM2=compute/${VAR2}_dir
else
  export COM0=compute/${VAR0}_omni
  export COM1=compute/${VAR1}_omni
  export COM2=compute/${VAR2}_omni
fi

export OUTCOM0=${OUTWP}/${COM0}
export OUTCOM1=${OUTWP}/${COM1}
export OUTCOM2=${OUTWP}/${COM2}
  
export OUTVARS0=${OUTCOM0}_${HIGH}/_varspace
export OUTVARS1=${OUTCOM1}_${HIGH}/_varspace
export OUTVARS2=${OUTCOM2}_${HIGH}/_varspace

muse_geometry -D -p ${WP} --refmod ${OUTSURF}/${HIGH}.obj -f ${OUTVARS0}/_stats/${VAR0}_mean.csv --mesh ${OUTSURF}/${MID}.obj --outf ${COM0}_${MID}/_varspace/_stats
muse_geometry -D -p ${WP} --refmod ${OUTSURF}/${HIGH}.obj -f ${OUTVARS1}/_stats/${VAR1}_mean.csv --mesh ${OUTSURF}/${MID}.obj --outf ${COM1}_${MID}/_varspace/_stats
muse_geometry -D -p ${WP} --refmod ${OUTSURF}/${HIGH}.obj -f ${OUTVARS2}/_stats/${VAR2}_mean.csv --mesh ${OUTSURF}/${MID}.obj --outf ${COM2}_${MID}/_varspace/_stats


muse_geometry -D -p ${WP} --refmod ${OUTSURF}/${MID}.obj -f ${OUTCOM0}_${MID}/_varspace/_stats/${VAR0}_mean.csv --mesh ${OUTSURF}/${COARSE}.obj --outf ${COM0}_${COARSE}/_varspace/_stats
muse_geometry -D -p ${WP} --refmod ${OUTSURF}/${MID}.obj -f ${OUTCOM1}_${MID}/_varspace/_stats/${VAR1}_mean.csv --mesh ${OUTSURF}/${COARSE}.obj --outf ${COM1}_${COARSE}/_varspace/_stats
muse_geometry -D -p ${WP} --refmod ${OUTSURF}/${MID}.obj -f ${OUTCOM2}_${MID}/_varspace/_stats/${VAR2}_mean.csv --mesh ${OUTSURF}/${COARSE}.obj --outf ${COM2}_${COARSE}/_varspace/_stats



#utility:
##########  UTILITY  ###########
declare -a arr=("nis_tri" "nis_grid" "nis_tri_fine") #"nis_tri_mid" "nis_tri_coarse")

for i in "${arr[@]}"
do
  if [[ $DIR == 'DIR'* ]]; then
    export OUTCOM0=${OUTWP}/compute/${VAR0}_dir_$i/_varspace
    export OUTCOM1=${OUTWP}/compute/${VAR1}_dir_$i/_varspace
    export OUTCOM2=${OUTWP}/compute/${VAR2}_dir_$i/_varspace
  else
    export OUTCOM0=${OUTWP}/compute/${VAR0}_omni_$i/_varspace
    export OUTCOM1=${OUTWP}/compute/${VAR1}_omni_$i/_varspace
    export OUTCOM2=${OUTWP}/compute/${VAR2}_omni_$i/_varspace
  fi
  
  muse_utility -H --json ${OUTCOM0}/${VAR0}.json --more
  muse_utility -H --json ${OUTCOM1}/${VAR1}.json --more
  muse_utility -H --json ${OUTCOM2}/${VAR2}.json --more
done

####################################################################### MUSE END
)
