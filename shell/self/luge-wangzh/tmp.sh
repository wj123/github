#!/bin/bash

function usage()
{
  echo "Usage: $(basename $0) CONFFILE"
  exit
}

function check_res()
{
  if [ $? -ne 0 ]; then
    echo "Error!"
    exit;
  fi
}

OUTDIR=/home/wangzh/data/worklist/BSP/Fujitsu/Luge-kk/changed-all
INPUTDIR1=/home/wangzh/data/worklist/BSP/Fujitsu/Luge-kk/kk-luge-changed
INPUTDIR2=/home/wangzh/data/worklist/BSP/Fujitsu/Luge-kk/jb-luge-changed
INPUTDIR3=/home/wangzh/data/worklist/BSP/Fujitsu/Luge-kk/kk-squash-changed
while read -r dir;
do
  TMPOUT=$OUTDIR/$dir
  echo mkdir -p $TMPOUT
  mkdir -p $TMPOUT
  check_res
  cat $INPUTDIR1/$dir/diff_files.txt $INPUTDIR2/$dir/diff_files.txt $INPUTDIR3/$dir/diff_files.txt | awk '{print $2}' | sort -u > $TMPOUT/all_changed_files.txt
  check_res
done < $(dirname $0)/bsp-folder-list.txt
