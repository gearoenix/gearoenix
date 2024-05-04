#!/bin/bash

text_red=$(tput setaf 1)
text_reset=$(tput sgr0)

if [ "$(uname -s)" == "Darwin" ]; then
  np=$(sysctl -n hw.physicalcpu)
else
  np=$(nproc --all)
fi

pi=0

for ((i = 0; i < np; i++)); do
  pids[$i]=0
done

if command -v clang-format-18 &>/dev/null; then
  clang_format=clang-format-18
else
  clang_format=clang-format
fi

${clang_format} --version

clang_version=$(${clang_format} --version 2>&1)

# Extract the major version number using regular expression matching
regex='([0-9]+)\.[0-9]+\.[0-9]+'
if [[ ${clang_version} =~ ${regex} ]]; then
  major_version=${BASH_REMATCH[1]}
else
  echo "${text_red}Error:  to extract clang-format version.${text_reset}"
  exit 1
fi

expected_version=18

if ((major_version < expected_version)); then
  echo "${text_red}Error clang-format major version is equal to or greater than ${expected_version}.${text_reset}"
  exit 1
fi

format_func() {
  echo "$1"
  ${clang_format} -i -style=WebKit "$1" &
  pids[$pi]=$!
  pi=$((pi + 1))
  pi=$((pi % np))
  if [[ ${pids[$pi]} -ne 0 ]]; then
    wait ${pids[$pi]}
  fi
}

find ./games ./gearoenix ./examples ./tests -regex '.*\.\(cpp\|hpp\|mm\)' -print0 |
  while IFS= read -r -d '' line; do
    format_func "$line"
  done
