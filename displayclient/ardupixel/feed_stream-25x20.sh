#!/bin/bash
# test program
# modify below the ethernet adress
IP="$1"
# width and height are unfortunately hardcoded
a=-1
while true
do
a=$((a+1))
frame="$(
  echo "s2l"
  printf "DI000 "
#          55518 "
  printf "1"
  echo ""
  #a=$(date +%S)
  #a=$((0+a))
  j=0
  while [ $j -lt 20 ]
  do
    i=0
    while [ $i -lt 25 ]
    do
      n=$((i*5+j))
      if [ $n -eq $a ]
      then
        printf "xyz "
      else
        printf "    "
      fi
      i=$((i+1))
    done
    j=$((j+1))
    echo ""   # next row
  done
  d=0
#  while [ $d -lt 12 ]
#  do
#    s=0
#    while [ $s -lt 8 ]
#    do
#      printf "1${s}1 "
#      s=$((s+1))
#    done
#    d=$((d+1))
#    echo ""    # next display
#  done
)"
  echo "$frame"
  echo "$frame" | nc -w 1 -u $IP 8888
#  sleep 1
done
