#!/bin/sh 

date=`date`

git add *.c  *.sh 

git commit -m " $date push"

git push origin master
