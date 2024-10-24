#!/bin/bash

make -C shared
make -C server
make -C client

tmux new-session -d -s test_session

tmux split-window -h
tmux split-window -v
tmux select-pane -t 0
tmux split-window -v

tmux send-keys -t 0 "cd server && ./out/server.out 127.0.0.1 8080" C-m

sleep 0.1
tmux send-keys -t 1 "cd client && ./out/client.out 127.0.0.1 8080" C-m
sleep 0.1
tmux send-keys -t 2 "cd client && ./out/client.out 127.0.0.1 8080" C-m
sleep 0.1
tmux send-keys -t 3 "cd client && ./out/client.out 127.0.0.1 8080" C-m

tmux attach-session -t test_session
