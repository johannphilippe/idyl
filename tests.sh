#!/bin/bash 

for filename in tests/*; do 
    echo "Test : ${filename}" 
    if(./build/idyl $filename ) 
    then 
        echo ">> Success <<"
    else 
        echo "!! Failure to parse ${filename} !!"
        exit 
    fi
    echo ""
done