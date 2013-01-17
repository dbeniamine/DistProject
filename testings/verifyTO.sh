#!/bin/bash
if [ -z $3 ]
then
    echo "usage $0 nNodes nRounds proto maxBroadcast"
    echo "nNodes        : int >0"
    echo "nRounds       : int >0"
    echo "proto         : -L | -T"
    echo "maxBroadcast  : int >0"
    exit 1
fi
nodes=$1
round=$2
proto=$3
maxBroadcast=$4
let MAX=$nodes-1
bench="RandBench"
let nbBroadcast=$RANDOM%$maxBroadcast
i=0
##generate the random bench
#rm $bench
#while [ $nbBroadcast -gt 0 ] || [ $i -lt $round ]
#do
#    let bool=$RANDOM%2
#    if [ $bool -ne 1 ] && [ $nbBroadcast -gt 0 ]
#    then
#        let init=$RANDOM%$nodes
#        echo "$init broadcast" >> $bench
#        let nbBroadcast=$nbBroadcast-1
#    else
#        echo "start" >> $bench
#        let i=$i+1
#    fi
#done
##just to be sure there is enough rounds at the end, we start 4 N rounds
#for i in `seq 0 $MAX`
#do
#    echo "start" >> $bench
#    echo "start" >> $bench
#    echo "start" >> $bench
#    echo "start" >> $bench
#done
##do the experiment
##rm node.*
for i in `seq 0 $MAX`
do
    ../src/Broadcast $proto  -N $nodes -R $round < $bench |\
        grep "Delivered" | grep "by $i," |\
        sed "s/ by [0-9]*, sender: [0-9]*//" > node.$i
done
ok=0
for i in `seq 0 $MAX`
do
    let j=($i+1)%$MAX
    ret=`diff node.$i node.$j`
    if [ ! -z "$ret" ]
    then 
        let ok=1
    fi
done
if [ $ok -ne 1 ]
then
    echo "ok"
else
    echo "fail"
fi
#rm node.*
