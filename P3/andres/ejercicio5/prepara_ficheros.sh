#!/bin/bash

directorio="$1"

if [ -d "$directorio" ]; then
	rm -r $1


fi

mkdir $1 
cd $1
touch fichero1
touch fichero2
echo "1234567890" >> fichero2   
ln -s ./fichero1 ./enlaceS
ln ./fichero2 ./enlaceH
