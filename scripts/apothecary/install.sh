#!/bin/bash

# openFrameorks information.
echo "Calling ${BASH_SOURCE[0]} from `pwd`"

# This script assumes it is in the openFrameworks/addons/THE_ADDON/scripts dir.
if [ -z ${OF_ROOT+x} ]; then
  OF_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../../../../" && pwd )"
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

# Check to see if apothecary is already installed.
if ! [ -f $OF_APOTHECARY_PATH/apothecary/apothecary ] ; then
  echo "Apothecary not installed, installing latest version."
  git clone https://github.com/openframeworks/apothecary.git $OF_APOTHECARY_PATH/
else
  pushd $OF_APOTHECARY_PATH/ > /dev/null
  if git rev-parse --is-inside-work-tree ; then
      echo "Apothecary is under git control, updating."
      git pull origin master
  else
      echo "Apothecary is not under git control, so it may not be up-to-date."
  fi
  popd > /dev/null
fi

# Install any apothecary dependencies.
if [ -f $OF_APOTHECARY_PATH/scripts/$TARGET/install.sh ] ; then
  echo "Installing apothecary dependencies."
  $OF_APOTHECARY_PATH/scripts/$TARGET/install.sh
else
  echo "No additional apothecary dependencies to install."
fi

echo "Done installing apothecary."
