#!/bin/bash
while true
do
frame="$(
  echo "s2l"
  printf "<8<18 "
  printf "1"
  echo ""
  a=$(date +%S)
  a=$((0+a))
  j=0
  while [ $j -lt 8 ]
  do
    i=0
    while [ $i -lt 12 ]
    do
      n=$((j*8+i))
      if [ $n -eq $a ]
      then
        printf "zzz "
      else
        printf "000 "
      fi
      i=$((i+1))
    done
    j=$((j+1))
    echo ""   # next row
  done
  d=0
  while [ $d -lt 12 ]
  do
    s=0
    while [ $s -lt 8 ]
    do
      printf "1${s}1 "
      s=$((s+1))
    done
    d=$((d+1))
    echo ""    # next display
  done
)"
  echo "$frame"
  echo "$frame" | nc -w 1 -u localhost 4422
  sleep 1
done
