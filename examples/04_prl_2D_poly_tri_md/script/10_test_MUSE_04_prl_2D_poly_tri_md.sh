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
	export DATA_SOURCE=${MUSE}/examples/04_prl_2D_poly_tri_md/data
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
	export PROJ=04_prl_2D_poly_tri_md
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=cr_liguria.csv
fi


# 1. Export variables
#######################################################################
#export GEOM1=FSASSELLO.gpkg
#export GEOM2=FARENZANO.gpkg
declare -a name_geom=("F5TERRE" "FMAGRA" "FPETRONIO" "FPORTOFINO" "FENTELLA" "FAVETO" "FPADANO" "FBISAGNO" "FPOLCEVERA" "FARENZANO" "FSASSELLO" "FBORMIDE" "FSAVONESE" "FIMPERIESE")

export VAR0=cr
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=a10000Y

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
   
        ##########  DATA  ###########
        muse_data -N -p ${WP}
        cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
        muse_data -S -p ${WP}  --setX 1 --setY 2
        muse_data -C -p ${WP}
        
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
    
    ##########  DATA  ###########
    muse_data -N -p ${WP}
    cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
    muse_data -S -p ${WP}  --setX 1 --setY 2
    muse_data -C -p ${WP}

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
export CELLSIZE=1000
export NSTEPDECL=10

muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[0]}_tri --vclean 4 --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL}	###5TERRE
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[1]}_tri --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL} 			###MAGRA
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[2]}_tri --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL}			###PETRONIO
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[3]}_tri --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL}			###PORTOFINO
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[4]}_tri --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL}			###ENTELLA
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[5]}_tri --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL}			###AVETO
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[6]}_tri --vclean 5 --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL}	###PADANO
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[7]}_tri --vclean 3 --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL}	###BISAGNO --nugget 0.01
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[8]}_tri --vclean 5 --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL}	###POLCEVERA
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[9]}_tri --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL} 			###ARENZANO
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[10]}_tri --nugget 0.2 --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL} 	###SASSELLO
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[11]}_tri --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL}			###BORMIDE
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[12]}_tri --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL}			###SAVONESE
muse_vario -V -p ${WP} -v ${VAR0} --nscore YES --vario MODEL --sub ${name_geom[13]}_tri --decl --csize ${CELLSIZE} --nstep ${NSTEPDECL}			###IMPERIESE


#compute:
##########  COMPUTE  ###########
declare -a MIN_VAL=(60 40 50 40 60 60 5 10 10 25 150 5 3 5)
declare -a MAX_VAL=(2100 1600 1800 400 180 930 1300 430 2000 3200 3800 1100 405 130)

###for i in "${name_geom[@]}"
for ((i=0; i<14; i++))
do
   echo ${name_geom[$i]}
   
   declare -a type_geom=("_tri") ### "_grid")
   for j in "${type_geom[@]}"
   do
      echo $i$j
      if [[ $OUTSGS == 'MEAN'* ]]; then
          muse_compute -C -p ${WP} -v ${VAR0} --sub ${name_geom[$i]}${type_geom[0]} -m ${OUTSURF}/${name_geom[$i]}${j}.obj --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr ${MIN_VAL[$i]} --maxextr ${MAX_VAL[i]}
      else
          muse_compute -C -p ${WP} -v ${VAR0} --sub ${name_geom[$i]}${type_geom[0]} -m ${OUTSURF}/${name_geom[$i]}${j}.obj --nsim ${NSIM} --out ${OUTSGS}
          muse_compute -S -p ${WP} -v ${VAR0} --sub ${name_geom[$i]}${type_geom[0]} -m ${OUTSURF}/${name_geom[$i]}${j}.obj --csv
     
          muse_compute -B -p ${WP} -v ${VAR0} --sub ${name_geom[$i]}${type_geom[0]} -m ${OUTSURF}/${name_geom[$i]}${j}.obj --csv --extr Extr --minextr ${MIN_VAL[i]} --maxextr ${MAX_VAL[i]}
     
          muse_compute -S -p ${WP} -v ${VAR0} --sub ${name_geom[$i]}${type_geom[0]} -m ${OUTSURF}/${name_geom[$i]}${j}.obj --space VAR --csv
      fi
   done
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
