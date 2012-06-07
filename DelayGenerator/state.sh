#!/bin/bash

exit='out' 
exit=$1 

if [[ $exit == out ]]
then
echo 'DO Master'
#wDo
cat Makefile.template > Makefile
else
echo 'Init Master'
cat Makefile.template| sed 's/-DOUTPUT//' > Makefile
fi

sudo make clean; sudo make && sudo make upload && make size

