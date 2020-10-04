#!/bin/bash

# This script executes all the benchmarks of the repository. For that, the
# "exec*.maude" files of each directory are executed and some string processing
# is performed to generate the CSV files.

#Maude's Executable (include the PATH if needed, e.g., /home/guest/maude/maude)
MAUDE=maude

####################
#Checking for Maude
####################
if type $MAUDE &> /dev/null ; then
    echo "[OK] Maude found."

    VMAUDE=`$MAUDE --version`

    echo "Maude version: $VMAUDE"

    if [ $VMAUDE != "3.0" ]; then
        echo "[Warning] RESPEG was tested with the version 3.0 of Maude"
    fi
else
    echo "[Fail] Maude was not found!"
    echo "Configure property the variable MAUDE in line 7."
    exit
fi
##################


#################################
# Executing one of the benchmarks
#################################
# @param1: exec.maude file
# @param2: csv file to be generated

exec_benchmark(){
    FILEEXEC=$1
    FILECSV=$2

    echo "Running benchmark $FILEEXEC"
    
    filenames=`grep erew $FILEEXEC`
    nfiles=$(wc -l <<<"$filenames") 
    let nfiles=nfiles/2


    echo "Analyzing $nfiles files"

    result=("File;Status;Steps;Status with cuts;Steps with cuts")

    cat /dev/null > $FILECSV

    i=1

    while read line
    do
        f=`echo $line | sed 's/.*\"\(.*\)\".*/\1/'`
        result+=("$f ; ")
        let i+=1
        if [ "$i" -gt "$nfiles" ];then
            break
        fi
    done < <(grep erew $FILEEXEC)


    i=1
    while read line
    do
        r=$(echo $line | cut -c15-)

        if [[ $r == "{"* ]]; then
            # Positive result
            r=`echo $r | sed 's/.*{\(.*\)}/\1/'`
        else
            
            if [[ $r == "error"* ]]; then
                #Output error
                r="error ; "`echo $r | sed 's/.*(\(.*\))/\1/'`
            else
                #Output fail
                r="fail ; "`echo $r | sed 's/.*(\(.*\))/\1/'`
            fi

        fi

        if [ "$i" -gt "$nfiles" ];then
            # With cuts
            idx=$( expr $i - $nfiles )
            result[$idx]="${result[$idx]} $r"
        else
            # Without cuts
            result[$i]="${result[$i]} $r"
        fi
        let i+=1
    done < <($MAUDE $FILEEXEC | grep result)

    for i in "${result[@]}"
    do
       echo "$i" >> $FILECSV
    done

    echo "[Done] Results in $FILECSV"
}

############################
# Running all the benchmarks
############################

exec_benchmark "./pascal/exec.maude" "./pascal/results.csv"
exec_benchmark "./json/exec.maude" "./json/results.csv"
exec_benchmark "./c89/exec-bench1.maude" "./c89/results1.csv"
exec_benchmark "./c89/exec-bench2.maude" "./c89/results2.csv"
exec_benchmark "./pallene/exec.maude" "./pallene/results.csv"
exec_benchmark "./anbncn/exec.maude" "./anbncn/results.csv"

