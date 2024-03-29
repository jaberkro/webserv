#!/usr/bin/env bash

RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
BLUE="\033[0;34m"
PINK="\033[0;35m"
CYAN="\033[0;36m"
RESET="\033[0m"
BRIGHT="\033[1m"

OUTPUT=""

let ERRORS=0

echo -e "${PINK}${BRIGHT}Testing webserv configuration files that should give an error...${RESET}"

chmod 000 error_tests/invalid/00_open/03_no_access.conf

echo -e "${BRIGHT}"non-existent file:"${RESET}"
echo -en "${YELLOW}non_existent.conf:${RESET} "
OUTPUT=$(./webserv "error_tests/invalid/non_existent.conf" 2>&1) 
if grep -q "Error: " <<< $OUTPUT; then
	echo -e "$OUTPUT ${GREEN}${BRIGHT}OK${RESET}"
else
	echo -e "$OUTPUT ${RED}${BRIGHT}KO${RESET}"
	let "ERRORS+=1"
fi

for directory in $(ls error_tests/invalid/);
	do
	echo ""
	echo -e "${BRIGHT}$directory:${RESET}"
	for file in $(ls error_tests/invalid/$directory);
		do
		echo -en "${YELLOW}${file}:${RESET} "
		OUTPUT=$(./webserv "error_tests/invalid/${directory}/${file}" 2>&1) 
		if grep -q "Error: " <<< $OUTPUT; then
			echo -e "$OUTPUT ${GREEN}${BRIGHT}OK${RESET}"
		else
			echo -e "$OUTPUT ${RED}${BRIGHT}KO${RESET}"
			let "ERRORS+=1"
		fi
	done
done


echo ""

echo -e "${PINK}${BRIGHT}Testing webserv configuration files that should not give an error...${RESET}"

for directory in $(ls error_tests/valid/);
	do
	echo ""
	echo -e "${BRIGHT}$directory:${RESET}"
	for file in $(ls error_tests/valid/$directory);
		do
		echo -en "${YELLOW}${file}:${RESET} "
		OUTPUT=$(./webserv "error_tests/valid/${directory}/${file}" 2>&1) 
		if grep -q "Error: " <<< $OUTPUT; then
			echo -e "$OUTPUT ${RED}${BRIGHT}KO${RESET}"
			let "ERRORS+=1"
		else
			echo -e "$OUTPUT ${GREEN}${BRIGHT}OK${RESET}"
		fi
	done
done

echo ""

if (($ERRORS > 0)); then
	echo -e "${RED}${BRIGHT}Amount of tests not passed: $ERRORS${RESET}"
else
	echo -e "${GREEN}${BRIGHT}All tests passed! $ERRORS errors${RESET}"
fi

chmod 755 error_tests/invalid/00_open/03_no_access.conf
