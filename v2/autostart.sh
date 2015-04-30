#!/bin/bash

set -e
set -x

SESSIONNAME="syndilight"

tmux new-session -s $SESSIONNAME -n forwarder -d
tmux send-keys -t $SESSIONNAME ". /home/pi/gits/syndilights/v2/virtenv/bin/activate" C-m
tmux send-keys -t $SESSIONNAME "/home/pi/gits/syndilights/v2/backend/forwarding/forward.py " C-m
sleep 15
tmux split-window -t $SESSIONNAME -h
tmux send-keys -t $SESSIONNAME ". /home/pi/gits/syndilights/v2/virtenv/bin/activate" C-m
tmux send-keys -t $SESSIONNAME "/home/pi/gits/syndilights/v2/backend/forwarding/receiver.py " C-m

