#!/bin/bash
if [ $# -ne 2 ]
then
  echo "Usage :: $0 <Max Range> <path_to_file_to_store_random_numbers>"
  exit 1
fi
rm -rf $2
#for i in {1..$1}; do echo $RANDOM; done > $2 
for (( c=1; c<=$1; c++ ))
do
  echo $RANDOM >> $2;
done
