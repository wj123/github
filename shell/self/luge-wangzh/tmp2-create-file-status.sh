#!/bin/bash

function usage()
{
  echo "Usage: $(basename $0) CONFFILE"
  exit
}

if [ $# -ne 1 ]; then
  usage;
fi

if [ ! -f $1 ]; then
  echo "file $1 does not exist";
  exit;
fi

source $1

if [ "$BASETAG" = "" ]; then
  echo "BASETAG value invalid"
  exit;
elif [ "$CURRTAG" = "" ]; then
  echo "CURRTAG value invalid"
  exit;
elif [ ! -d $OUTDIR ]; then
  echo "dir $OUTDIR does not exist"
  exit;
fi

function check_res()
{
  if [ $? -ne 0 ]; then
    echo "Error!"
    exit;
  fi
}

INDIR=/home/wangzh/data/worklist/BSP/Fujitsu/Luge-kk/changed-all
while read -r dir;
do
  echo cd $dir;
  cd $dir;
  check_res
  for f in $(cat $INDIR/$dir/all_changed_files.txt); do
    output=$(git diff --name-status $BASETAG $CURRTAG -- $f)
    if [ "$output" = "" ]; then
      output="-       $f"
    fi
    echo $output >> $OUTDIR/$dir/all-diff-files.txt
  done
  cd -;
done < $(dirname $0)/bsp-folder-list.txt
