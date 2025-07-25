#!/bin/bash

for file in * 
do 
  echo $file
  ssconvert --export-type=Gnumeric_stf:stf_csv $file
  
  #newname=`echo $oldname | sed -e 's/ //g'` 
  #mv "$oldname" "$newname" 
done

