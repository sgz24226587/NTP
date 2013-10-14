#!/bin/sh 

date=`date`

git add *.c  *.sh  README 

git commit -m " $date push"

git push origin master
