#!/bin/bash
RED='\033[0;31m'
NC='\033[0m' # No Color
GREY='\033[1;30m'
GREEN='\033[0;32m'
./compiler < $1 > tmp.ifj
if [ $? -ne 0 ]; then
    printf "${RED}TESTSCRIPT FAILED ON COMPILATON\n"
    exit
fi
./ic20int tmp.ifj > outOur
if [ $? -ne 0 ]; then
    printf "${RED}TESTSCRIPT FAILED ON INTERPRET\n"
    exit
fi
rm tmp.ifj
go run $1 2> outGo
DIFF=$(diff outOur outGo) 
if [ "$DIFF" != "" ] 
then
    printf  "${RED}Output is different! Check the diff ('${NC}diff outOur outGo${RED}')${NC}\n"
    printf "${GREY}$DIFF\n"
    exit
fi
printf "${GREY}output: \n"
cat outOur
printf "\n\n${GREEN}SUCCESS! Your and GO output are same.${NC}\n"
rm outOur
rm outGo
