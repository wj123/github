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

while read -r dir;
do
  echo cd $dir;
  cd $dir;
  check_res
  TMPOUT=$OUTDIR/$dir
  echo mkdir -p $TMPOUT
  mkdir -p $TMPOUT
  git diff --name-status $CURRTAG $BASETAG 2>$TMPOUT/error-msg | tee $TMPOUT/diff_files.txt
  cd -;
done < $(dirname $0)/bsp-folder-list.txt
