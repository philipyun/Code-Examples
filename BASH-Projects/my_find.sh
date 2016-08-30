#!/bin/bash
    
params=($*)
directories=()
results=()
to_exec=()

predicate_order=""

types=""
names=""
predicates=""

type_count="0"
name_count="0"
last_directory="0"

find_on="0"

function parse()
{
    for ((i=0; i < ${#params[@]}; i++)); do
        if [ ${params[i]} == "-type" ]; then
            types+=${params[++i]}
            types+=" "
            predicate_order+="find\n"

        elif [ ${params[i]} == "-name" ]; then
            names+=${params[++i]}
            names+=" "
            predicate_order+="find\n"

        elif [ ${params[i]} == "-exec" ]; then
            predicate_order+="exec\n"
            for ((i=++i; i < ${#params[@]}; i++)); do
                to_exec+=(${params[i]})
            done

        elif [ ${params[i]} == "-print" ]; then
            predicate_order+="print\n"

        else
            if [ $i -eq $last_directory ]; then
                directories+=(${params[i]})
                ((last_directory++))
            elif [ $i -gt $last_directory ]; then
                echo "my_find: paths must precede expression"
                exit 1
            fi
        fi
    done
}


function set_flags()
{
    if [ -z "$types" ]; then
        types+="e"
    fi

    if [ -z "$names" ]; then
        names+="*"
    fi

    type_count=$(printf "$types" | sed 's/ /\n/g' | sort -u | wc -l)
    name_count=$(printf "$names" | sed 's/ /\n/g' | sort -u | wc -l)
    predicates=$(printf "$predicate_order" | uniq)

}


function walk_flags()
{
    directory=$1

    if [ $type_count == "1" ] && [ $name_count == "1" ]; then
        if [ -$types ${directory} ]; then
            results+=$(echo "$directory " | sed 's/\/\//\//g')
        fi
    fi

    for f in $directory/*; do
        if [ -d ${f} ]; then
            walk_flags $f
        fi
        if [ $type_count == "1" ] && [ $name_count == "1" ]; then
            if [ -$types ${f} ]; then
                results+=$(echo "$f " | sed 's/\/\//\//g')
            fi
        fi
    done
}


function walk()
{
    directory=$1
    echo $(echo "$directory " | sed 's/\/\//\//g')
    for f in $directory/*; do
        if [ -d ${f} ]; then
            walk $f
        fi
        echo $(echo "$f " | sed 's/\/\//\//g')
    done
}


function execute()
{
    cmd=$(echo "${to_exec[*]}" | sed "s@{}@${results[*]}@g")
    eval $cmd
    exit 0
}


function debug_info()
{  
    echo "type count is: " $type_count
    echo "name count is: " $name_count
    echo "print count is: " $print_count

    echo ""

    echo dirs: ${directories[*]}
    echo exec: ${to_exec[*]}
    echo ps: $predicates
    echo type: ${types[*]}
    echo name: ${names[*]}

    echo ""
}


function print()
{
    for r in ${results[*]}; do
        echo $r
    done
}


function process_preds()
{
    for p in ${predicates[*]}; do
        if [ $p == "find" ] || [ $p == "name" ]; then
            find_on="1"
        fi

        if [ $p == "print" ] && [ $find_on == "0" ]; then
            for dir in ${directories[*]}; do
                walk $dir
            done;
        fi

        if [ $p == "print" ] && [ $find_on == "1" ]; then
            for dir in ${directories[*]}; do
                walk_flags $dir
            done;
            print
        fi

        if [ $p == "exec" ]; then
            execute
        fi
    done
}



parse params
set_flags
process_preds

