#!/bin/bash
nodes=16
let MAX=$nodes-1
bench="testRand"
proto="-T"
round=50

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
rm node.*
