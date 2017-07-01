#!/bin/bash
set -e

# openFrameorks information.
echo "Calling ${BASH_SOURCE[0]} from `pwd`"

# This script assumes it is in the openFrameworks/addons/THE_ADDON/scripts dir.
if [ -z ${OF_ROOT+x} ]; then
  OF_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../../../" && pwd )"
  echo "Setting OF_ROOT to: ${OF_ROOT}"
else
  echo "OF_ROOT is already set to: ${OF_ROOT}"
fi

OF_ADDONS_PATH=$OF_ROOT/addons
OF_SCRIPTS_PATH=$OF_ROOT/scripts
OF_APOTHECARY_PATH=$OF_SCRIPTS_PATH/apothecary

# Addon information.
if [ -z ${ADDON_NAME+x} ]; then
  ADDON_NAME="$(basename $( cd "$( dirname "${BASH_SOURCE[0]}" )/../" && pwd ))"
fi

ADDON_PATH=$OF_ADDONS_PATH/$ADDON_NAME
ADDON_SCRIPTS_PATH=$ADDON_PATH/scripts

# Get the OS type.
TARGET=`${ADDON_SCRIPTS_PATH}/shared/ostype.sh`

echo "openFrameworks Config"
echo "====================="
echo "           OF_ROOT: ${OF_ROOT}"
echo "    OF_ADDONS_PATH: ${OF_ADDONS_PATH}"
echo "   OF_SCRIPTS_PATH: ${OF_SCRIPTS_PATH}"
echo "OF_APOTHECARY_PATH: ${OF_APOTHECARY_PATH}"
echo ""
echo "MACHINE CONFIG"
echo "=============="
echo "           TARGET: ${TARGET}"
echo ""
echo "ADDON CONFIG"
echo "============"
echo "        ADDON_NAME: ${ADDON_NAME}"
echo "        ADDON_PATH: ${ADDON_PATH}"
echo "ADDON_SCRIPTS_PATH: ${ADDON_SCRIPTS_PATH}"
echo ""

echo "Building for ${TARGET} ..."

# Install any library dependencies.
if [ -f $ADDON_SCRIPTS_PATH/dependencies/$TARGET/install.sh ] ; then
  echo "Installing ${ADDON_NAME} dependencies for ${TARGET} ..."
  $ADDON_SCRIPTS_PATH/dependencies/$TARGET/install.sh
else
  echo "No special ${ADDON_NAME} dependencies required for ${TARGET}."
fi

# Install or update apothecary.
echo "Installing apothecary ..."
$ADDON_SCRIPTS_PATH/apothecary/install.sh

# # Clean any prior builds.
echo "Cleaning prior apothecary builds for ${ADDON_NAME} ..."
$OF_APOTHECARY_PATH/apothecary/apothecary -v clean $ADDON_NAME

# Build using apothcary
echo "Building ${ADDON_NAME} libraries for ${TARGET} ..."
$OF_APOTHECARY_PATH/apothecary/apothecary -v -j10 -d $ADDON_PATH/libs update $ADDON_NAME
$OF_APOTHECARY_PATH/apothecary/apothecary -v clean $ADDON_NAME
echo "Build of ${ADDON_NAME} complete for ${TARGET}."

