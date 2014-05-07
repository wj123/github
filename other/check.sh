#!/bin/bash - 
#===============================================================================
#
#          FILE: check.sh
# 
#         USAGE: ./check.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 03/18/2014 10:09
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error
a=`pwd`
cd ../../../../system/core/init
for i in `ls *.h`; do
	ret=`find $a -name $i`
	if [[ -n $ret ]]; then
		echo "exist "$i 
	else
		echo "no exist "$i 
	fi
done


