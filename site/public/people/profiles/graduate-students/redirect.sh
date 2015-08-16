#!/bin/bash

for file in *.html
do
    name=`basename $file .html`
    name="graduate-students/"$name
    mindex.sh $name > $file
done

for file in */index.html
do
    name=`echo $file | sed 's:\/index\.html::g'`
    name="graduate-students/"$name
    mindex.sh $name > $file
done
