#!/usr/bin/env bash

RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
BLUE="\033[0;34m"
PINK="\033[0;35m"
CYAN="\033[0;36m"
RESET="\033[0m"
BRIGHT="\033[1m"

echo "Testing webserv..."

for directory in $(ls error_tests/testfiles/);
	do
		echo ""
		echo -e "${GREEN}${BRIGHT}$directory:${RESET}"
		for file in $(ls error_tests/testfiles/$directory);
		do
		echo -e "${YELLOW}${file}:${RESET}"
		./webserv "error_tests/testfiles/${directory}/${file}"
	done
done

echo ""
