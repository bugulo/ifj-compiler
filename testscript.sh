#!/bin/bash
./compiler < $1 > tmp.ifj
if [ $? -ne 0 ]; then
    echo "TESTSCRIPT FAILED ON COMPILATON"
    exit
fi
./ic20int tmp.ifj > outOur
if [ $? -ne 0 ]; then
    echo "TESTSCRIPT FAILED ON INTERPRET"
    exit
fi
rm tmp.ifj
go run $1 2> outGo
DIFF=$(diff outOur outGo) 
if [ "$DIFF" != "" ] 
then
    echo "Output is different! Check the diff"
    echo "$DIFF"
    exit
fi
echo "SUCCESS! Your and GO output are same."
rm outOur
rm outGo
