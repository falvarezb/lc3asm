#!/bin/bash

CWD=$(pwd)
BASEDIR=$(dirname "$0")
JAR_FILE="LC3sim.jar"
echo "changing to $BASEDIR"
cd "$BASEDIR" || exit

if [[ ! -e $JAR_FILE ]]; then
    echo "Downloading $JAR_FILE"
    wget https://acg.cis.upenn.edu/milom/cse240-Fall05/handouts/code/$JAR_FILE
fi

echo "changing back to $CWD"
cd "$CWD" || exit

java -jar "$BASEDIR"/$JAR_FILE &