#!/usr/bin/env bash

GREEN="\033[0;32m"
YELLOW="\033[0;33m"
BLUE="\033[0;34m"
PINK="\033[0;35m"
CYAN="\033[0;36m"
RESET="\033[0m"
BRIGHT="\033[1m"

RESPONSE=""
let ERRORS=0

echo -e "${PINK}${BRIGHT}Testing curl -x DELETE options that should give an error...${RESET}"


echo -en "${YELLOW}images/cat.jpg:${RESET} "
RESPONSE=$("curl" "-X" "DELETE" "localhost/images/cat.jpg")
if grep -q "Not allowed" <<< $RESPONSE; then
	echo -e "$RESPONSE ${GREEN}${BRIGHT}OK${RESET}"
else
	echo -e "$RESPONSE ${RED}${BRIGHT}KO${RESET}"
	let "ERRORS+=1"
fi

echo -en "${YELLOW}images/cat.jpg:${RESET} "
RESPONSE=$("curl" "-X" "DELETE" "localhost/images/cat.jpg")
if grep -q "Not allowed" <<< $RESPONSE; then
	echo -e "$RESPONSE ${GREEN}${BRIGHT}OK${RESET}"
else
	echo -e "$RESPONSE ${RED}${BRIGHT}KO${RESET}"
	let "ERRORS+=1"
fi

echo ""

if (($ERRORS > 0)); then
	echo -e "${RED}${BRIGHT}Amount of tests not passed: $ERRORS${RESET}"
else
	echo -e "${GREEN}${BRIGHT}All tests passed!${RESET}"
fi