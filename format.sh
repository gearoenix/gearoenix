#!/bin/bash
if [ "$(uname -s)" == "Darwin" ]
then
    np=$(sysctl -n hw.physicalcpu)
else
    np=$(nproc --all)
fi

pi=0

for (( i = 0; i < np; i++ )); do
	pids[$i]=0
done

format_func() {
	echo "$1"
	clang-format-15 -i -style=WebKit "$1" &
	pids[$pi]=$!
	pi=$((pi + 1))
	pi=$((pi % np))
	if [[ ${pids[$pi]} -ne 0 ]]
	then
		wait ${pids[$pi]}
	fi
}

find ./games ./gearoenix ./examples ./tests -regex '.*\.\(cpp\|hpp\|cc\|mm\|cxx\)' -print0 |
	while IFS= read -r -d '' line
	do 
        format_func "$line"
    done