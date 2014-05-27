#!/bin/bash - 
#===============================================================================
#
#          FILE: github.sh
# 
#         USAGE: ./github.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 2014年05月27日 11:22
#      REVISION:  ---
#===============================================================================

set -o nounset                             # Treat unset variables as an error
wj_github() {
	git status
	read -p "please enter your changed file:" wj_file
	echo "your git add file:$wj_file"
	git add $wj_file
	read -p "please enter git commit comment info:" wj_comment
	echo $wj_comment
	git commit $wj_file -m "$wj_comment"
	git push origin master
}

alias github=wj_github
