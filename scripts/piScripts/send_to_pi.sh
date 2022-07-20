#!/bin/bash
source ./scripts/shScripts/settings.sh
files="scripts/piScripts/test.py"

scp -r "${files}" "${pi}":"${projPathOnPi}"
