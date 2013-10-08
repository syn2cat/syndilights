# this works with debian style linux
PATH=$PATH:~/nodejs/bin/
if [ "$(which x-terminal-emulator)" = "" ]
then
  xt=xterm
else
  xt=x-terminal-emulator
fi
if [ "$(which node)" = "" ]
then
  echo "cannot find nodejs program 'node' in your path"
  exit 1
fi
$xt -t "central frameserver" -e frameserver/frameserver &
$xt -t "webserver" -e "$(which node) displayclienthtml/ws_udp.js" &
sleep 5
chromium-browser displayclienthtml/visionneuse.html ||
   x-www-browser displayclienthtml/visionneuse.html &
$xt -t "client: plasma" -e "python clients/plasma.py" &
sleep 20
$xt -t "client: cellular" -e "python clients/cellular.py"
