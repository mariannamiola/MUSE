#!/bin/bash
#
# Validation test for the FULL 3D anisotropic variogram (ellipsoid) workflow.
#
# A synthetic Gaussian Random Field with KNOWN 3D anisotropy (exact Cholesky simulation,
# exponential model, setrot/GSLIB angle convention) is packaged as a MUSE project,
# muse_vario is run with --dim 3D --dir DIR --vario MODEL, and the fitted ellipsoid
# is finally compared with the ground truth parameters.
#
# Ground truth (see 00_generate_data.py): ranges 60/30/12, azimuth 30, roll 15,
# pitch 0, nugget 0.1, sill 0.9, EXPONENTIAL model.

set -e  #exit if an error occurs

######################################################
################ PROJECT NAME ########################

FILENAME=$(basename "$0")           # name of the running script
NAME="${FILENAME%.*}"               # remove the .sh extension
export setPROJECT_NAME="${NAME#10_test_MUSE_}"
echo "Project name (automatically) set to: $setPROJECT_NAME"

######################################################
################ PATHS ###############################

#### Directory (full path) where this script lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

export MUSE=${SCRIPT_DIR}/../../..

# Prefer freshly built binaries (root build tree), fall back to the bin symlinks
for app in muse_project muse_data muse_vario; do
  if [ -x "${MUSE}/build/${app}/${app}" ]; then
    export PATH="${MUSE}/build/${app}:$PATH"
  fi
done
export PATH=${MUSE}/bin:$PATH

export DATA_SOURCE=${MUSE}/examples/${setPROJECT_NAME}/data
export WORK=${1:-${MUSE}/examples/MUSE_test}
export PROJ=${setPROJECT_NAME}
export DATA=data.csv
export VAR=phi

# Project folders
export WP=${WORK}/${PROJ}
export INDATA=${WP}/in/data
export OUTVARIO=${WP}/out/vario

mkdir -p ${WORK}

######################################################
################ 0. GROUND TRUTH DATA ################

# Generate the synthetic dataset only once (fixed seed: fully reproducible)
if [ ! -f "${DATA_SOURCE}/${DATA}" ] || [ ! -f "${DATA_SOURCE}/ground_truth.json" ]; then
  echo "Generating synthetic ground-truth dataset ..."
  python3 ${SCRIPT_DIR}/00_generate_data.py
fi

######################################################
################ 1. PROJECT + DATA ###################

muse_project -N -p ${WORK} --name ${PROJ} --overwrite

muse_data -N -p ${WP}
cp -R ${DATA_SOURCE}/${DATA} ${INDATA}
muse_data -S -p ${WP} --setX 1 --setY 2 --setZ 3
muse_data -C -p ${WP}

######################################################
################ 2. VARIO 3D (ELLIPSOID) #############

# Full 3D directional variogram on the AUTO orientation grid (--pstep 45 -> 13 planes),
# 6 in-plane scan directions (30 degree step), variable lag spacing
muse_vario -V -p ${WP} -v ${VAR} --nscore YES --vario MODEL --dim 3D --dir DIR \
           --deg 30 --degtol 15 --zdegtol 15 --lagspac VARIABLE \
           --pstep 30 --eps 5.0  

######################################################
################ 3. GROUND TRUTH COMPARISON ##########

python3 ${SCRIPT_DIR}/20_compare_ground_truth.py \
        ${DATA_SOURCE}/ground_truth.json \
        ${OUTVARIO}/${VAR}_DIR3D/${VAR}.json
