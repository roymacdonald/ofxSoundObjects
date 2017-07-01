#!/bin/bash
set -e

echo "Calling ${BASH_SOURCE[0]} from `pwd`"

# This script assumes it is in the openFrameworks/addons/THE_ADDON/scripts/ci.
SCRIPTS_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../" && pwd )"

echo "Scripts path ${SCRIPTS_PATH}"

echo "Calling ${SCRIPTS_PATH}/build.sh"
$SCRIPTS_PATH/build.sh
