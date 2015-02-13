#!/bin/sh
awk -F '=' '
BEGIN{rows=columns=displays=disprows=dispsegments=0}
/^COLUMNS=/{columns=$2}
/^ROWS=/{rows=$2}
/^DISPLAYS=/{displays=$2}
/^DISPROWS=/{disprows=$2}
/^DISPSEGMENTS=/{dispsegments=$2}
END {
  print "s2l"
  printf "%c",columns+48
  printf "%c",rows+48
  printf "%c",displays+48
  printf "%c",disprows+48
  printf "%c",dispsegments+48
  print " "
}
'
