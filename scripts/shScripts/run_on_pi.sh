#!/bin/bash
source ./scripts/shScripts/settings.sh

scp "${syBaseFilePath}".s "${syBaseFilePath}".in "${syBaseFilePath}".out \
	"${pi}":"${projPathOnPi}"/

ssh "${pi}" "
cd ${projPathOnPi}
python test.py --ms test.s --in test.in --out test.out --perf
"

scp "${pi}":"${projPathOnPi}/result.json" "${syBaseFilePath}".res.json
