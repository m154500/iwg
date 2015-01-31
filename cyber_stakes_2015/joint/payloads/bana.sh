#!/bin/bash
while :
    do
    for i in  {3..4} {6..8}
    do
        echo "Trying $i at `date`"
        flag=$(python bana.py $i)
        echo $flag
        curl -d 'apikey=cfe4d033170d8a56c327c620b05dc6c3b460f22de0087316d9' -d "flag=$flag" https://cyberstakesonline.com/liveapi/2/submit 2> curllog
        echo
    done
    sleep 5m
done
