#!/bin/bash

exit='burst' 
exit=$1 

if [[ $exit == burst ]]
then
echo 'Master as CALIBRATION'
#wDo
cat Makefile.template > Makefile
else
echo 'Master as BURST'
cat Makefile.template| sed 's/-DBURST//' > Makefile
fi

sudo make clean; sudo make && sudo make upload && make size

