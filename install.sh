#!/bin/bash

RED='\033[0;31m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
RST='\033[0m'
IT='\e[3m'
ITRST='\e[0m'

# Check if script is running with privileges
if [ $(id -u) -ne 0 ]; then
	printf "\n$IT${RED}A tool as powerful as kimsay requires powerful means of installation.\n$ITRST${YELLOW}Please run the script with sudo!$RST\n\n"
	exit 1
fi

# Check if the necessary programs are present
if ! [ -x "$(command -v c++)" ]; then
	printf "\n${RED}Couldn't find a valid c++ compiler :($RST\n\n"
	exit 1
fi

BIN=/usr/games
DIALOG=/usr/share/kimsay/dialog
PORTRAITS=/usr/share/kimsay/portraits
MANDIR=/usr/share/man/man6

printf "\nCompiling kimsay..."
if ! c++ kimsay.cpp -o kimsay; then
	printf "\n${RED}Compilation failed :($RST\n\n"
	exit 1
fi
printf "\n\t${CYAN}Compilation done!$RST\n"
mkdir -p $BIN
mv kimsay $BIN/kimsay

mkdir -p $PORTRAITS
cp art/kim $PORTRAITS/kim
printf "\n${CYAN}Art files created~$RST"

mkdir -p $DIALOG
cp dialog/kim.json $DIALOG/kim.json
printf "\n${CYAN}Dialog files created~$RST"

mkdir -p $MANDIR
cp man/man6/kimsay.6 $MANDIR/
printf "\n${CYAN}Manpage created~$RST"

printf "\n\nYou now have access to the wisdom of 57's finest. Don't let him down."