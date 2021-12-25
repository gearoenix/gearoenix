#!/bin/bash
np=`nproc --all`
pi=0

for (( i = 0; i < $np; i++ )); do
	pids[$i]=0
done

format_func() {
	echo $1
	clang-format-13 -i -style=WebKit $1 &
	pids[$pi]=$!
	pi=$((pi + 1))
	pi=$((pi % $np))
	if [[ ${pids[$pi]} -ne 0 ]]
	then
		wait ${pids[$pi]}
	fi
}

find ./gearoenix ./examples ./tests -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -print0 | 
	while IFS= read -r -d '' line
	do 
        format_func "$line"
    done