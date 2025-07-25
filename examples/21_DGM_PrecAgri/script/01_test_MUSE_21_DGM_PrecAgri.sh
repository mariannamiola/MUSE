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
	export DATA_SOURCE=${MUSE}/examples/21_DGM_PrecAgri/data
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
	export PROJ=21_DGM_PrecAgri
fi

if [ "$OPTDATA" ]	#if a data filename is provided
then
	export DATA=${OPTDATA}
else
	export DATA=data.csv
fi


# 1. Export variables
#######################################################################
export GEOM=area ##area

export VAR0=H
export VAR1=T
#######################################################################


# 2. Export flags
#######################################################################
#For geometry
export OPT=a1000
export DELTA=-1 ##- 1 metro
export OPTVOL=qY

#For vario
export DIR=DIR
export DIM=3Dxy

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
        cp -R ${DATA_SOURCE}/../${GEOM}.xyz ${INGEOM}
        muse_geometry -P -p ${WP} --tri --points ${INGEOM}/${GEOM}.xyz --concave --obj ##--opt ${OPT} --meth MEAN --obj
        #muse_geometry -L -p ${WP} -m ${OUTSURF}/${GEOM}.obj --splmet EDGE --obj
        #muse_geometry -L -p ${WP} -m ${OUTSURF}/${GEOM}_res.obj --splmet EDGE --obj
        #mv ${OUTSURF}/${GEOM}_res.obj ${OUTSURF}/${GEOM}.obj
        #mv ${OUTSURF}/${GEOM}_res.json ${OUTSURF}/${GEOM}.json
                
        muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --delta -z -0.20 --obj
        mv ${OUTSURF}/${GEOM}_dz.obj ${OUTSURF}/${GEOM}1.obj
        mv ${OUTSURF}/${GEOM}_dz.json ${OUTSURF}/${GEOM}1.json
        muse_geometry -T -p ${WP} -m ${OUTSURF}/${GEOM}.obj -m ${OUTSURF}/${GEOM}1.obj --obj
        mv ${OUTSURF}/${GEOM}-${GEOM}1.obj ${OUTSURF}/${GEOM}01.obj
        mv ${OUTSURF}/${GEOM}-${GEOM}1.json ${OUTSURF}/${GEOM}01.json
        
        muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --delta -z -0.40 --obj
        mv ${OUTSURF}/${GEOM}_dz.obj ${OUTSURF}/${GEOM}2.obj
        mv ${OUTSURF}/${GEOM}_dz.json ${OUTSURF}/${GEOM}2.json
        muse_geometry -T -p ${WP} -m ${OUTSURF}/${GEOM}1.obj -m ${OUTSURF}/${GEOM}2.obj --obj
        mv ${OUTSURF}/${GEOM}1-${GEOM}2.obj ${OUTSURF}/${GEOM}12.obj
        mv ${OUTSURF}/${GEOM}1-${GEOM}2.json ${OUTSURF}/${GEOM}12.json
    	
    	muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --delta -z -0.60 --obj
        mv ${OUTSURF}/${GEOM}_dz.obj ${OUTSURF}/${GEOM}3.obj
        mv ${OUTSURF}/${GEOM}_dz.json ${OUTSURF}/${GEOM}3.json
        muse_geometry -T -p ${WP} -m ${OUTSURF}/${GEOM}2.obj -m ${OUTSURF}/${GEOM}3.obj --obj
        mv ${OUTSURF}/${GEOM}2-${GEOM}3.obj ${OUTSURF}/${GEOM}23.obj
        mv ${OUTSURF}/${GEOM}2-${GEOM}3.json ${OUTSURF}/${GEOM}23.json
        
        muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --delta -z -0.80 --obj
        mv ${OUTSURF}/${GEOM}_dz.obj ${OUTSURF}/${GEOM}4.obj
        mv ${OUTSURF}/${GEOM}_dz.json ${OUTSURF}/${GEOM}4.json
        muse_geometry -T -p ${WP} -m ${OUTSURF}/${GEOM}3.obj -m ${OUTSURF}/${GEOM}4.obj --obj
        mv ${OUTSURF}/${GEOM}3-${GEOM}4.obj ${OUTSURF}/${GEOM}34.obj
        mv ${OUTSURF}/${GEOM}3-${GEOM}4.json ${OUTSURF}/${GEOM}34.json
        
        muse_geometry -U -p ${WP} -m ${OUTSURF}/${GEOM}01.obj -m ${OUTSURF}/${GEOM}12.obj --obj
        mv ${OUTSURF}/${GEOM}01_${GEOM}12.obj ${OUTSURF}/tmp.obj
    
    	muse_geometry -U -p ${WP} -m ${OUTSURF}/tmp.obj -m ${OUTSURF}/${GEOM}23.obj --obj
    	mv ${OUTSURF}/tmp_${GEOM}23.obj ${OUTSURF}/tmp.obj

    	muse_geometry -U -p ${WP} -m ${OUTSURF}/tmp.obj -m ${OUTSURF}/${GEOM}34.obj --obj
    	mv ${OUTSURF}/tmp_${GEOM}34.obj ${OUTSURF}/${GEOM}_merge.obj
    	    	
    	muse_geometry -M -p ${WP} -m ${OUTSURF}/${GEOM}_merge.obj --tet --vtk    	
    	
        ##########  DATA  ###########
        muse_data -N -p ${WP}
        cp -R ${DATA_SOURCE}/*.csv ${INDATA}
        muse_data -S -p ${WP} --setX 2 --setY 3 --setZ 4
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
        cp -R ${DATA_SOURCE}/../${GEOM}.xyz ${INGEOM}
        muse_geometry -P -p ${WP} --tri --points ${INGEOM}/${GEOM}.xyz --concave --obj ##--opt ${OPT} --meth MEAN --obj
        #muse_geometry -L -p ${WP} -m ${OUTSURF}/${GEOM}.obj --splmet EDGE --obj
        #muse_geometry -L -p ${WP} -m ${OUTSURF}/${GEOM}_res.obj --splmet EDGE --obj
        #mv ${OUTSURF}/${GEOM}_res.obj ${OUTSURF}/${GEOM}.obj
        #mv ${OUTSURF}/${GEOM}_res.json ${OUTSURF}/${GEOM}.json
        
                
        muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --delta -z -0.20 --obj
        mv ${OUTSURF}/${GEOM}_dz.obj ${OUTSURF}/${GEOM}1.obj
        mv ${OUTSURF}/${GEOM}_dz.json ${OUTSURF}/${GEOM}1.json
        muse_geometry -T -p ${WP} -m ${OUTSURF}/${GEOM}.obj -m ${OUTSURF}/${GEOM}1.obj --obj
        mv ${OUTSURF}/${GEOM}-${GEOM}1.obj ${OUTSURF}/${GEOM}01.obj
        mv ${OUTSURF}/${GEOM}-${GEOM}1.json ${OUTSURF}/${GEOM}01.json
        
        muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --delta -z -0.40 --obj
        mv ${OUTSURF}/${GEOM}_dz.obj ${OUTSURF}/${GEOM}2.obj
        mv ${OUTSURF}/${GEOM}_dz.json ${OUTSURF}/${GEOM}2.json
        muse_geometry -T -p ${WP} -m ${OUTSURF}/${GEOM}1.obj -m ${OUTSURF}/${GEOM}2.obj --obj
        mv ${OUTSURF}/${GEOM}1-${GEOM}2.obj ${OUTSURF}/${GEOM}12.obj
        mv ${OUTSURF}/${GEOM}1-${GEOM}2.json ${OUTSURF}/${GEOM}12.json
    	
    	muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --delta -z -0.60 --obj
        mv ${OUTSURF}/${GEOM}_dz.obj ${OUTSURF}/${GEOM}3.obj
        mv ${OUTSURF}/${GEOM}_dz.json ${OUTSURF}/${GEOM}3.json
        muse_geometry -T -p ${WP} -m ${OUTSURF}/${GEOM}2.obj -m ${OUTSURF}/${GEOM}3.obj --obj
        mv ${OUTSURF}/${GEOM}2-${GEOM}3.obj ${OUTSURF}/${GEOM}23.obj
        mv ${OUTSURF}/${GEOM}2-${GEOM}3.json ${OUTSURF}/${GEOM}23.json
        
        muse_geometry -O -p ${WP} -m ${OUTSURF}/${GEOM}.obj --delta -z -0.80 --obj
        mv ${OUTSURF}/${GEOM}_dz.obj ${OUTSURF}/${GEOM}4.obj
        mv ${OUTSURF}/${GEOM}_dz.json ${OUTSURF}/${GEOM}4.json
        muse_geometry -T -p ${WP} -m ${OUTSURF}/${GEOM}3.obj -m ${OUTSURF}/${GEOM}4.obj --obj
        mv ${OUTSURF}/${GEOM}3-${GEOM}4.obj ${OUTSURF}/${GEOM}34.obj
        mv ${OUTSURF}/${GEOM}3-${GEOM}4.json ${OUTSURF}/${GEOM}34.json
        
        muse_geometry -U -p ${WP} -m ${OUTSURF}/${GEOM}01.obj -m ${OUTSURF}/${GEOM}12.obj --obj
        mv ${OUTSURF}/${GEOM}01_${GEOM}12.obj ${OUTSURF}/tmp.obj
    
    	muse_geometry -U -p ${WP} -m ${OUTSURF}/tmp.obj -m ${OUTSURF}/${GEOM}23.obj --obj
    	mv ${OUTSURF}/tmp_${GEOM}23.obj ${OUTSURF}/tmp.obj

    	muse_geometry -U -p ${WP} -m ${OUTSURF}/tmp.obj -m ${OUTSURF}/${GEOM}34.obj --obj
    	mv ${OUTSURF}/tmp_${GEOM}34.obj ${OUTSURF}/${GEOM}_merge.obj

    	muse_geometry -M -p ${WP} -m ${OUTSURF}/${GEOM}_merge.obj --tet --vtk    	

        
    ##########  DATA  ###########
    muse_data -N -p ${WP}
    cp -R ${DATA_SOURCE}/*.csv ${INDATA}
    muse_data -S -p ${WP} --setX 2 --setY 3 --setZ 4
    muse_data -C -p ${WP}
fi

#manip:
##########  MANIPULATE  ###########
export GEOMNAME=${GEOM}_merge

muse_manipulate -E -p ${WP} --geom ${OUTSURF}/${GEOMNAME}.obj

#muse_manipulate -V -p ${WP} --type SAMPLES --sub ${GEOMNAME} --mgeom ${OUTVOL}/${GEOMNAME}.vtk --prdir Z
muse_manipulate -V -p ${WP} --type SAMPLES --sub ${GEOMNAME} --reggrow --mgeom ${OUTVOL}/${GEOMNAME}.vtk --prdir Z

#muse_manipulate -V -p ${WP} --type GEOMETRY --geom ${OUTVOL}/${GEOMNAME}.vtk --mgeom ${OUTVOL}/${GEOMNAME}.vtk --prdir Z
muse_manipulate -V -p ${WP} --type TET --reggrow --geom ${OUTVOL}/${GEOMNAME}.vtk --prdir Z


#strat:
muse_manipulate -T -p ${WP} --sttype PROPORTIONAL --type SAMPLES --sub ${GEOMNAME} --top ${GEOMNAME}_top --bot ${GEOMNAME}_bot --name ${GEOMNAME} --prdir Z
muse_manipulate -T -p ${WP} --sttype PROPORTIONAL --type VOLUME --top ${GEOMNAME}_top --bot ${GEOMNAME}_bot --geom ${OUTVOL}/${GEOMNAME}.vtk --vtk --name ${GEOMNAME} --prdir Z


#vario:
##########  VARIO  ###########
muse_vario -V -p ${WP} -v ${VAR0} --sttype PROPORTIONAL -f samples_${GEOMNAME} --sub ${GEOMNAME} --nscore YES --dir ${DIR} --dim ${DIM} --vario MODEL --zdegtol 5 --eps 5.0 --epsy 0.5 --dirs 90,112.5,135,157.5 --degtol 30 ##--vario MODEL --zdegtol 5 --eps 5.0 --maxdist 160 --vclean 5 --weight
muse_vario -V -p ${WP} -v ${VAR0} --sttype PROPORTIONAL -f samples_${GEOMNAME} --sub ${GEOMNAME} --nscore YES --dir ${DIR} --dim 3Dz --vario MODEL --lagspac FIXED --spac 0.10 --zdegtol 5.0


#compute:
##########  COMPUTE  ###########
export zrange=0.20


if [[ $OUTSGS == 'MEAN'* ]]; then
  muse_compute -C -p ${WP} -v ${VAR0} --sub ${GEOMNAME} -m ${OUTVOL}/${GEOMNAME}.vtk --nsim ${NSIM} --out ${OUTSGS} --bnscore --extr Extr --minextr 0 --maxextr 100 --dir ${DIR} --dim ${DIM} --zrange ${zrange}
else
  muse_compute -C -p ${WP} -v ${VAR0} --sub ${GEOMNAME} -m ${OUTVOL}/${GEOMNAME}.vtk --nsim ${NSIM} --out ${OUTSGS} --dir ${DIR} --dim ${DIM} --zrange ${zrange}

  muse_compute -S -p ${WP} -v ${VAR0} --sub ${GEOMNAME} -m ${OUTVOL}/${GEOMNAME}.vtk --csv --dir ${DIR} --dim ${DIM}

  #back:
  muse_compute -B -p ${WP} -v ${VAR0} --sub ${GEOMNAME} -m ${OUTVOL}/${GEOMNAME}.vtk --extr Extr --minextr 0 --maxextr 100 --csv --dir ${DIR} --dim ${DIM}

  #statsback:
  muse_compute -S -p ${WP} -v ${VAR0} --sub ${GEOMNAME} -m ${OUTVOL}/${GEOMNAME}.vtk --space VAR --csv --dir ${DIR} --dim ${DIM}
fi


 
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


#plot:
##########  PLOT  ###########
#-H -p ${WP} -v <FILE>

####################################################################### MUSE END
)
