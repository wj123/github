#!/bin/bash - 
#===============================================================================
#
#          FILE: jb_dcm-kk_dcm_01.sh
# 
#         USAGE: ./jb_dcm-kk_dcm_01.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 03/13/2014 14:14
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

git checkout jb-lagan-1.1.1

declare -a jbDcmFileDiffName

#jbDcmFileDiffName=`git diff --name-only jb-lagan-1.1.1 jb-lagan-1.1.1-docomo ./common ./fusion3 ./yuga`

git diff --name-only jb-lagan-1.1.1 jb-lagan-1.1.1-docomo ./common ./fusion3 ./yuga > jbDcmFileDiffName.txt
jbDcmFileDiffName=(`cat 'jbDcmFileDiffName.txt'`)

num_file=${#jbDcmFileDiffName[*]}
echo ${num_file}

for ((i=0;i<${num_file};i++)); do
	Namei=${jbDcmFileDiffName[$i]}

	echo "processing "${i}":"${Namei}
	git diff jb-lagan-1.1.1 jb-lagan-1.1.1-docomo -- ${Namei} > jbDcmDiff.tmp

	#base_name=`basename ${Namei}`

	git checkout kk-lagan-docomo

	#find ./ -name ${base_name} > base_name.tmp
	#isExist=`cat base_name.tmp | wc -l`

	if [[ -e ${Namei} ]]; then
		git diff kk-lagan kk-lagan-docomo -- ${Namei} > kkDcmDiff.tmp

		isEmpty=`cat kkDcmDiff.tmp | wc -l`

		if [[ ${isEmpty} -ne 0 ]]; then
			meld jbDcmDiff.tmp kkDcmDiff.tmp &
			echo "Enterd whether has dcm diff bewteen jb and kk,yes or no?"
			read isDiff
			if [[ ${isDiff} = yes ]]; then
				echo "whether no porting,yes or no?"
				read porting
				if [[ ${porting} = no ]]; then
					echo "whether the diff exist in kk-lagan-dcm,yes or no?"
					read isOnlyExistKkDcm
					if [[ ${isOnlyExistKkDcm} = yes  ]]; then
						echo ${Namei} >> OnlyExistInKkDcm
					else
						echo ${Namei} >> jb_dcm-kk_dcm.diff
					fi
				else
					echo ${Namei} >> NoPorting
				fi
			else
				echo ${Namei} >> jb_dcm-kk_dcm.same
			fi
		else
			git checkout kk-lagan
			meld jbDcmDiff.tmp ${Namei} &
			echo "Entered whether jb-dcm exist kk-lagan,yes or no?"
			read isExistMaster
			if [[ ${isExistMaster} = yes ]]; then
				echo ${Namei} >> ExistMaster
			else
				echo ${Namei} >> NoPorting
			fi
		fi
	else
		echo ${Namei} >> kkNoExistFile
	fi
done

rm -rf *.tmp

