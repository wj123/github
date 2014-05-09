#!/bin/bash - 
#===============================================================================
#
#          FILE: ubuntu-Common-Sony-Env.sh
# 
#         USAGE: ./ubuntu-Common-Sony-Env.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 2014年05月08日 16:52
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

home_dir=`whoami`
wj_pwd=`pwd`

#install jdk
check_jdk=`java -version`
if [[ -z "$check_jdk" ]]; then
	echo -n "install JDK , Please enter any key to continue: "
	sudo apt-get install openjdk-7-jdk
fi

#install sdk


#install git/repo
check_git=`git --version`
if [[ -z $check_git ]]; then
	echo -n "install git , Please enter any key to continue: "
	sudo apt-get install git-core
fi

check_repo=`which repo`
if [[ -z $check_repo ]]; then
	echo -n "install repo , Please enter any key to continue: "
	git clone git://code.thunderst.com/tools/repo.git
		if [[ ! -d /home/$home_dir/bin ]]; then
			cd /home/$home_dir/ && mkdir bin/
			cd wj_pwd
			cp repo/repo /home/$home_dir/bin/
			echo "export PATH=~/bin:$PATH" >> ~/.bashrc
		fi
fi

echo "==========================jdk & git & repo install success=========================== "

#apply sony Gerrit license
ssh-keygen
echo "please copy the following public key to sony Gerrit"
echo "=================================================="
cat /home/$home_dir/.ssh/id_rsa.pub
echo "=================================================="


echo "send mail to CM , CC PM and leader to open authority"

#run sony project setup-env scripts
git clone ssh://semc@192.168.11.15/semc-dev.git
cd Semc-dev
/bin/sh install-dev-env.sh

