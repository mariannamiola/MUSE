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
	export DATA_SOURCE=${MUSE}/examples/04_prl_2D_poly_grid_tri/data
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
	export PROJ=04_prl_2D_poly_grid_tri
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=cr_10-11.csv
fi


# 1. Export variables
#######################################################################
#export GEOM1=FSASSELLO.gpkg
#export GEOM2=FARENZANO.gpkg
declare -a name_geom=("FSASSELLO" "FARENZANO")

export VAR0=cr
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=a10000
export BBP1=1447000,4908200,0
export BBP2=1490400,4908200,0
export BBP3=1490400,4936600,0
export BBP4=1447000,4936600,0

#For vario
export DIR=OMNI
export DIM=3D

#For compute
if [ "$OPTSIM" ]	#if a number of simulations is provided
then
	export NSIM=$OPTSIM
else
	export NSIM=50
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
# Export folders
export WP=${WORK}/${PROJ}

export INWP=${WP}/in

export INDATA=${INWP}/data
export INGEOM=${INWP}/geometry

export OUTWP=${WP}/out

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

        for g in "${name_geom[@]}"
        do
            cp -R ${DATA_SOURCE}/${g}.gpkg ${INGEOM}
        done

        muse_geometry -V -p ${WP} --tri --opt ${OPT} --obj
        for g in "${name_geom[@]}"
        do
            mv ${OUTSURF}/${g}.obj ${OUTSURF}/${g}_tri.obj
            mv ${OUTSURF}/${g}.json ${OUTSURF}/${g}_tri.json
        done

        ### Command for creating rectangular grid
        muse_geometry -G -p ${WP} --grid --resx 200 --resy 200 --bbp ${BBP1} --bbp ${BBP2} --bbp ${BBP3} --bbp ${BBP4} --obj

        ### Command for extracting grid mesh from rectangular grid, respecting boundaries of triangular meshes
        for g in "${name_geom[@]}"
        do
            muse_geometry -S -p ${WP} --grid -m ${OUTSURF}/grid.obj --boundary ${OUTSURF}/${g}_tri.obj --obj
            mv ${OUTSURF}/grid_${g}_tri.obj ${OUTSURF}/${g}_grid.obj
            mv ${OUTSURF}/grid_${g}_tri.json ${OUTSURF}/${g}_grid.json
        done

        muse_geometry -U -p ${WP} -m ${OUTSURF}/${name_geom[0]}_tri.obj -m ${OUTSURF}/${name_geom[1]}_tri.obj --obj
        muse_geometry -U -p ${WP} -m ${OUTSURF}/${name_geom[0]}_grid.obj -m ${OUTSURF}/${name_geom[1]}_grid.obj --obj
        
        ##########  DATA  ###########
        muse_data -N -p ${WP}
        cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
        muse_data -S -p ${WP} --setX 1 --setY 2 --setZ 3
        muse_data -C -p ${WP}
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

    for g in "${name_geom[@]}"
    do
        cp -R ${DATA_SOURCE}/${g}.gpkg ${INGEOM}
    done

    muse_geometry -V -p ${WP} --tri --opt ${OPT} --obj
    for g in "${name_geom[@]}"
    do
        mv ${OUTSURF}/${g}.obj ${OUTSURF}/${g}_tri.obj
        mv ${OUTSURF}/${g}.json ${OUTSURF}/${g}_tri.json
    done

    ### Command for creating rectangular grid
    muse_geometry -G -p ${WP} --grid --resx 200 --resy 200 --bbp ${BBP1} --bbp ${BBP2} --bbp ${BBP3} --bbp ${BBP4} --obj

    ### Command for extracting grid mesh from rectangular grid, respecting boundaries of triangular meshes
    for g in "${name_geom[@]}"
    do
        muse_geometry -S -p ${WP} --grid -m ${OUTSURF}/grid.obj --boundary ${OUTSURF}/${g}_tri.obj --obj
        mv ${OUTSURF}/grid_${g}_tri.obj ${OUTSURF}/${g}_grid.obj
        mv ${OUTSURF}/grid_${g}_tri.json ${OUTSURF}/${g}_grid.json
    done

    muse_geometry -U -p ${WP} -m ${OUTSURF}/${name_geom[0]}_tri.obj -m ${OUTSURF}/${name_geom[1]}_tri.obj --obj
    muse_geometry -U -p ${WP} -m ${OUTSURF}/${name_geom[0]}_grid.obj -m ${OUTSURF}/${name_geom[1]}_grid.obj --obj
    
    ##########  DATA  ###########
    muse_data -N -p ${WP}
    cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
    muse_data -S -p ${WP} --setX 1 --setY 2 --setZ 3
    muse_data -C -p ${WP}
fi



#manip:
##########  MANIPULATE  ###########
for g in "${name_geom[@]}"
do
    muse_manipulate -E -p ${WP} --geom ${OUTSURF}/${g}_tri.obj
done


#vario:
##########  VARIO  ###########

#### Declustering parameters:
export CELLSIZE=2400
export NSTEPDECL=10

muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --nugget 0.2 --sub ${name_geom[0]}_tri --vclean 4 --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL} 	###SASSELLO
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[1]}_tri --vclean 4 --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL} 			###ARENZANO


#compute:
##########  COMPUTE  ###########
###SASSELLO
declare -a arr_sas=("${name_geom[0]}_tri" "${name_geom[0]}_grid")
for i in "${arr_sas[@]}"
do
  #export OUTCOM=${OUTWP}/compute/${VAR0}_${arr_sas[0]}_${DIR}${DIM}_${i}
  
  #export OUTNORMS=${OUTCOM}/_normspace
  #export OUTVARS=${OUTCOM}/_varspace
  

  if [[ $OUTSGS == 'MEAN'* ]]; then
     muse_compute -C -p ${WP} -v ${VAR0} --sub ${arr_sas[0]} -m ${OUTSURF}/$i.obj --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 10 --maxextr 4000
  else
     muse_compute -C -p ${WP} -v ${VAR0} --sub ${arr_sas[0]} -m ${OUTSURF}/$i.obj --nsim ${NSIM} --out ${OUTSGS}
     muse_compute -S -p ${WP} -v ${VAR0} --sub ${arr_sas[0]} -m ${OUTSURF}/$i.obj --csv
     
     muse_compute -B -p ${WP} -v ${VAR0} --sub ${arr_sas[0]} -m ${OUTSURF}/$i.obj --extr Extr --minextr 10 --maxextr 4000 --csv
          
     muse_compute -S -p ${WP} -v ${VAR0} --sub ${arr_sas[0]} -m ${OUTSURF}/$i.obj --space VAR --csv
  fi
  
  #db:
  if [[ $OUTSGS == 'MEAN'* ]]; then
    echo "Dataset creation for MEAN SGS method ... NOT IMPLEMENTED!"
  else
     muse_compute -D -p ${WP} -v ${VAR0} --sub ${arr_sas[0]} -m ${OUTSURF}/$i.obj --space VAR
  fi
done


###ARENZANO
declare -a arr_are=("${name_geom[1]}_tri" "${name_geom[1]}_grid")
for i in "${arr_are[@]}"
do

  if [[ $OUTSGS == 'MEAN'* ]]; then
     muse_compute -C -p ${WP} -v ${VAR0} --sub ${arr_are[0]} -m ${OUTSURF}/$i.obj --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 1 --maxextr 3500
  else
     muse_compute -C -p ${WP} -v ${VAR0} --sub ${arr_are[0]} -m ${OUTSURF}/$i.obj --nsim ${NSIM} --out ${OUTSGS}
     muse_compute -S -p ${WP} -v ${VAR0} --sub ${arr_are[0]} -m ${OUTSURF}/$i.obj --csv
     
     muse_compute -B -p ${WP} -v ${VAR0} --sub ${arr_are[0]} -m ${OUTSURF}/$i.obj --extr Extr --minextr 1 --maxextr 3500 --csv
     #for (( id=0; id<$NSIM; id++ ))
     #do
     #  muse_compute -B -p ${WP} -v ${VAR0} --sub ${arr_are[0]} -m ${OUTSURF}/$i.obj --extr Extr --minextr 1 --maxextr 3500 -f ${OUTNORMS}/sgs_$id.csv --csv
     #done
     
     muse_compute -S -p ${WP} -v ${VAR0} --sub ${arr_are[0]} -m ${OUTSURF}/$i.obj --space VAR --csv
  fi
  
  #db:
  if [[ $OUTSGS == 'MEAN'* ]]; then
    echo "Dataset creation for MEAN SGS method ... NOT IMPLEMENTED!"
  else
     muse_compute -D -p ${WP} -v ${VAR0} --sub ${arr_are[0]} -m ${OUTSURF}/$i.obj --space VAR
  fi
done


#cat:
declare -a type=("tri" "grid")
for t in "${type[@]}"
do
    export OUTCOM_sas=${OUTWP}/compute/${VAR0}_${arr_sas[0]}_${DIR}${DIM}_${name_geom[0]}_${t}
    export OUTNORMS_sas=${OUTCOM_sas}/_normspace
    export OUTVARS_sas=${OUTCOM_sas}/_varspace
    
    export OUTCOM_are=${OUTWP}/compute/${VAR0}_${arr_are[0]}_${DIR}${DIM}_${name_geom[1]}_${t}
    export OUTNORMS_are=${OUTCOM_are}/_normspace
    export OUTVARS_are=${OUTCOM_are}/_varspace
  
    if [[ $OUTSGS == 'MEAN'* ]]; then
       echo "Merging SGS results for MEAN SGS method ... NOT IMPLEMENTED!"
      #cat ${OUTCOM}/${VAR}_${SURF1}.csv ${OUTCOM2}/${VAR}_${SURF2}.csv ${OUTCOM3}/${VAR}_${SURF3}.csv > ${OUTWP}/compute/${VAR}_merge.csv
    else
       cat ${OUTVARS_sas}/_stats/${VAR0}_mean.csv ${OUTVARS_are}/_stats/${VAR0}_mean.csv > ${OUTWP}/compute/${VAR0}_merge_${t}.csv
    fi
done


#clean:
#echo ${SCRIPT_DIR}
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





#plot:
##########  PLOT  ###########
#-H -p ${WP} -v <FILE>

####################################################################### MUSE END

)
