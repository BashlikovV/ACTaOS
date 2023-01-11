#!/bin/bash

find $1 -type f -exec grep "$2" {} \; -printf '%p\t%s byte\n' 

#grep - поиск строки в файле
#find - поиск файлов ()
#-type f - тип файл
#-exec - выполнить grep $2
#./LB5.sh /home/bashlykovvv/Test/bashLB/test_data H - str 
