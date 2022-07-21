#!/bin/bash
source ./scripts/shScripts/settings.sh

scp -r "${piFiles}" "${pi}":"${projPathOnPi}"
